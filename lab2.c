#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DEFAULT_ARRAY_SIZE 16

void bubbleSort(int *array, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void printArray(int *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void processZero(
    int numberOfProcesses, int arraySize,
    int chunkSize, int remainder, int benchmark
) {
    // Array generation
    int *array = malloc(arraySize * sizeof(int));
    srand(time(NULL)); // set seed (current time) for rand()
    for (int i = 0; i < arraySize; i++) {
        array[i] = rand() % 100;
    }
    // Printing
    printf("Array size: %d, chunk size: %d\n", arraySize, chunkSize);
    // Sending
    for (int sourceRank = 1; sourceRank < numberOfProcesses; sourceRank++) {
        int currentChunkSize = sourceRank == numberOfProcesses - 1
                                   ? chunkSize + remainder
                                   : chunkSize;
        MPI_Send(
            &array[(sourceRank - 1) * chunkSize], currentChunkSize,
            MPI_INT, sourceRank,
            0, MPI_COMM_WORLD
        );
    }
    // Receiving
    int *sortedArray = malloc(arraySize * sizeof(int));
    for (int sourceRank = 1; sourceRank < numberOfProcesses; sourceRank++) {
        int currentChunkSize = sourceRank == numberOfProcesses - 1
                                   ? chunkSize + remainder
                                   : chunkSize;
        MPI_Recv(
            &sortedArray[(sourceRank - 1) * chunkSize], currentChunkSize,
            MPI_INT, sourceRank,
            0, MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    // Printing
    if (benchmark != 1) {
        printf("Sorted array by chunks: ");
        printArray(sortedArray, arraySize);
    }
    // Sorting
    bubbleSort(sortedArray, arraySize);
    // Final printing
    if (benchmark != 1) {
        printf("Final sorted array: ");
        printArray(sortedArray, arraySize);
    }
    // Cleaning
    free(array);
    free(sortedArray);
}

void otherProcesses(int chunkSize) {
    int *chunk = malloc(chunkSize * sizeof(int));
    int zeroRank = 0;
    // Receiving
    MPI_Recv(
        chunk, chunkSize,
        MPI_INT, zeroRank,
        0, MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );
    // Sorting
    bubbleSort(chunk, chunkSize);
    // Sending
    MPI_Send(
        chunk, chunkSize,
        MPI_INT, zeroRank,
        0, MPI_COMM_WORLD
    );
    // Cleaning
    free(chunk);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int myRank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    int arraySize = argc > 1 ? atoi(argv[1]) : DEFAULT_ARRAY_SIZE;
    if (numberOfProcesses < 2) {
        if (myRank == 0) {
            printf("Please increase the number of processes more than 1.\n");
        }
    } else if (arraySize < numberOfProcesses - 1) {
        if (myRank == 0) {
            printf("The array size must be greater than or equal"
                " to the number of processes.\n");
        }
    } else {
        int benchmark = (argc > 2 && strcmp(argv[2], "benchmark") == 0) ? 1 : 0;
        int chunkSize = arraySize / (numberOfProcesses - 1);
        int remainder = arraySize % (numberOfProcesses - 1);
        // Running
        double startTime = MPI_Wtime();
        if (myRank == 0) {
            processZero(numberOfProcesses, arraySize,
                        chunkSize, remainder, benchmark);
        } else {
            int currentChunkSize = myRank == numberOfProcesses - 1
                                       ? chunkSize + remainder
                                       : chunkSize;
            otherProcesses(currentChunkSize);
        }
        double endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;
        if (benchmark == 0 || myRank == 0) {
            printf("Elapsed time of process %d: %f seconds\n",
                   myRank, elapsedTime);
        }
    }
    MPI_Finalize();
    return 0;
}
