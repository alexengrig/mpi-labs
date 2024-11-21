#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int myRank, numberOfProcesses;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    if (numberOfProcesses != 8 && numberOfProcesses != 12) {
        if (myRank == 0) {
            printf("Error: Number of processes must be 8 or 12.");
        }
        MPI_Finalize();
        return 1;
    }
    int nDims = 3;
    int *dims = malloc(nDims * sizeof(int));
    dims[0] = 2;
    dims[1] = 2;
    dims[2] = numberOfProcesses / 4;
    int *periods = malloc(nDims * sizeof(int));
    periods[0] = 0;
    periods[1] = 0;
    periods[2] = 0;
    // Cart
    double startTime = MPI_Wtime();
    MPI_Comm cartComm;
    MPI_Cart_create(
        MPI_COMM_WORLD,
        nDims,
        dims,
        periods,
        0,
        &cartComm
    );
    int coords[nDims];
    MPI_Cart_coords(
        cartComm,
        myRank,
        nDims,
        coords
    );
    int matrixNumber = coords[2];
    double myValue = myRank + 1;
    int *remainDims = malloc(nDims * sizeof(int));
    remainDims[0] = 1;
    remainDims[1] = 1;
    remainDims[2] = 0;
    MPI_Comm matrixComm;
    MPI_Cart_sub(
        cartComm,
        remainDims,
        &matrixComm
    );
    int subRank;
    MPI_Comm_rank(matrixComm, &subRank);
    // Reducing
    double matrixSum;
    MPI_Allreduce(
        &myValue,
        &matrixSum,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        matrixComm
    );
    // Printing
    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;
    printf(
        "Process %d (matrix %d): subRank=%d, value=%f, sum=%f, %f seconds\n",
        myRank, matrixNumber, subRank, myValue, matrixSum, elapsedTime
    );
    MPI_Comm_free(&matrixComm);
    MPI_Comm_free(&cartComm);
    MPI_Finalize();
    return 0;
}
