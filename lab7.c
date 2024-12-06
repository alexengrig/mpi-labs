#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PROCESS_ZERO 0
#define DEFAULT_MATRIX_SIZE 3

void printRowMatrix(int matrixSize, int *matrix) {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            printf("%d ", matrix[row * matrixSize + col]);
        }
        printf("\n");
    }
}

void printColMatrix(int matrixSize, int *matrix) {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            printf("%d ", matrix[col * matrixSize + row]);
        }
        printf("\n");
    }
}

void fillRndMatrix(int matrixSize, int *matrix) {
    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            matrix[row * matrixSize + col] = rand() % 100;
        }
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int myRank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    int matrixSize = argc > 1 ? atoi(argv[1]) : DEFAULT_MATRIX_SIZE;
    int benchmark = (argc > 2 && strcmp(argv[2], "benchmark") == 0) ? 1 : 0;
    int *matrixA = NULL;
    int *matrixB = NULL;
    int *matrixC = NULL;
    if (myRank == 0) {
        matrixA = (int *) malloc(matrixSize * matrixSize * sizeof(int));
        matrixB = (int *) malloc(matrixSize * matrixSize * sizeof(int));
        matrixC = (int *) malloc(matrixSize * matrixSize * sizeof(int));
        srand(time(NULL));
        fillRndMatrix(matrixSize, matrixA);
        fillRndMatrix(matrixSize, matrixB);
        if (benchmark != 1) {
            printf("Matrix A:\n");
            printRowMatrix(matrixSize, matrixA);
            printf("Matrix B:\n");
            printColMatrix(matrixSize, matrixB);
        }
    }
    double startTime = MPI_Wtime();
    if (numberOfProcesses < 2) {
        for (int row = 0; row < matrixSize; row++) {
            for (int col = 0; col < matrixSize; col++) {
                matrixC[row * matrixSize + col] = 0;
                for (int k = 0; k < matrixSize; k++) {
                    matrixC[row * matrixSize + col] +=
                            matrixA[row * matrixSize + k]
                            *
                            matrixB[col * matrixSize + k];
                }
            }
        }
    } else {
        int numberOfActiveProcesses = matrixSize < numberOfProcesses ? matrixSize : numberOfProcesses;
        int *sendcounts = calloc(numberOfProcesses, sizeof(int));
        int *displs = calloc(numberOfProcesses, sizeof(int));
        int *lineNums = calloc(numberOfActiveProcesses, sizeof(int));
        int linesPerProcess = matrixSize / numberOfActiveProcesses;
        int extraLines = matrixSize % numberOfActiveProcesses;
        int currLineNum = 0;
        for (int rank = 0; rank < numberOfActiveProcesses; rank++) {
            sendcounts[rank] = (rank < extraLines ? linesPerProcess + 1 : linesPerProcess) * matrixSize;
            displs[rank] = rank == 0 ? 0 : displs[rank - 1] + sendcounts[rank - 1];
            lineNums[rank] = currLineNum;
            currLineNum += sendcounts[rank] / matrixSize;
        }
        int *myRowA = NULL;
        int *myColB = NULL;
        int *myRowC = NULL;
        int recvCount = myRank < numberOfActiveProcesses ? sendcounts[myRank] : 0;
        int maxRecvCount = sendcounts[PROCESS_ZERO];
        if (recvCount > 0) {
            myRowA = (int *) malloc(recvCount * sizeof(int));
            myColB = (int *) malloc(maxRecvCount * sizeof(int));
            myRowC = (int *) malloc(recvCount * sizeof(int));
            for (int i = 0; i < recvCount; i++) {
                myRowC[i] = -1;
            }
        }
        if (myRank < numberOfActiveProcesses) {
            MPI_Scatterv(
                matrixA, sendcounts, displs, MPI_INT,
                myRowA, recvCount, MPI_INT,
                PROCESS_ZERO, MPI_COMM_WORLD
            );
            MPI_Scatterv(
                matrixB, sendcounts, displs, MPI_INT,
                myColB, maxRecvCount, MPI_INT,
                PROCESS_ZERO, MPI_COMM_WORLD
            );
            int currCol = lineNums[myRank];
            int currRank = myRank;
            for (int step = 0; step < numberOfActiveProcesses; step++) {
                // Multiplication
                for (int col = 0; col < sendcounts[currRank] / matrixSize; col++) {
                    for (int row = 0; row < recvCount / matrixSize; row++) {
                        myRowC[currCol + row * matrixSize + col] = 0;
                        for (int i = 0; i < matrixSize; i++) {
                            myRowC[currCol + row * matrixSize + col] +=
                                    myRowA[row * matrixSize + i]
                                    *
                                    myColB[col * matrixSize + i];
                        }
                    }
                }
                // Sharing
                int prev = (myRank - 1 + numberOfActiveProcesses) % numberOfActiveProcesses;
                int next = (myRank + 1) % numberOfActiveProcesses;
                MPI_Sendrecv_replace(
                    myColB, maxRecvCount, MPI_INT,
                    prev, 0,
                    next, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
                );
                currRank = (currRank + 1) % numberOfActiveProcesses;;
                currCol = lineNums[currRank];
            }
            MPI_Gatherv(
                myRowC, recvCount, MPI_INT,
                matrixC, sendcounts, displs, MPI_INT,
                PROCESS_ZERO, MPI_COMM_WORLD
            );
        }
        // Cleaning
        free(sendcounts);
        free(displs);
        free(lineNums);
        if (recvCount > 0) {
            free(myRowA);
            free(myColB);
            free(myRowC);
        }
    }
    // Printing
    if (myRank == 0) {
        if (benchmark != 1) {
            printf("Matrix C:\n");
            printRowMatrix(matrixSize, matrixC);
        }
        double endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;
        printf("Process %d: %f seconds\n",
               myRank, elapsedTime);
    }
    // Cleaning
    if (myRank == 0) {
        free(matrixA);
        free(matrixB);
        free(matrixC);
    }
    MPI_Finalize();
    return 0;
}
