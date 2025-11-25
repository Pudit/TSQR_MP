#include <iostream>
#include "tsqr.h"

int main() {
    Matrix A = {
        {1, 2},
        {3, 4},
        {5, 6},
        {7, 8}, 
    };

    A = {
        {1, 2, 3, 4, 1, 1, 1, 1},
        {5, 6, 7, 8, 1, 1, 1, 1}, 
        {1, 2, 3, 4, 1, 1, 1, 1},
        {5, 6, 7, 8, 1, 1, 1, 1}, 
        {1, 2, 3, 4, 1, 1, 1, 1},
        {5, 6, 7, 8, 1, 1, 1, 1}, 
        {1, 2, 3, 4, 1, 1, 1, 1},
        {5, 6, 7, 8, 1, 1, 1, 1}, 
    };
    
    // Matrix t= partial_diag_multiply(A,A,A);
    // print_matrix(t);
    // exit(0);

    Matrix Q, R;
    tsqr(A, Q, R, 2);

    std::cout << "Q = \n";
    for(auto &row : Q) {
        for(double x : row) std::cout << x << " ";
        std::cout << "\n";
    }

    std::cout << "\nR = \n";
    for(auto &row : R) {
        for(double x : row) std::cout << x << " ";
        std::cout << "\n";
    }

    Matrix A_recon = matmul(Q, R);
    std::cout << "\nA_recon = \n";
    for(auto &row : A) {
        for(double x : row) std::cout << x << " ";
        std::cout << "\n";
    }

    Matrix QQT = matmul(transpose(Q), Q);
    std::cout << "\nQQ.T = \n";
    for(auto &row : QQT) {
        for(double x : row) std::cout << x << " ";
        std::cout << "\n";
    }
}