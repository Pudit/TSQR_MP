#include <iostream>
#include <vector>
// #include "tsqr.h"
// #include "qr.h"
#include "tsqr_cuda.h"
// #include "qr_cuda.h"


int main()
{
    const int h = 8 * 4;
    const int w = 2;
    int block_height = 2;

    std::vector<double> A_host = {
        1,2,
        3,4,
        5,6,
        7,8,
        9,10,
        11,12,
        13,14,
        15,16,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        // 1,2,
        // 3,4,
        // 5,6,
        // 7,8,
        // 9,10,
        // 11,12,
        // 13,14,
        // 15,16,
        // 1,2,
        // 3,4,
        // 5,6,
        // 7,8,
        // 9,10,
        // 11,12,
        // 13,14,
        // 15,16,
        // 1,2,
        // 3,4,
        // 5,6,
        // 7,8,
        // 9,10,
        // 11,12,
        // 13,14,
        // 15,16,
    };

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            printf("%f, ", A_host[i * w + j]);
        }
        printf("\n");
    }
    printf("\n-------------\n");

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