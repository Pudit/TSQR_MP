#pragma once

#include <vector>
#include <math.h>
#include <iostream>
#include <cuda_runtime.h>

struct Matrix 
{
    double *ptr;
    int h, w;
    bool is_transpose;

    __host__ __device__
    Matrix(double *ptr, int h, int w): ptr(ptr), h(h), w(w), is_transpose(false) {}

    __host__ __device__
    double& operator()(int i, int j)
    {
        if (!is_transpose)
            return ptr[i*w + j];
        else 
            return ptr[j*h + i];
    }

    __host__ __device__
    Matrix T() {
        Matrix m(ptr, w, h);
        m.is_transpose = !is_transpose;
        return m;
    }
 };

/*
matmul 
params 
    A = R ^ (m x k)
    B = R ^ (k x n)
return 
    C = A x B = R ^ (m x k) x (k x n) = R ^ (m x n)
*/
__device__ inline void matmul_device(Matrix A, Matrix B, Matrix C) 
{
    size_t m = A.h; 
    size_t k = A.w;
    size_t n = B.w;

    // Initialize C to zero
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            C(i, j) = 0;
        }
    }

    // Perform multiplication
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            for (size_t kk = 0; kk < k; kk++) {  // Use kk, iterate over k (not n!)
                C(i, j) += A(i, kk) * B(kk, j);
            }
        }
    }
}

__device__ inline void identity_device(double *buf, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            buf[i * n + j] = (i == j) ? 1.0 : 0.0;
        }
    }
}

// inline std::vector<double> identity(int n) {
//     std::vector<double> v(n*n, 0);
    
//     Matrix m(v.data(), n, n);
//     for(int i = 0; i < n; i++) m(i, i) = 1;

//     return v;
// }

// // identity (n x n)
// inline Matrix identity(int n) {
//     Matrix I(n, std::vector<double>(n, 0.f));

//     for (int i = 0; i < n; i++) I[i][i] = 1.f;

//     return I;
// }


// print matrix 
inline __device__  __host__ void print_matrix(Matrix A) {
    printf("\nh: %d, w: %d\n", A.h, A.w);
    for (int i = 0; i < A.h; i++) {
        for (int j = 0; j < A.w; j++) {
            // std::cout << A(i, j) << " ";
            printf("%lf ", A(i, j));
        }
        // std::cout << "\n";
        printf("\n");
    }
}

__global__ void print_matrix_kernel(double* d, int h, int w) {
    int block_id = blockIdx.x * blockDim.x + threadIdx.x;
    if (block_id != 0)
        return;

    Matrix A(d, h, w);
    print_matrix(A);
}

