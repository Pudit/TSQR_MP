#include <cuda_runtime.h>
#include <iostream>
#include <vector>

#include "tsqr_cuda.h"
#include "qr_cuda.h"

#include "mat.h"

__global__ void qr_on_blocks(double *A_global, double *R_global, int m, int n, int block_height);
__global__ void qr_on_stacks(double *R_in, double *R_out, int num_blocks, int n);
__device__ void qr_device(const int h, const int w, double * buf_in, double * buf_out, double * v);

void check_cuda() {
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);

    for (int i = 0; i < deviceCount; i++) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);

        int coresPerSM = 0;

        // Convert SM version to cores per SM (Turing)
        if (prop.major == 7) {  // Turing
            coresPerSM = 64;    // Turing has 64 CUDA cores per SM
        } else {
            // For other architectures, you'd need a mapping table
        }

        int totalCores = coresPerSM * prop.multiProcessorCount;
        std::cout << "GPU " << i << ": " << prop.name
                  << " has " << totalCores << " CUDA cores\n";
    }
}

__global__ void qr_on_stacks(double *R_in, double *R_out, int num_blocks, int n) {

    int block_id = blockIdx.x;
    int idx = block_id * 2;

    if (idx + 1 >= num_blocks) return;

    double *R1 = R_in + idx * n * n;
    // double *R2 = R1 + idx * n * n; 
    double *R2 = R_in + (idx + 1) * n * n;


    double *R12_out = R_out + block_id * n * n;

    extern __shared__ char shared_mem[];
    double *R12 = (double*)shared_mem;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            R12[i * n + j] = R1[i * n + j];
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            R12[n * n + i * n + j] = R2[i * n + j];
        }
    }
    
    double *v = R12 + 2 * n * n;

    qr_device(2 * n, n, R12, R12_out, v);

}


void test_tsqr(std::vector<double> A, const int m, const int n, const int block_height)
{
    // inputs
    if (block_height <= 0 || block_height > m) {
        std::cerr << "Invalid block_height: " << block_height << "\n";
        return;
    }

    int num_blocks = (m + block_height - 1) / block_height;
    
    std::cout << "TSQR Test\n";
    std::cout << "Matrix: " << m << " x " << n << "\n";
    std::cout << "Block height: " << block_height << "\n";
    std::cout << "Number of blocks: " << num_blocks << "\n\n";

    /////////////////////////////////////////////////////////////////////////////////////////////////

    // allocate device memory
    double *d_A, *d_R_blocks, *d_R_next = nullptr;

    cudaMalloc(&d_A,          sizeof(double) * m * n);
    cudaMalloc(&d_R_blocks,   sizeof(double) * num_blocks * n * n);  // Each block produces n x n R
    // cudaMalloc(&d_R_stacked,  sizeof(double) * num_blocks * n * n);  // Stacked R matrices (num_blocks*n x n)
    // cudaMalloc(&d_R_final,    sizeof(double) * n * n);               // Final R matrix
    
    // Copy input matrix to device
    cudaMemcpy(d_A, A.data(), sizeof(double) * m * n, cudaMemcpyHostToDevice);
    

    size_t shared_size = (block_height + block_height * block_height + block_height * n) * sizeof(double);    
    qr_on_blocks<<<num_blocks, 1, 64 * shared_size>>>(d_A, d_R_blocks, block_height, n, block_height);
    cudaDeviceSynchronize();

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        std::cerr << "CUDA error in Stage 1: " << cudaGetErrorString(err) << "\n";
        return;
    }

    printf("done qr on blocks\n");



    // merge and reduce
    while(num_blocks > 1) {
        num_blocks /= 2;

        cudaMalloc(&d_R_next, sizeof(double) * num_blocks * n * n);

        size_t shared_mem_reduce = (n + 2 * n * n) * sizeof(double);
        qr_on_stacks<<<num_blocks, 1, 64 * shared_mem_reduce>>>(d_R_blocks, d_R_next, num_blocks * 2, n);
        cudaDeviceSynchronize();

        cudaFree(d_R_blocks);
        d_R_blocks = d_R_next;

    }


    
    // print_matrix(Matrix(d_R_blocks, n, n));
    double *d_R_cpu = new double[n * n];
    cudaMemcpy(d_R_cpu, d_R_blocks, sizeof(double) * n * n, cudaMemcpyDeviceToHost);
    
    printf("\nR:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%lf ", d_R_cpu[i * n + j]);
        }
        printf("\n");
    }

    // ============ cleanup ============
    cudaFree(d_A);
    cudaFree(d_R_blocks);
   

    delete[] d_R_cpu;

}

__global__ void qr_on_blocks(double *A_global, double *R_global, int m, int n, int block_height) {
    int block_id = blockIdx.x;
    
    int start_id = block_id * block_height * n;
    
    double *A_block = A_global + start_id;
    double *R_block = R_global + block_id * n * n;


    extern __shared__ char shared_mem[];
    double *v = (double*)shared_mem;  // size m


    qr_device(m, n, A_block, R_block, v);


    
}


__device__ void qr_device(const int h, const int w, double * buf_in, double * buf_out, double * v) {

    int m = h;
    int n = w;
    
    // Work directly with R (m x n input, n x n output eventually)
    double *R_full = buf_in;  // Use input buffer as R
    
    // allocate temporary storage for Householder vectors and matrices
    // extern __shared__ char shared_mem[];
    // double *v = (double*)shared_mem;  // size m
    double *H_data = v + m;  // size m*m
    double *temp_R = H_data + m*m;  // size m*n
    
    Matrix R(R_full, m, n);
    Matrix H(H_data, m, m);
    Matrix temp_R_mat(temp_R, m, n);

    // QR factorization 
    for (int k = 0; k < n && k < m; k++) {
        
        // norm of column k (from row k to m)
        double x_norm = 0.0;
        for (int i = k; i < m; i++) {
            x_norm += R(i, k) * R(i, k);
        }
        x_norm = sqrt(x_norm);

        double alpha = (R(k, k) > 0.0) ? -x_norm : x_norm;

        // Compute Householder vector v
        for (int i = 0; i < m; i++) {
            if (i < k) {
                v[i] = 0.f;
            } else {
                v[i] = R(i, k);
            }
            
        }
        v[k] -= alpha;

        // norm v
        double v_norm = 0.0;
        for (int i = k; i < m; i++) {
            v_norm += v[i] * v[i];
        }
        v_norm = sqrt(v_norm);

        
        if (v_norm > 1e-10) {
            for (int i = k; i < m; i++) {
                v[i] /= v_norm;
            }
        }
       

        //  H = I - 2 vv^T
        identity_device(H_data, m);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < m; j++) {
                H(i, j) -= 2 * v[i] * v[j];
            }
        }


        // temp_R_mat = H * R
        matmul_device(H, R, temp_R_mat);
        
        // Copy temp_R back to R
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                R(i, j) = temp_R_mat(i, j);
            }
        }
        
    }

    
    // Extract upper triangular R (n x n)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            buf_out[i * n + j] = R(i, j);      
           
        }
        
    }

}