#include "qr.h"

// void qr(Matrix &A, Matrix &Q, Matrix &R) 
// {
//     int m = A.h; 
//     int n = A.w; 

//     // Create a temporary full R matrix (m x n)
//     std::vector<double> R_full_data(m * n);
//     Matrix R_full(R_full_data.data(), m, n);
    
//     // Copy A to R_full
//     for (int i = 0; i < m; i++) {
//         for (int j = 0; j < n; j++) {
//             R_full(i, j) = A(i, j);
//         }
//     }

//     printf("R_full = \n");
//     print_matrix(R_full);


//     // Initialize Q_full as identity matrix (m x m)
//     std::vector<double> Q_full_data(m * m);
//     Matrix Q_full(Q_full_data.data(), m, m);
//     for (int i = 0; i < m; i++) {
//         for (int j = 0; j < m; j++) {
//             Q_full(i, j) = (i == j) ? 1.0 : 0.0;
//         }
//     }


//     for (int k = 0; k < n && k < m; k++) 
//     {
//         // Compute norm of the k-th column below diagonal
//         double x_norm = 0.0;
//         for (int i = k; i < m; i++)
//         {
//             x_norm += R_full(i, k) * R_full(i, k);
//         }
//         x_norm = std::sqrt(x_norm);

//         // if (x_norm < 1e-10) continue;

//         // Compute alpha with sign convention
//         double alpha = (R_full(k, k) > 0.0) ? -x_norm : x_norm;

//         // Compute the Householder vector v
//         std::vector<double> v(m - k);
//         v[0] = R_full(k, k) - alpha;
//         for (int i = 1; i < m - k; i++) {
//             v[i] = R_full(k + i, k);
//         }

//         // Compute v_norm for normalization
//         double v_norm_sq = 0.0;
//         for (int i = 0; i < m - k; i++) {
//             v_norm_sq += v[i] * v[i];
//         }

//         // if (v_norm_sq < 1e-10) continue;

//         // Apply Householder reflection to R_full
//         for (int j = k; j < n; j++) {
//             double vTR = 0.0;
//             for (int i = 0; i < m - k; i++) {
//                 vTR += v[i] * R_full(k + i, j);
//             }

//             double factor = 2.0 * vTR / v_norm_sq;
//             for (int i = 0; i < m - k; i++) {
//                 R_full(k + i, j) -= factor * v[i];
//             }
//         }

//         // Apply Householder reflection to Q_full (all columns, not just first n)
//         for (int j = 0; j < m; j++) {
//             double vTQ = 0.0;
//             for (int i = 0; i < m - k; i++) {
//                 vTQ += v[i] * Q_full(k + i, j);
//             }

//             double factor = 2.0 * vTQ / v_norm_sq;
//             for (int i = 0; i < m - k; i++) {
//                 Q_full(k + i, j) -= factor * v[i];
//             }
//         }
//     }

//     // Extract the reduced R (first n rows, n columns)
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             R(i, j) = R_full(i, j);
//         }
//     }

//     // Extract the reduced Q (first n columns of Q_full)
//     for (int i = 0; i < m; i++) {
//         for (int j = 0; j < n; j++) {
//             Q(i, j) = Q_full(i, j);
//         }
//     }
// }

void qr(Matrix &A, Matrix &Q, Matrix &R) {
    int m = A.h; 
    int n = A.w; 

    // Work with full-size matrices during computation
    std::vector<double> q_full = identity(m);
    Matrix Q_full(q_full.data(), m, m);


    std::vector<double> R_full_data(m * n);
    Matrix R_full(R_full_data.data(), m, n);
    
    // Copy A to R_full
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            R_full(i, j) = A(i, j);
        }
    }


    for (int k = 0; k < n; k++) {
        
        // compute norm of column k (from row k to m)
        double x_norm = 0.0;
        for (int i = k; i < m; i++) {
            x_norm += R_full(i, k) * R_full(i, k);
        }
        x_norm = std::sqrt(x_norm);

        double alpha = (R_full(k, k) > 0.0) ? -x_norm : x_norm;


        std::vector<double> v(m, 0.0);
        for (int i = k; i < m; i++) 
        {
            v[i] = R_full(i, k);
        }
        v[k] -= alpha;


        // normalize v
        double v_norm = 0.0;
        for (double x : v) v_norm += x * x;
        v_norm = std::sqrt(v_norm);
        if (v_norm > 1e-10) {
            for (double &x : v) x /= v_norm;
        }


        // compute H = I - 2 vv^T
        
        std::vector<double> h = identity(m);
        Matrix H(h.data(), m, m);

        for (int i = 0; i < m; i++)
            for (int j = 0; j < m; j++)
                H(i, j) -= 2 * v[i] * v[j];
                

        // Need temporary storage
        Matrix temp_R(new double[m * n], m, n);
        Matrix temp_Q(new double[m * m], m, m);

        // R_full = H * R_full
        matmul(H, R_full, temp_R);
        // Copy temp_R back to R_full
        for (size_t i = 0; i < m; i++) {
            for (size_t j = 0; j < n; j++) {
                R_full(i, j) = temp_R(i, j);
            }
        }

        // Q_full = Q_full * H
        matmul(Q_full, H, temp_Q);
        // Copy temp_Q back to Q_full
        for (size_t i = 0; i < m; i++) {
            for (size_t j = 0; j < m; j++) {
                Q_full(i, j) = temp_Q(i, j);
            }
        }

        // free
        delete[] temp_R.ptr;
        delete[] temp_Q.ptr;
    }

    // Extract reduced Q (m x n) - first n columns of Q_full
    // Q = Matrix(m, std::vector<double>(n));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            Q(i, j)= Q_full(i, j);
        }
    }
  
    // Extract reduced R (n x n) - first n rows of R
    // Matrix R_reduced(n, std::vector<double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            R(i, j) = R_full(i, j);
        }
    }

}
