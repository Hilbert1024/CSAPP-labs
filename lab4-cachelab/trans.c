/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

// misses:343
char transpose_block_32_32_desc[] = "Transpose block 32,32";
void transpose_block_32_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    int row = 8;
    int col = 8;
    for (i = 0; i < N; i += row) {
        for (j = 0; j < M; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                for (int j1 = 0; j1 < col; j1++) {
                    tmp = A[i + i1][j + j1];
                    B[j + j1][i + i1] = tmp;
                }
            }
        }
    }
}

// misses:287
char transpose_block_opt_32_32_desc[] = "Transpose block optimize 32,32";
void transpose_block_opt_32_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, c0, c1, c2, c3, c4, c5, c6, c7;
    int row = 8;
    int col = 8;
    for (i = 0; i < N; i += row) {
        for (j = 0; j < M; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                c0 = A[i + i1][j + 0];
                c1 = A[i + i1][j + 1];
                c2 = A[i + i1][j + 2];
                c3 = A[i + i1][j + 3];
                c4 = A[i + i1][j + 4];
                c5 = A[i + i1][j + 5];
                c6 = A[i + i1][j + 6];
                c7 = A[i + i1][j + 7];
                B[j + 0][i + i1] = c0;
                B[j + 1][i + i1] = c1;
                B[j + 2][i + i1] = c2;
                B[j + 3][i + i1] = c3;
                B[j + 4][i + i1] = c4;
                B[j + 5][i + i1] = c5;
                B[j + 6][i + i1] = c6;
                B[j + 7][i + i1] = c7;
            }
        }
    }
}

// misses:1699
char transpose_block_opt_64_64_desc[] = "Transpose block optimize 64,64";
void transpose_block_opt_64_64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, c0, c1, c2, c3;
    int row = 4;
    int col = 4;
    for (i = 0; i < N; i += row) {
        for (j = 0; j < M; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                c0 = A[i + i1][j + 0];
                c1 = A[i + i1][j + 1];
                c2 = A[i + i1][j + 2];
                c3 = A[i + i1][j + 3];
                B[j + 0][i + i1] = c0;
                B[j + 1][i + i1] = c1;
                B[j + 2][i + i1] = c2;
                B[j + 3][i + i1] = c3;
            }
        }
    }
}

// misses:1699
char transpose_block_opt1_64_64_desc[] = "Transpose block optimize1 64,64";
void transpose_block_opt1_64_64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, c0, c1, c2, c3;
    int row = 4;
    int col = 4;
    for (i = 0; i < N; i += row) {
        for (j = 0; j < M; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                c0 = A[i + i1][j + 0];
                c1 = A[i + i1][j + 1];
                c2 = A[i + i1][j + 2];
                c3 = A[i + i1][j + 3];
                B[j + 0][i + i1] = c0;
                B[j + 1][i + i1] = c1;
                B[j + 2][i + i1] = c2;
                B[j + 3][i + i1] = c3;
            }
        }
    }
}

// misses:1900
char transpose_block_opt_61_67_desc[] = "Transpose block optimize1 61,67";
void transpose_block_opt_61_67(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11;
    int row = 12;
    int col = 12;
    for (i = 0; i <= N - row; i += row) {
        for (j = 0; j <= M - col; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                c0 = A[i + i1][j + 0];
                c1 = A[i + i1][j + 1];
                c2 = A[i + i1][j + 2];
                c3 = A[i + i1][j + 3];
                c4 = A[i + i1][j + 4];
                c5 = A[i + i1][j + 5];
                c6 = A[i + i1][j + 6];
                c7 = A[i + i1][j + 7];
                c8 = A[i + i1][j + 8];
                c9 = A[i + i1][j + 9];
                c10 = A[i + i1][j + 10];
                c11 = A[i + i1][j + 11];
                B[j + 0][i + i1] = c0;
                B[j + 1][i + i1] = c1;
                B[j + 2][i + i1] = c2;
                B[j + 3][i + i1] = c3;
                B[j + 4][i + i1] = c4;
                B[j + 5][i + i1] = c5;
                B[j + 6][i + i1] = c6;
                B[j + 7][i + i1] = c7;
                B[j + 8][i + i1] = c8;
                B[j + 9][i + i1] = c9;
                B[j + 10][i + i1] = c10;
                B[j + 11][i + i1] = c11;
            }
        }
    }

    // N in [60,60], M in [60, 66]
    for (int j = 0; j <= M - col; j+= col) {
        c0 = A[60][j + 0];
        c1 = A[60][j + 1];
        c2 = A[60][j + 2];
        c3 = A[60][j + 3];
        c4 = A[60][j + 4];
        c5 = A[60][j + 5];
        c6 = A[60][j + 6];
        c7 = A[60][j + 7];
        c8 = A[60][j + 8];
        c9 = A[60][j + 9];
        c10 = A[60][j + 10];
        c11 = A[60][j + 11];
        B[j + 0][60] = c0;
        B[j + 1][60] = c1;
        B[j + 2][60] = c2;
        B[j + 3][60] = c3;
        B[j + 4][60] = c4;
        B[j + 5][60] = c5;
        B[j + 6][60] = c6;
        B[j + 7][60] = c7;
        B[j + 8][60] = c8;
        B[j + 9][60] = c9;
        B[j + 10][60] = c10;
        B[j + 11][60] = c11;
    }
    
    row = 7;
    j = 60;
    for (int i = 0; i <= N - row; i += row) {
        for (int i1 = 0; i1 < row; i1++) {
            c0 = A[i + i1][j + 0];
            c1 = A[i + i1][j + 1];
            c2 = A[i + i1][j + 2];
            c3 = A[i + i1][j + 3];
            c4 = A[i + i1][j + 4];
            c5 = A[i + i1][j + 5];
            c6 = A[i + i1][j + 6];
            B[j + 0][i + i1] = c0;
            B[j + 1][i + i1] = c1;
            B[j + 2][i + i1] = c2;
            B[j + 3][i + i1] = c3;
            B[j + 4][i + i1] = c4;
            B[j + 5][i + i1] = c5;
            B[j + 6][i + i1] = c6;
        }
    }

    // fill
    for (int i = 56; i < N; i++) {
        for (int j = 60; j < M; j++) {
            c0 = A[i][j];
            B[j][i] = c0;
        }
    }
}

// misses:2075
char transpose_block_opt1_61_67_desc[] = "Transpose block optimize1 61,67";
void transpose_block_opt1_61_67(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, c0, c1, c2, c3, c4, c5, c6, c7;
    int row = 8;
    int col = 8;
    for (i = 0; i <= N - row; i += row) {
        for (j = 0; j <= M - col; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                c0 = A[i + i1][j + 0];
                c1 = A[i + i1][j + 1];
                c2 = A[i + i1][j + 2];
                c3 = A[i + i1][j + 3];
                c4 = A[i + i1][j + 4];
                c5 = A[i + i1][j + 5];
                c6 = A[i + i1][j + 6];
                c7 = A[i + i1][j + 7];
                B[j + 0][i + i1] = c0;
                B[j + 1][i + i1] = c1;
                B[j + 2][i + i1] = c2;
                B[j + 3][i + i1] = c3;
                B[j + 4][i + i1] = c4;
                B[j + 5][i + i1] = c5;
                B[j + 6][i + i1] = c6;
                B[j + 7][i + i1] = c7;
            }
        }
    }

    // N in [56,60], M in [64, 66]
    i = 56;
    for (int j = 0; j <= M - col; j+= col) {
        for (int i1 = 0; i1 < 5; i1++) {
            c0 = A[i + i1][j + 0];
            c1 = A[i + i1][j + 1];
            c2 = A[i + i1][j + 2];
            c3 = A[i + i1][j + 3];
            c4 = A[i + i1][j + 4];
            c5 = A[i + i1][j + 5];
            c6 = A[i + i1][j + 6];
            c7 = A[i + i1][j + 7];
            B[j + 0][i + i1] = c0;
            B[j + 1][i + i1] = c1;
            B[j + 2][i + i1] = c2;
            B[j + 3][i + i1] = c3;
            B[j + 4][i + i1] = c4;
            B[j + 5][i + i1] = c5;
            B[j + 6][i + i1] = c6;
            B[j + 7][i + i1] = c7;
        }
    }
    
    row = 3; // 67 - 8*8
    j = 64;
    for (int i = 0; i <= N - row; i+= row) {
        for (int i1 = 0; i1 < row; i1++) {
            c0 = A[i + i1][j + 0];
            c1 = A[i + i1][j + 1];
            c2 = A[i + i1][j + 2];
            B[j + 0][i + i1] = c0;
            B[j + 1][i + i1] = c1;
            B[j + 2][i + i1] = c2;
        }
    }

    // fill
    c0 = A[60][64];
    c1 = A[60][65];
    c2 = A[60][66];
    B[64][60] = c0;
    B[65][60] = c1;
    B[66][60] = c2;
}

// misses:4624
char transpose_block_opt2_61_67_desc[] = "Transpose block optimize2 61,67";
void transpose_block_opt2_61_67(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, c0, c1, c2, c3, c4, c5, c6, c7;
    for (i = 0; i < N; i++) {
        for (j = 0; j <= M - 8; j += 8) {
            c0 = A[i][j + 0];
            c1 = A[i][j + 1];
            c2 = A[i][j + 2];
            c3 = A[i][j + 3];
            c4 = A[i][j + 4];
            c5 = A[i][j + 5];
            c6 = A[i][j + 6];
            c7 = A[i][j + 7];
            B[j + 0][i] = c0;
            B[j + 1][i] = c1;
            B[j + 2][i] = c2;
            B[j + 3][i] = c3;
            B[j + 4][i] = c4;
            B[j + 5][i] = c5;
            B[j + 6][i] = c6;
            B[j + 7][i] = c7;
        }
    }

    j = 64;
    for (int i = 0; i < N; i++) {
        c0 = A[i][j + 0];
        c1 = A[i][j + 1];
        c2 = A[i][j + 2];
        B[j + 0][i] = c0;
        B[j + 1][i] = c1;
        B[j + 2][i] = c2;
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    switch (N) {
    case 32:
        transpose_block_opt_32_32(M,N,A,B);
        break;
    case 64:
        transpose_block_opt_64_64(M,N,A,B);
        break;
    case 61:
        transpose_block_opt_61_67(M,N,A,B);
        break;
    default:
        break;
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc); 
    // registerTransFunction(transpose_block_32_32, transpose_block_32_32_desc);
    // registerTransFunction(transpose_block_opt_32_32, transpose_block_opt_32_32_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

