#pragma once

#include <vector>
#include <math.h>
#include <iostream>

struct Matrix 
{
    double *ptr;
    size_t h, w;
    bool is_transpose;

    Matrix(double *ptr, size_t h, size_t w): ptr(ptr), h(h), w(w), is_transpose(false) {}

    double& operator()(int i, int j)
    {
        if (!is_transpose)
            return ptr[i*w + j];
        else 
            return ptr[j*h + i];
    }

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
inline void matmul(Matrix A, Matrix B, Matrix C) 
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

inline std::vector<double> identity(int n) {
    std::vector<double> v(n*n, 0);
    
    Matrix m(v.data(), n, n);
    for(int i = 0; i < n; i++) m(i, i) = 1;

    return v;
}

// // identity (n x n)
// inline Matrix identity(int n) {
//     Matrix I(n, std::vector<double>(n, 0.f));

//     for (int i = 0; i < n; i++) I[i][i] = 1.f;

//     return I;
// }


// print matrix 
inline void print_matrix( Matrix A) {
    for (int i = 0; i < A.h; i++) {
        for (int j = 0; j < A.w; j++) {
            std::cout << A(i, j) << " ";
        }
        std::cout << "\n";
    }
}


// 
// inline Matrix partial_diag_multiply(const Matrix &Q0, const Matrix &Q1, const Matrix &Q01) {
//     int m = Q0.size();
//     int n = Q0[0].size();
    
//     Matrix Q_out(2*m, std::vector<double>(Q01[0].size(), 0.0));
//     // Q01[:m] = matmul(Q0, Q01[:m]);
//     for (int i = 0; i < m; i++) {
//         for (int k = 0; k < n; k++) {
//             for(int j = 0; j < Q01[0].size(); j++) {
//                 Q_out[i][j] += Q0[i][k] * Q01[k][j];
//             }
//         }
//     }

//     // Q01[m:]
//     for (int i = 0; i < m; i++) {
//         for (int k = 0; k < n; k++) {
//             for(int j = 0; j < Q01[0].size(); j++) {
//                 Q_out[i + m][j] += Q1[i][k] * Q01[k + n][j];      
//             }
//         }
//     }
    
//     return Q_out;
// }