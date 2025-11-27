#ifndef TSQR_H
#define TSQR_H

#include "mat.h"


void tsqr(std::vector<double> init_data, int world_size, int height, int width);

// void tsqr(double *A, double *Q, double *R, size_t width, size_t height);

#endif // TSQR_H