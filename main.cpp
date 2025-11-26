#include <iostream>
#include "tsqr.h"
#include "qr.h"

#include <chrono>

#include <random>

inline std::vector<double> generate_random_matrix(int m, int n, int seed) {
    std::mt19937 gen(seed);                     // Mersenne Twister engine
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::vector<double> A(m * n);

    for (int i = 0; i < m * n; ++i) {
        A[i] = dist(gen);
    }

    return A;
}



int main(int argc, char* argv[]) {

    int num_processors = 1;
    int height = 2;
    int width = 2;
    int random_seed = -1;   // -1 means not provided

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-p" && i + 1 < argc) {
            num_processors = std::stoi(argv[++i]);
        }
        else if (arg == "-h" && i + 1 < argc) {
            height = std::stoi(argv[++i]);
        }
        else if (arg == "-w" && i + 1 < argc) {
            width = std::stoi(argv[++i]);
        }
        else if (arg == "-r" && i + 1 < argc) {
            random_seed = std::stoi(argv[++i]);
        }
        else {
            std::cerr << "Unknown or incomplete argument: " << arg << std::endl;
            return 1;
        }
    }

    std::cout << "Processors: " << num_processors << "\n";
    std::cout << "Height (h): " << height << "\n";
    std::cout << "Width (w): " << width << "\n";

    
    // broken
    // Matrix A = {
    //     {1, 2},
    //     {3, 4},
    //     {5, 6},
    //     {7, 8},     
    //     {1, 2},
    //     {3, 4},
    //     {5, 6},
    //     {7, 8}, 
    // };

    // std::vector<double> A = {
    //     1, 2,
    //     3, 4,
    //     5, 6,
    //     7, 8,
    //     // 9, 10,
    //     // 11, 12,
    //     // 13, 14,
    //     // 15, 16,
    //     // 2352, 235,
    //     // -12, 12,
    //     // -1, -2,
    //     // -3, -4,
    // };

    std::vector<double> A = generate_random_matrix(height, width, 1);

    // std::vector<double> A = {
    //     1, 1,
    //     1, 1,
    //     1, 1,
    //     1, 1,
    // };

    // const int height = 8, width = 2;

    // const int num_processors = 1;

    //////////////////////////////////// tsqr
    std::vector<double> r( width * width, 0.f);
    std::vector<double> q(height * width, 0.f);

    Matrix A_full(A.data(), height, width);
    Matrix R(r.data(), width, width);
    Matrix Q(q.data(), height, width);
    
    auto start = std::chrono::high_resolution_clock::now();

    tsqr(A_full, Q, R, num_processors);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "TSQR elapsed time: " << elapsed.count() << " ms\n";

    // print_matrix(R);

    

    /////////////////////////////// qr
    
    // num_processors = 1;
    // print_matrix(A_full);
    // printf("----------------\n");
    // qr(A_full, Q, R);
    // print_matrix(R);

    // const int h_stride = height / num_processors;
    // std::vector<double> R1(num_processors * width * width, 0), 
    //                     R2(num_processors * width * width, 0);
    // printf("qr testing\n");
    // for(int i = 0; i < num_processors; i++)
    // {
    //     // Allocate Q with correct size: h_stride x width
    //     std::vector<double> q_tmp(h_stride * width, 0);

    //     Matrix A_slice(&A[i*h_stride*width], h_stride, width),
    //            R_slice(&R1[i*width*width], width, width),
    //            Q(&q_tmp[0], h_stride, width);  // Start from index 0
               
    //     std::cout << "-------------------------------\nA_slice " << i << " = \n";
    //     print_matrix(A_slice);

    //     qr(A_slice, Q, R_slice);

    //     std::cout << "\nQ = \n";
    //     print_matrix(Q);

    //     std::cout << "\nR = \n";
    //     print_matrix(R_slice);
        
    //     // Allocate A_recon with correct size
    //     std::vector<double> a_recon(h_stride * width, 0);
    //     Matrix A_recon(&a_recon[0], h_stride, width);
    //     matmul(Q, R_slice, A_recon);
    //     std::cout << "\nA_recon = \n";
    //     print_matrix(A_recon);

    //     std::vector<double> qqt(width * width, 0 );
    //     std::vector<double> qt(width * width, 0 );
    //     Matrix QQT(&qqt[0], width, width);
    //     Matrix QT(&qt[0], width, width);
    //     QT = Q.T();
    //     matmul(QT, Q, QQT);
    //     std::cout << "\nQQT = \n";
    //     print_matrix(QQT);
    // }

    // print_matrix

    // int counter = num_processors;

    // while(counter != 1)
    // {
    //     // TODO: merge R1 -> R2
    //     // swap
    //     R1.swap(R2);
    // }

    // // B = identity, C = answer
    // std::vector<double> B = identity(2);
    // std::vector<double> C(A.size(), 0.f);

    // matmul(Matrix(A.data(), 8, 2), Matrix(B.data(), 2, 2), Matrix(C.data(), 8, 2));
    // print_matrix(Matrix(C.data(), 8, 2));


    // QR(Matrix(A[i*w*h], w, h), Matrix(R1[i*w*w], w, w));
    // Matrix(A[i*w*h], w, h) m;
    // m(1, 2) = 3;


    
    
    // Matrix t= partial_diag_multiply(A,A,A);
    // print_matrix(t);
    // exit(0);

    // Matrix Q, R;
    // tsqr(A, Q, R, 2);

    

    // Matrix QQT = matmul(transpose(Q), Q);
    // std::cout << "\nQQ.T = \n";
    // for(auto &row : QQT) {
    //     for(double x : row) std::cout << x << " ";
    //     std::cout << "\n";
    // }
}