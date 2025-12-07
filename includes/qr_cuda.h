#include <iostream>
#include <cuda_runtime.h>
#include <vector>

void test_qr(std::vector<double> A, const int h, const int w);

__device__ void qr_device(const int h, const int w, double * buf_in, double * buf_out, double * v);