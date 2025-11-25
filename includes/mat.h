#pragma once

#include <vector>
#include <math.h>
#include <iostream>

using Matrix = std::vector<std::vector<double>>;

/*
matmul 
params 
    A = R ^ (m x k)
    B = R ^ (k x n)
return 
    C = A x B = R ^ (m x k) x (k x n) = R ^ (m x n)
*/
inline Matrix matmul(const Matrix &A, const Matrix &B) {
    int m = A.size(), k = A[0].size(), n = B[0].size();

    // C = R ^ (m x n)
    Matrix C(m, std::vector<double>(n, 0.0));

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            for (int t = 0; t < k; t++) {
                C[i][j] += A[i][t] * B[t][j];
            }
        }
    }

    return C;
}

// transpose A (m x n) -> A.T (n x m)
inline Matrix transpose(const Matrix &A) {
    int m = A.size(), n = A[0].size();

    Matrix AT(n, std::vector<double>(m));

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            AT[j][i] = A[i][j];
        }
    }

    return AT;
}

// identity (n x n)
inline Matrix identity(int n) {
    Matrix I(n, std::vector<double>(n, 0.f));

    for (int i = 0; i < n; i++) I[i][i] = 1.f;

    return I;
}

// print matrix 
inline void print_matrix(const Matrix &A) {
    for (const std::vector<double> &row : A) {
        for (const double d : row) {
            std::cout << d << " "; 
        }
        std::cout << "\n";
    }
}

// 
inline Matrix partial_diag_multiply(const Matrix &Q0, const Matrix &Q1, const Matrix &Q01) {
    int m = Q0.size();
    int n = Q0[0].size();
    
    Matrix Q_out(2*m, std::vector<double>(Q01[0].size(), 0.0));
    // Q01[:m] = matmul(Q0, Q01[:m]);
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < n; k++) {
            for(int j = 0; j < Q01[0].size(); j++) {
                Q_out[i][j] += Q0[i][k] * Q01[k][j];
            }
        }
    }

    // Q01[m:]
    for (int i = 0; i < m; i++) {
        for (int k = 0; k < n; k++) {
            for(int j = 0; j < Q01[0].size(); j++) {
                Q_out[i + m][j] += Q1[i][k] * Q01[k + n][j];      
            }
        }
    }
    
    return Q_out;
}