#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include "mpi.h"
#define N 4

void print_results(const char* prompt, int a[N][N]);

int main(int argc, char* argv[])
{
    int i, j, k, rank, size, sum = 0;
    int A[N][N] = { {1,2,3,4},{5,6,7,8},{9,1,2,3},{4,5,6,7,} };
    int B[N][N] = { {1,2,3,4},{5,6,7,8},{9,1,2,3},{4,5,6,7,} };
    int C[N][N];
    int a[N], c[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //scatter rows of first matrix to different processes     
    MPI_Scatter(A, N * N / size, MPI_INT, a, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);

    //broadcast second matrix to all processes
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    //perform vector multiplication by all processes
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            sum = sum + a[j] * B[j][i];              
        }
        c[i] = sum;
        sum = 0;
    }

    MPI_Gather(c, N * N / size, MPI_INT, C, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    if (rank == 0)                      
        print_results("C = ", C);
}

void print_results(const char* prompt, int a[N][N])
{
    int i, j;
    printf("%s\n", prompt);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf(" %d", a[i][j]);
        }
        printf("\n");
    }
}