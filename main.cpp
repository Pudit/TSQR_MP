#include <iostream>
#include <mpi.h>
#include "tsqr.h"
#include "qr.h"

#include <chrono>

#include <random>

inline std::vector<double> generate_random_matrix(int m, int n, int seed)
{
    std::mt19937 gen(seed); // Mersenne Twister engine
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::vector<double> A(m * n);

    for (int i = 0; i < m * n; ++i)
    {
        A[i] = dist(gen);
    }

    return A;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int height = 2;
    int width = 2;
    int random_seed = -1; // -1 means not provided

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        else if (arg == "-h" && i + 1 < argc)
        {
            height = std::stoi(argv[++i]);
        }
        else if (arg == "-w" && i + 1 < argc)
        {
            width = std::stoi(argv[++i]);
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

    int height_per_rank = height / world_size;
    std::vector<double> A(height_per_rank * width, 0), init_data;

    if (rank == 0)
    {
        std::cout << "Processors: " << world_size << "\n";
        std::cout << "Height (h): " << height << "\n";
        std::cout << "Width (w): " << width << "\n";
        // IDK if this is bugged or not :P
        init_data = generate_random_matrix(height, width, 1);
    }

    // Scatter data
    MPI_Scatter(
        init_data,
        height_per_rank * width,
        MPI_DOUBLE,
        A,
        height_per_rank * width,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    std::vector<double> Q(height_per_rank * w, 0),
        R1(2 * w * w, 0),
        R2(2 * w * w, 0); // allocate double the space to recv

    // TODO: reduce A -> R1

    for (int round = 0; (1 << round) < world_size; round++)
    {
        if (rank % (1 << round) != 0)
            continue;
        bool is_sending = rank % (1 << (round+1));
        if(is_sending)
        {
            // TODO: send data from start of R1 => rank - (1<<rank)
        }
        else
        {
            // TODO: recv data into the SECOND half of R1 <= rank + (1<<rank)
            // TODO: qr factorization from R1 => first half of R2
            // TODO: swap R1 and R2
        }
    }

    if(rank==0)
    {
        // TODO: print the solution
    }

    // std::vector<double> r(width * width, 0.f);
    // std::vector<double> q(height * width, 0.f);

    // Matrix A_full(A.data(), height, width);
    // Matrix R(r.data(), width, width);
    // Matrix Q(q.data(), height, width);

    // tsqr(A_full, Q, R, num_processors);
    // print_matrix(R);
}