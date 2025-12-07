#pragma once

#include <cuda_runtime.h>


void check_cuda();

__global__ void qr_on_blocks(double *A_global, double *R_global, int m, int n, int block_height);

void test_tsqr(std::vector<double> A, const int m, const int n, const int block_height);

