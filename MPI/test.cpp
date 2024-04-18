#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <time.h>
#include <chrono>
#include <iostream>
#include <vector>


using namespace std;

//#include <sys/time.h>

#define N 2000
MPI_Status status;

int A[N][N];
int B[N][N];
int C[N][N];

int main(int argc, char** argv)
{

    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int processCount;
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);

    int slaveTaskCount = processCount - 1;
    int source;
    int rows;
    int offset;
    if (rank == 0) {


        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = rand() % 100;
                B[i][j] = rand() % 100;
            }
        }

#pragma region print


        //// Print Matrix A
        //printf("\nMatrix A\n\n");
        //for (int i = 0; i < N; i++) {
        //    for (int j = 0; j < N; j++) {
        //        printf("%d\t", A[i][j]);
        //    }
        //    printf("\n");
        //}

        //// Print Matrix B
        //printf("\nMatrix B\n\n");
        //for (int i = 0; i < N; i++) {
        //    for (int j = 0; j < N; j++) {
        //        printf("%d\t", B[i][j]);
        //    }
        //    printf("\n");
        //}
#pragma endregion

        rows = N / slaveTaskCount;
        int remainder = N % slaveTaskCount;
        offset = 0;
        auto start_parallel = chrono::high_resolution_clock::now();
        for (int dest = 1; dest <= slaveTaskCount; dest++)
        {
            int currentRows = rows;
            if (dest <= remainder) {
                currentRows++;
            }
            //cout << currentRows << endl;
            //send offset of matrix A
            MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            // send number of rows
            MPI_Send(&currentRows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            // send rows of the Matrix A
            MPI_Send(&A[offset][0], currentRows * N, MPI_INT, dest, 1, MPI_COMM_WORLD);
            // send matrix B
            MPI_Send(&B, N * N, MPI_INT, dest, 1, MPI_COMM_WORLD);

            // Offset is modified according to number of rows sent to each process
            offset = offset + currentRows;
        }
        // wait until each process return status with tag = 2
        for (int i = 1; i <= slaveTaskCount; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&C[offset][0], rows * N, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
        }
        auto end_parallel = chrono::high_resolution_clock::now();
        auto duration_parallel = chrono::duration_cast<chrono::milliseconds>(end_parallel - start_parallel);

        cout <<"time: "<< duration_parallel.count() << " " << "ms";


    #pragma region print result
    //    // Print the result matrix
    //    printf("\nResult Matrix C = Matrix A * Matrix B:\n\n");
    //    for (int i = 0; i < N; i++) {
    //        for (int j = 0; j < N; j++)
    //            printf("%d\t", C[i][j]);
    //        printf("\n");
    //    }
    //    printf("\n");
    }

#pragma endregion

    if (rank > 0) {

        source = 0;

        // slave process receives the offset 
        MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives number of row
        MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the part of the Matrix A 
        MPI_Recv(&A, rows * N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
        // The slave process receives the Matrix B
        MPI_Recv(&B, N * N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);


        //multiplication
        for (int k = 0; k < N; k++) {
            for (int i = 0; i < rows; i++) {
                C[i][k] = 0;
                // A(i, j)* B(j, k)
                for (int j = 0; j < N; j++)
                    C[i][k] = C[i][k] + A[i][j] * B[j][k];
            }
        }

        //send bag with tag 2

        MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&C, rows * N, MPI_INT, 0, 2, MPI_COMM_WORLD);

    }

    MPI_Finalize();
}