#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ZERO_RANK 0

void processZero(int size) {
    // Reducing
    int *minValues = malloc(size * sizeof(int));
    for (int i = 0; i < size; i++) {
        minValues[i] = 100;
    }
    MPI_Reduce(
        MPI_IN_PLACE,
        minValues,
        size,
        MPI_INT,
        MPI_MIN,
        ZERO_RANK,
        MPI_COMM_WORLD
    );
    // Printing
    printf("Process %d values: ", ZERO_RANK);
    for (int i = 0; i < size; i++) {
        printf("%d, ", minValues[i]);
    }
    printf("\n");
    // Cleaning
    free(minValues);
}

void otherProcess(int myRank, int size) {
    // Generation
    int *values = malloc(size * sizeof(int));
    srand(time(NULL) + myRank); // set seed (current time) for rand()
    printf("Process %d values: ", myRank);
    for (int i = 0; i < size; i++) {
        values[i] = rand() % 100;
        printf("%d, ", values[i]);
    }
    printf("\n");
    // Reducing
    MPI_Reduce(
        values,
        NULL,
        size,
        MPI_INT,
        MPI_MIN,
        ZERO_RANK,
        MPI_COMM_WORLD
    );
    // Cleaning
    free(values);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int myRank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    if (numberOfProcesses < 2) {
        if (myRank == 0) {
            printf("Please increase the number of processes more than 1.\n");
        }
    } else {
        int size = numberOfProcesses + 5;
        // Running
        double startTime = MPI_Wtime();
        if (myRank == ZERO_RANK) {
            processZero(size);
        } else {
            otherProcess(myRank, size);
        }
        double endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;
        printf("Elapsed time of process %d: %f seconds\n",
               myRank, elapsedTime);
    }
    MPI_Finalize();
    return 0;
}
