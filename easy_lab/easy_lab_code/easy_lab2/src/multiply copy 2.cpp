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
#define min(a,b) ((a)<(b)?(a):(b))

void matrix_multiply_thread(double matrix1[N][M], double matrix2[M][P], double result_matrix[N][P], int start_row, int end_row) {
    for (int i_o = start_row; i_o < end_row; i_o+=16) 
    {
        for (int i = i_o; i< i_o+16 && i < end_row; i++){
        double temp[P] = {0};  // 用于存储临时结果的数组

        for (int k = 0; k < M; k++) {
            __m512d vecA = _mm512_set1_pd(A(i, k));  // 用于存储A矩阵中的一行

            for (int j = 0; j < P; j+=8) {
                __m512d vecC = _mm512_loadu_pd(&temp[j]);
                __m512d vecB = _mm512_loadu_pd(&B(k,j));

                vecC = _mm512_fmadd_pd(vecA, vecB, vecC);

                _mm512_storeu_pd(&temp[j], vecC);
            }
        }
        for (int j = 0; j < P; j++) {
            C(i, j) = temp[j];
        }
    }
    }
}


void matrix_multiplication(double matrix1[N][M], double matrix2[M][P], double result_matrix[N][P])
{
    //矩阵大小为4，4，4时，执行基础矩阵乘法
    if (N == 4 && M == 4 && P == 4) {
        for (int i = 0; i < N; i++) {

            for (int j = 0; j < P; j++) {
                double temp = 0;
                for(int k=0;k<M;k++)
                {
                    temp+=A(i,k)*B(k,j);
                }
                C(i,j)=temp;
            }
        }
        return;
    }
    const int num_threads = 64;
    std::vector<std::thread> threads;

    int rows_per_thread = N / num_threads;


    for (int i = 0; i < num_threads; i++) {
        int start_row = i * rows_per_thread;
        int end_row = (i == num_threads - 1) ? N : (i + 1) * rows_per_thread;
        threads.emplace_back(std::thread(matrix_multiply_thread, matrix1, matrix2, result_matrix, start_row, end_row));
    }



    for (auto& th : threads) th.join();

    /*
    __m256d c, a, b;
    int iTile = 64, jTile = 64, kTile = 32;
    int iOuterBound = N / iTile, jOuterBound = P / jTile, kOuterBound = M / kTile;
    
    int i,j,k,ii,jj,kk;
    for(i=0;i<iOuterBound;i++)
        for(j=0;j<jOuterBound;j++)
            for(k=0;k<kOuterBound;k++)
                for(ii=0;ii<iTile;ii++)
                    for(kk=0;kk<kTile;kk++)
                    {
                        for(jj=0;jj<jTile;jj++)
                        {
                            C(i*iTile+ii,j*jTile+jj) += A(i*iTile+ii,k*kTile+kk) * B(k*kTile+kk,j*jTile+jj);
                        }

                    }
*/


    /*
    for (int i = 0; i < N; i+=8) {
        for (int j = 0; j < P; j += 8) {
            __m256d C0v, C1v, C2v, C3v, C4v, C5v, C6v, C7v, B0v, B1v, B2v, B3v, B4v, B5v, B6v, B7v;
            for (int k = 0; k < M; k += 8) {
                C0v = _mm256_loadu_pd(&C(i,j));
                C1v = _mm256_loadu_pd(&C(i+1,j));
                C2v = _mm256_loadu_pd(&C(i+2,j));
                C3v = _mm256_loadu_pd(&C(i+3,j));
                C4v = _mm256_loadu_pd(&C(i+4,j));
                C5v = _mm256_loadu_pd(&C(i+5,j));
                C6v = _mm256_loadu_pd(&C(i+6,j));
                C7v = _mm256_loadu_pd(&C(i+7,j));

                B0v = _mm256_loadu_pd(&B(k,j));

                C0v += A(i,k) * B0v;
                C1v += A(i+1,k) * B0v;
                C2v += A(i+2,k) * B0v;
                C3v += A(i+3,k) * B0v;
                C4v += A(i+4,k) * B0v;
                C5v += A(i+5,k) * B0v;
                C6v += A(i+6,k) * B0v;
                C7v += A(i+7,k) * B0v;

                B1v = _mm256_loadu_pd(&B(k+1,j));

                C0v += A(i,k+1) * B1v;
                C1v += A(i+1,k+1) * B1v;
                C2v += A(i+2,k+1) * B1v;
                C3v += A(i+3,k+1) * B1v;
                C4v += A(i+4,k+1) * B1v;
                C5v += A(i+5,k+1) * B1v;
                C6v += A(i+6,k+1) * B1v;
                C7v += A(i+7,k+1) * B1v;

                B2v = _mm256_loadu_pd(&B(k+2,j));

                C0v += A(i,k+2) * B2v;
                C1v += A(i+1,k+2) * B2v;
                C2v += A(i+2,k+2) * B2v;
                C3v += A(i+3,k+2) * B2v;
                C4v += A(i+4,k+2) * B2v;
                C5v += A(i+5,k+2) * B2v;
                C6v += A(i+6,k+2) * B2v;
                C7v += A(i+7,k+2) * B2v;

                B3v = _mm256_loadu_pd(&B(k+3,j));

                C0v += A(i,k+3) * B3v;
                C1v += A(i+1,k+3) * B3v;
                C2v += A(i+2,k+3) * B3v;
                C3v += A(i+3,k+3) * B3v;
                C4v += A(i+4,k+3) * B3v;
                C5v += A(i+5,k+3) * B3v;
                C6v += A(i+6,k+3) * B3v;
                C7v += A(i+7,k+3) * B3v;

                B4v = _mm256_loadu_pd(&B(k+4,j));

                C0v += A(i,k+4) * B4v;
                C1v += A(i+1,k+4) * B4v;
                C2v += A(i+2,k+4) * B4v;
                C3v += A(i+3,k+4) * B4v;
                C4v += A(i+4,k+4) * B4v;
                C5v += A(i+5,k+4) * B4v;
                C6v += A(i+6,k+4) * B4v;
                C7v += A(i+7,k+4) * B4v;

                B5v = _mm256_loadu_pd(&B(k+5,j));

                C0v += A(i,k+5) * B5v;
                C1v += A(i+1,k+5) * B5v;
                C2v += A(i+2,k+5) * B5v;
                C3v += A(i+3,k+5) * B5v;
                C4v += A(i+4,k+5) * B5v;
                C5v += A(i+5,k+5) * B5v;
                C6v += A(i+6,k+5) * B5v;
                C7v += A(i+7,k+5) * B5v;

                B6v = _mm256_loadu_pd(&B(k+6,j));

                C0v += A(i,k+6) * B6v;
                C1v += A(i+1,k+6) * B6v;
                C2v += A(i+2,k+6) * B6v;
                C3v += A(i+3,k+6) * B6v;
                C4v += A(i+4,k+6) * B6v;
                C5v += A(i+5,k+6) * B6v;
                C6v += A(i+6,k+6) * B6v;
                C7v += A(i+7,k+6) * B6v;

                B7v = _mm256_loadu_pd(&B(k+7,j));

                C0v += A(i,k+7) * B7v;
                C1v += A(i+1,k+7) * B7v;
                C2v += A(i+2,k+7) * B7v;
                C3v += A(i+3,k+7) * B7v;
                C4v += A(i+4,k+7) * B7v;
                C5v += A(i+5,k+7) * B7v;
                C6v += A(i+6,k+7) * B7v;
                C7v += A(i+7,k+7) * B7v;

                _mm256_storeu_pd(&C(i,j), C0v);
                _mm256_storeu_pd(&C(i + 1,j), C1v);
                _mm256_storeu_pd(&C(i + 2,j), C2v);
                _mm256_storeu_pd(&C(i + 3,j), C3v);
                _mm256_storeu_pd(&C(i + 4,j), C4v);
                _mm256_storeu_pd(&C(i + 5,j), C5v);
                _mm256_storeu_pd(&C(i + 6,j), C6v);
                _mm256_storeu_pd(&C(i + 7,j), C7v);
            }*/
            
            /*
            C(i,j) = 0;
            C(i,j+1) = 0;
            C(i,j+2) = 0;
            C(i,j+3) = 0;
            for (int k = 0; k < M; k++) {
                C(i,j) += A(i,k) * B(k,j);
                C(i,j+1) += A(i,k) * B(k,j+1);
                C(i,j+2) += A(i,k) * B(k,j+2);
                C(i,j+3) += A(i,k) * B(k,j+3);

                C(i,j) += A(i,k) * B(k,j);
                C(i,j+1) += A(i,k) * B(k,j+1);
                C(i,j+2) += A(i,k) * B(k,j+2);
                C(i,j+3) += A(i,k) * B(k,j+3);

                C(i+1,j) += A(i+1,k) * B(k,j);
                C(i+1,j+1) += A(i+1,k) * B(k,j+1);
                C(i+1,j+2) += A(i+1,k) * B(k,j+2);
                C(i+1,j+3) += A(i+1,k) * B(k,j+3);

                C(i+2,j) += A(i+2,k) * B(k,j);
                C(i+2,j+1) += A(i+2,k) * B(k,j+1);
                C(i+2,j+2) += A(i+2,k) * B(k,j+2);
                C(i+2,j+3) += A(i+2,k) * B(k,j+3);

                C(i+3,j) += A(i+3,k) * B(k,j);
                C(i+3,j+1) += A(i+3,k) * B(k,j+1);
                C(i+3,j+2) += A(i+3,k) * B(k,j+2);
                C(i+3,j+3) += A(i+3,k) * B(k,j+3);
            }
            */
            
}

