#include <cstdio>
#include <cassert>
#include <cstring>

#include "tsqr.h"
#include "qr.h"

#include "omp.h"

Matrix slice_matrix(const Matrix &A, int start_row, int num_rows)
{
    double *slice_ptr = A.ptr + start_row * A.w;
    return Matrix(slice_ptr, num_rows, A.w);
}

void tsqr(std::vector<double> init_data, int world_size, int height, int width) 
{
    int height_per_rank = height / world_size;
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

    // std::vector<double> Q(height_per_rank * w, 0),
    //     R1(2 * w * w, 0),
    //     R2(2 * w * w, 0); // allocate double the space to recv

    std::vector<double> Q(max(2*width, height_per_rank) * width, 0);
    std::vector<double> R1(2 * width * width, 0);
    std::vector<double> R2(2 * width * width, 0);

    Matrix A_local(A.data(), height_per_rank, width);
    Matrix Q_local(Q.data(), height_per_rank, width);
    Matrix R_local(R1.data(), width, width);
    
    qr(A_local, Q_local, R_local);

    for (int round = 0; (1 << round) < world_size; round++)
    {
        if (rank % (1 << round) != 0)
            continue;
        bool is_sending = rank % (1 << (round+1));
        
        if(is_sending)
        {
            // TODO: send data from start of R1 => rank - (1<<rank)
            MPI_Send(R1.data(), width * width, MPI_DOUBLE, rank - (1 << round), 0, MPI_COMM_WORLD);
        }
        else
        {
            // TODO: recv data into the SECOND half of R1 <= rank + (1<<rank)
            // TODO: qr factorization from R1 => first half of R2
            // TODO: swap R1 and R2
            MPI_Recv(R1.data() + width * width, width * width, MPI_DOUBLE, rank + (1<<rank), 
                                            0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            Matrix R_stacked(R1.data(), 2 * width, width);
            Matrix Q_tmp(Q.data(), 2 * width, width); 
            Matrix R_result(R2.data(), width, width);
            
            qr(R_stacked, Q_tmp, R_result);
            
            R1.swap(R2);
        }
    }

    if (rank == 0)
    {
        std::cout << "R = \n";
        Matrix R_final(R1.data(), width, width);
        print_matrix(R_final);
    }

    MPI_Finalize();

}

void tsqr(const Matrix &A, Matrix &Q, Matrix &R, int num_processors)
{
    int m = A.h;
    int n = A.w;

    assert((num_processors & (num_processors - 1)) == 0 && "num_processors must be power of 2");
    

    if (m % num_processors != 0)
    {
        printf("Error: m=%zu is not divisible by num_processors=%d\n", m, num_processors);
        exit(1);
    }

    int block_height = m / num_processors;

    // 2 inw buffers for pingpong
    // store R
    std::vector<double> R_buffer1(num_processors * n * n);
    std::vector<double> R_buffer2(num_processors * n * n);

    // store Q
    // TODO:
    std::vector<double> q_tmp(block_height * n);
    Matrix Q_tmp(q_tmp.data(), block_height, n);
    int counter = num_processors;

#pragma omp parallel num_threads(num_processors)
    {

#pragma omp for
        for (int i = 0; i < num_processors; i++)
        {
            size_t start_row = i * block_height;

            Matrix A_block = slice_matrix(A, start_row, block_height);

            Matrix R_partial(R_buffer1.data() + i * n * n, n, n);

            qr(A_block, Q_tmp, R_partial);
            // print_matrix(R_partial);
        }
        // printf("len\n");
        // printf("bf p\n");

#pragma omp barrier
        // #pragma omp single
        //         {
        //             printf("R_partial %d:\n", omp_get_thread_num());
        //             print_matrix(Matrix(R_buffer1.data(), num_processors * n, n));
        //             printf("-----\n");
        //         }

        while (counter != 1)
        {
            #ifdef DEBUG
            printf("parallel count %d, counter = %d\n", omp_get_thread_num(), counter);
            #endif

#pragma omp for
            for (int i = 0; i < counter / 2; i++)
            {
                // printf("i = %d, thread = %d\n", i, omp_get_thread_num());
                Matrix M(R_buffer1.data() + 2 * i * n * n, 2 * n, n);

                Matrix R_partial(R_buffer2.data() + i * n * n, n, n);

                qr(M, Q_tmp, R_partial);
                // printf("done qr %d\n", omp_get_thread_num());
            }

// printf("barrier, counter = %d, omp get thread id = %d\n", counter, omp_get_thread_num());
#pragma omp barrier

#pragma omp single
            {
                R_buffer1.swap(R_buffer2);
                counter /= 2;
            }
        }


#pragma omp single
        {
            std::memcpy(R.ptr, R_buffer1.data(), n * n * sizeof(double));
        }
    }
}

// void tsqr(const Matrix &A, Matrix &Q, Matrix &R, int block_count)
// {
//     int m = A.size();
//     int n = A[0].size();
//     // assert block_count = 2^n
//     if (m % block_count != 0)
//     {
//         printf("error jaaa!\n");
//         exit(1);
//     }

//     int block_height = m / block_count;

//     std::vector<Matrix> Rs, Qs;
//     for(int i = 0; i < m; i+=block_height)
//     {
//         // TODO: slice A[i:i+block_height]
//         // TODO: do QR
//         // TODO: push into Rs
//         Matrix Q_partial, R_partial;
//         qr(A, Q_partial, R_partial);
//         Rs.push_back(R_partial);

//         Qs.push_back(Q_partial);
//     }

//     while(Rs.size() > 1)
//     {
//         std::vector<Matrix> new_Rs, new_Qs;

//         for(int i = 0; i < Rs.size(); i += 2)
//         {
//             // TODO: concat Rs[i], Rs[i+1] >> M
//             // TODO: calculate QR on M
//             // TODO: push R in to new_Rs
//             Matrix Q_partial, R_partial;
//             Matrix M = Rs[i];

//             M.insert(M.end(), Rs[i + 1].begin(), Rs[i + 1].end());

//             qr(M, Q_partial, R_partial);
//             new_Rs.push_back(R_partial);

//             Matrix Q_new = partial_diag_multiply(Qs[i], Qs[i+1], Q_partial);

//             new_Qs.push_back(Q_new);

//             // Q_states[i].push_back(Q_partial);
//         }
//         // TODO: set Rs = new_Rs
//         Rs = new_Rs;
//         Qs = new_Qs;
//     }

//     R = Rs[0];
//     Q = Qs[0];

// }