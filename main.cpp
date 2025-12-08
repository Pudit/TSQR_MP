#include <iostream>
#include <chrono>
#include <random>

#include "tsqr.h"
#include "qr.h"

inline std::vector<double> generate_random_matrix(int m, int n, int seed)
{
    std::mt19937 gen(seed); // Mersenne Twister engine
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::vector<double> A(m * n);

    for (int i = 0; i < m * n; ++i)
    {
        A[i] = dist(gen);
        // A[i] = 1;
    }

    return A;
}

int main(int argc, char *argv[]) {
    int h = 2;
    int w = 2;
    int random_seed = -1; // -1 means not provided

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "-h" && i + 1 < argc)
        {
            h = std::stoi(argv[++i]);
        }
        else if (arg == "-w" && i + 1 < argc)
        {
            w = std::stoi(argv[++i]);
            if (w % 2 != 0)
            {
                std::cerr << "Error: w (-w) must be an even number.\n";
                return 1;
            }
        }
        else if (arg == "-r" && i + 1 < argc)
        {
            random_seed = std::stoi(argv[++i]);
        }
        else
        {
            std::cerr << "Unknown or incomplete argument: " << arg << std::endl;
            return 1;
        }
    }

    std::cout << "Height (h): " << h << "\n";
    std::cout << "Width (w): " << w << "\n";

    // std::vector<double> A = {
    //     1, 2,
    //     3, 4,
    //     5, 6,
    //     7, 8,
    //     9, 10,
    //     11, 12,
    //     13, 14,
    //     15, 16,
    //     // 2352, 235,
    //     // -12, 12,
    //     // -1, -2,
    //     // -3, -4,
    // };
    
    std::vector<double> A = generate_random_matrix(h, w, 1);
    std::vector<double> r(w * w, 0.f);
    std::vector<double> q(h * w, 0.f);

    Matrix A_full(A.data(), h, w);
    Matrix R(r.data(), w, w);
    Matrix Q(q.data(), h, w);

    auto start = std::chrono::high_resolution_clock::now();
        
    qr(A_full, Q, R);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    print_matrix(R);
    std::cout << "TSQR elapsed time: " << elapsed.count() << " us\n";
}
