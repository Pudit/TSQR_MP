#include "qr.h"

void qr(const Matrix &A, Matrix &Q, Matrix &R) {
    int m = A.size();
    int n = A[0].size();

    // Work with full-size matrices during computation
    Matrix Q_full = identity(m);
    R = A;

    for (int k = 0; k < n; k++) {
        // compute norm of column k (from row k to m)
        double x_norm = 0.0;
        for (int i = k; i < m; i++) {
            x_norm += R[i][k] * R[i][k];
        }
        x_norm = std::sqrt(x_norm);

        double alpha = (R[k][k] > 0.0) ? -x_norm : x_norm;

        std::vector<double> v(m, 0.0);
        for (int i = k; i < m; i++) v[i] = R[i][k];
        v[k] -= alpha;

        // normalize v
        double v_norm = 0.0;
        for (double x : v) v_norm += x * x;
        v_norm = std::sqrt(v_norm);
        if (v_norm > 1e-10) {
            for (double &x : v) x /= v_norm;
        }

        // compute H = I - 2 vv^T
        Matrix H = identity(m);
        for (int i = 0; i < m; i++)
            for (int j = 0; j < m; j++)
                H[i][j] -= 2 * v[i] * v[j];

        R = matmul(H, R);
        Q_full = matmul(Q_full, H);
    }

    // Extract reduced Q (m x n) - first n columns of Q_full
    Q = Matrix(m, std::vector<double>(n));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            Q[i][j] = Q_full[i][j];
        }
    }

    // Extract reduced R (n x n) - first n rows of R
    Matrix R_reduced(n, std::vector<double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            R_reduced[i][j] = R[i][j];
        }
    }
    R = R_reduced;
}