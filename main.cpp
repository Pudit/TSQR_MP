#include <iostream>
#include <vector>
// #include "tsqr.h"
// #include "qr.h"
#include "tsqr_cuda.h"
// #include "qr_cuda.h"
#include <random>

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

auto isPowerOf2 = [](int x)
{
    return x > 0 && (x & (x - 1)) == 0;
};

int main(int argc, char *argv[])
{
    int block_height = -1;
    int h = 2;
    int w = 2;
    int random_seed = -1; // -1 means not provided

    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    size_t max_shmem = prop.sharedMemPerBlock;
    printf("Max shared memory = %zu\n", max_shmem);

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "-b" && i + 1 < argc)
        {
            block_height = std::stoi(argv[++i]);
            if (!isPowerOf2(block_height))
            {
                std::cerr << "Error: block_height (-b) must be a power of 2.\n";
                return 1;
            }
        }
        else if (arg == "-h" && i + 1 < argc)
        {
            h = std::stoi(argv[++i]);
            if (!isPowerOf2(h))
            {
                std::cerr << "Error: h (-h) must be a power of 2.\n";
                return 1;
            }
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

    if (block_height == -1)
        block_height = std::max(4, h / 2048);

    // block_height = std::max(w, block_height);

    std::cout << "Block Height: " << block_height << "\n";
    std::cout << "Height (h): " << h << "\n";
    std::cout << "Width (w): " << w << "\n";

    // const int h = 8 * 4;
    // const int w = 2;
    // int block_height = 4;

    // std::vector<double> A_host = {
    //     1,2,
    //     3,4,
    //     5,6,
    //     7,8,
    //     9,10,
    //     11,12,
    //     13,14,
    //     15,16,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     0, 0,
    //     // 0, 0,
    //     // 0, 0,
    //     // 0, 0,
    //     // 0, 0,
    //     // 1,2,
    //     // 3,4,
    //     // 5,6,
    //     // 7,8,
    //     // 9,10,
    //     // 11,12,
    //     // 13,14,
    //     // 15,16,
    //     // 1,2,
    //     // 3,4,
    //     // 5,6,
    //     // 7,8,
    //     // 9,10,
    //     // 11,12,
    //     // 13,14,
    //     // 15,16,
    //     // 1,2,
    //     // 3,4,
    //     // 5,6,
    //     // 7,8,
    //     // 9,10,
    //     // 11,12,
    //     // 13,14,
    //     // 15,16,
    // };

    std::vector<double> A_host = generate_random_matrix(h, w, 1);

    // print A host
    // for (int i = 0; i < h; i++)
    // {
    //     for (int j = 0; j < w; j++)
    //     {
    //         printf("%f, ", A_host[i * w + j]);
    //     }
    //     printf("\n");
    // }
    // printf("\n-------------\n");

    // test_qr(A_host, h, w);
    test_tsqr(A_host, h, w, block_height);

    // Print Q
    // std::cout << "Q:\n";
    // for (int i = 0; i < h; i++)
    // {
    //     for (int j = 0; j < w; j++)
    //         std::cout << out[i*w + j] << " ";
    //     std::cout << "\n";
    // }

    return 0;
}