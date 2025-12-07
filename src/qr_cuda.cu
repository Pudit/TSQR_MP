#include "qr_cuda.h"
#include "mat.h"
// #include "qr.h"
#include <vector>

__global__ void qr_test_kernel(const int h, const int w, double *buf_in, double *buf_out) 
{
    // only one thread 
    // printf("blockIdx.x  = %d", blockIdx.x );
    if (blockIdx.x == 0 && threadIdx.x == 0) {
        // qr_device(h, w, buf_in, buf_out);
    }
}

void test_qr(std::vector<double> A, const int h, const int w)
{
    
    double *d_A, *d_out;

    // for (int i = 0; i < h; i++) {
    //     for (int j = 0; j < w; j++) {
    //         printf("%lf, ", A[i * w + j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n-------------\n");

    cudaMalloc(&d_A,   sizeof(double) * h * w);
    cudaMalloc(&d_out, sizeof(double) * (h*w + w*w));

    cudaMemcpy(d_A, A.data(),
               sizeof(double) * h * w,
               cudaMemcpyHostToDevice);

    // qr_test_kernel<<<1,1>>>(h, w, d_A, d_out);
    int m = h; 
    int n = w;

    size_t shared_size = m * sizeof(double) + (m * m) * sizeof(double) + (m * n) * sizeof(double);
    qr_test_kernel<<<1,1, 64*shared_size>>>(h, w, d_A, d_out);
    
    // cudaError_t err = cudaGetLastError();
    // printf("%s", cudaGetErrorString(err));

    cudaDeviceSynchronize();

    // Copy result back
    std::vector<double> out(w*w);

    cudaMemcpy(out.data(), d_out,
               sizeof(double) * ( w*w),
               cudaMemcpyDeviceToHost);
    
    // Print R
    std::cout << "\nR:\n";
    double *R = out.data();
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < w; j++)
            std::cout << R[i*w + j] << " ";
        std::cout << "\n";
    }

    cudaFree(d_A);
    cudaFree(d_out);
}


__device__ void qr_device(const int h, const int w, double * buf_in, double * buf_out, double * v) {

    int m = h;
    int n = w;
    
    // Work directly with R (m x n input, n x n output eventually)
    double *R_full = buf_in;  // Use input buffer as R
    
    // Allocate temporary storage for Householder vectors and matrices
    // extern __shared__ char shared_mem[];
    // double *v = (double*)shared_mem;  // size m
    double *H_data = v + m;  // size m*m
    double *temp_R = H_data + m*m;  // size m*n
    
    Matrix R(R_full, m, n);
    Matrix H(H_data, m, m);
    Matrix temp_R_mat(temp_R, m, n);

    // QR factorization loop
    for (int k = 0; k < n && k < m; k++) {
        
        // Compute norm of column k (from row k to m)
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

        // Normalize v
        double v_norm = 0.0;
        for (int i = k; i < m; i++) {
            v_norm += v[i] * v[i];
        }
        v_norm = sqrt(v_norm);

        
        for (int i = k; i < m; i++) {
            v[i] /= v_norm;
        }
       

        // Compute H = I - 2 vv^T
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