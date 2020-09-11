/* Brian Ekstrom 
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
	int x, y, rows, cols, temp, i;
	//For 32x32 matrix	
	if (M==32)
	{
		for(rows = 0; rows < N; rows+=8)
		{ //Break the matrix down to 8x8 blocks to lower misses
			for(cols =0; cols < M; cols +=8)
			{
				for(x = rows; x < rows+8; x++)
				{
					for(y = cols; y < cols +8; y++)
					{
						if(x != y)
						{
							B[y][x] = A[x][y];
						}
						else
						{
							temp = A[x][y];
							i = x;
						}
					}
					if(cols == rows)
					{
						B[i][i] = temp;
					}
				}
			}
		}
	}
	//For 64x64 matrix
	else if (M==64)
	{
	for(rows = 0; rows < N; rows += 4)
	{ //Break the matrix down to 4x4 blocks to lower misses
		for(cols = 0; cols < M; cols += 4)
		{
			for(x = rows; x < rows + 4; x++)
			{
				for(y = cols; y < cols + 4; y++)
				{
					if(x != y)
					{
						B[y][x] = A[x][y];
					}
					else
					{
						temp = A[x][y];
						i = x;
					}
				}
				if(cols == rows)
				{
					B[i][i] = temp;
				}
			}
		}
	}
	}
	else
	{//For a matrix of any size other than 32x32 or 64x64
		for(rows = 0; rows < N; rows += 16)
		{ //Break the matrix down into 16x16 blocks to lower misses
			for(cols = 0; cols < M; cols += 16)
			{
				for(x = rows; x  < (rows + 16) && (x < N); x++)
				{
					for(y = cols; y < (cols + 16) && (y <M); y++)
					{
						if(x != y)
						{
							B[y][x] = A[x][y];
						}
						else
						{
							temp = A[x][y];
							i = x;
						}
					}
					if(cols == rows)
					{
						B[i][i] = temp;
					}
				}
			}
		}
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
    registerTransFunction(trans, trans_desc); 

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

