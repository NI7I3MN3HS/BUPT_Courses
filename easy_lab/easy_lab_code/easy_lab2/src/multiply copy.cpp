//#pragma GCC optimize(3,"Ofast","inline")
//#pragma GCC optimize(2)
#include "multiply.h"
#include <immintrin.h>
#include <thread>
#include <vector>

// TODO: you should implement your code in this file, we will only call `matrix_multiplication` to 
// test your implementation
#define A(i,j) matrix1[i][j]
#define B(i,j) matrix2[i][j]
#define C(i,j) result_matrix[i][j]

void matrix_multiply_thread(double matrix1[N][M], double matrix2[M][P], double result_matrix[N][P], int start_row, int end_row) {
   for(int i_o=start_row;i_o<end_row;i_o+=16){
    for (int i = i_o; i< i_o+16 && i < end_row; i++) {
        double temp[3200];
        for(int j=0;j<P;j++)
        {
            temp[j]=C(i,j);
        }
        for (int k = 0; k < M; k+=4) {
            double s= A(i,k);
            double s1 = A(i,k+1);
            double s2 = A(i,k+2);
            double s3 = A(i,k+3);
            for (int j = 0; j < P; j++) {
                temp[j] += s * B(k,j)+s1 * B(k+1,j)+s2 *B(k+2,j)+s3*B(k+3,j);
            }
        }
        for(int j=0;j<P;j++)
        {
            C(i,j)=temp[j];
        }
    }
}
}


void matrix_multiplication(double matrix1[N][M], double matrix2[M][P], double result_matrix[N][P])
{
    const int num_threads = 64;
    std::vector<std::thread> threads;

    int rows_per_thread = N / num_threads;

    for (int i = 0; i < num_threads; i++) {
        int start_row = i * rows_per_thread;
        int end_row = (i == num_threads - 1) ? N : (i + 1) * rows_per_thread;
        threads.emplace_back(std::thread(matrix_multiply_thread, matrix1, matrix2, result_matrix, start_row, end_row));
    }

    for (auto& th : threads) th.join();
}