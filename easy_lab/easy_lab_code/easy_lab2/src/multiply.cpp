#include "multiply.h"
#include <immintrin.h>
#include <thread>
#include <vector>

// TODO: you should implement your code in this file, we will only call `matrix_multiplication` to
// test your implementation
#define A(i, j) matrix1[i][j]
#define B(i, j) matrix2[i][j]
#define C(i, j) result_matrix[i][j]
#define min(a, b) ((a) < (b) ? (a) : (b))

void matrix_multiply_thread(double matrix1[N][M], double matrix2[M][P], double result_matrix[N][P], int start_row, int end_row)
{
    if (P % 8 == 0)
    {
        for (int i_o = start_row; i_o < end_row; i_o += 16)
        {
            for (int i = i_o; i < i_o + 16 && i < end_row; i++)
            {
                double temp[P] = {0}; // 用于存储临时结果的数组

                for (int k = 0; k < M; k++)
                {
                    __m512d vecA = _mm512_set1_pd(A(i, k)); // 用于存储A矩阵中的一行

                    for (int j = 0; j < P; j += 8)
                    {
                        __m512d vecC = _mm512_loadu_pd(&temp[j]);
                        __m512d vecB = _mm512_loadu_pd(&B(k, j));

                        vecC = _mm512_fmadd_pd(vecA, vecB, vecC);

                        _mm512_storeu_pd(&temp[j], vecC);
                    }
                }
                for (int j = 0; j < P; j++)
                {
                    C(i, j) = temp[j];
                }
            }
        }
    }
    else
    {
        for (int i = start_row; i < end_row; i++)
        {
            for (int j = 0; j < M; j++)
            {
                for (int k = 0; k < P; k++)
                {
                    result_matrix[i][k] += matrix1[i][j] * matrix2[j][k];
                }
            }
        }
    }
}

void matrix_multiplication(double matrix1[N][M], double matrix2[M][P], double result_matrix[N][P])
{
    const int num_threads = 64;
    std::vector<std::thread> threads;

    int rows_per_thread = N / num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        int start_row = i * rows_per_thread;
        int end_row = (i == num_threads - 1) ? N : (i + 1) * rows_per_thread;
        threads.emplace_back(std::thread(matrix_multiply_thread, matrix1, matrix2, result_matrix, start_row, end_row));
    }

    for (auto &th : threads)
        th.join();
}