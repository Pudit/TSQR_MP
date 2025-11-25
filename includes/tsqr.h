#ifndef TSQR_H
#define TSQR_H

#include "mat.h"

/**
 * Tall-Skinny QR decomposition using tree-based approach
 * 
 * Computes A = QR where:
 * - Q is an m x n orthogonal matrix (m >= n)
 * - R is an n x n upper triangular matrix
 * 
 * The algorithm divides A into blocks, computes QR for each block,
 * then recursively combines the R factors.
 * 
 * @param A Input matrix (m x n, m >= n)
 * @param Q Output orthogonal matrix (m x n)
 * @param R Output upper triangular matrix (n x n)
 * @param block_size Size of blocks for decomposition (default: 2*n)
 */
void tsqr(const Matrix &A, Matrix &Q, Matrix &R, int block_size = -1);

// void tsqr(double *A, double *Q, double *R, size_t width, size_t height);

#endif // TSQR_H