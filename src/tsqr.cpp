#include <cstdio>

#include "tsqr.h"
#include "qr.h"

void tsqr(const Matrix &A, Matrix &Q, Matrix &R, int block_count) 
{
    int m = A.size();
    int n = A[0].size();
    // assert block_count = 2^n
    if (m % block_count != 0)
    {
        printf("error jaaa!\n");
        exit(1);
    }

    int block_height = m / block_count;

    std::vector<Matrix> Rs, Qs;
    for(int i = 0; i < m; i+=block_height)
    {
        // TODO: slice A[i:i+block_height]
        // TODO: do QR
        // TODO: push into Rs
        Matrix Q_partial, R_partial;
        qr(A, Q_partial, R_partial);   
        Rs.push_back(R_partial);

        Qs.push_back(Q_partial);
    }


    while(Rs.size() > 1)
    {
        std::vector<Matrix> new_Rs, new_Qs;

        for(int i = 0; i < Rs.size(); i += 2)
        {
            // TODO: concat Rs[i], Rs[i+1] >> M
            // TODO: calculate QR on M
            // TODO: push R in to new_Rs
            Matrix Q_partial, R_partial;
            Matrix M = Rs[i];

            M.insert(M.end(), Rs[i + 1].begin(), Rs[i + 1].end()); 

            qr(M, Q_partial, R_partial);
            new_Rs.push_back(R_partial);
            
            Matrix Q_new = partial_diag_multiply(Qs[i], Qs[i+1], Q_partial);
            
            new_Qs.push_back(Q_new);

            // Q_states[i].push_back(Q_partial);
        }
        // TODO: set Rs = new_Rs
        Rs = new_Rs;
        Qs = new_Qs;
    }

    R = Rs[0];
    Q = Qs[0];
    

}