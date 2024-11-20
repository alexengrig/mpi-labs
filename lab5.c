#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int myRank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    // Setting
    int N;
    double A = 0.0, SUM = 0.0;
    srand(time(NULL) + myRank);
    if (myRank == 0) {
        N = 1;
        A = (double) (rand() % 100) / 10.0;
    } else {
        N = rand() % 2;
        A = (double) (rand() % 100) / 10.0;
    }
    printf("Process %d: N = %d, A = %f\n",
           myRank, N, A);
    // Running
    double startTime = MPI_Wtime();
    MPI_Comm ourComm;
    MPI_Comm_split(
        MPI_COMM_WORLD,
        N == 1 ? 1 : MPI_UNDEFINED,
        myRank,
        &ourComm
    );
    if (N == 1) {
        MPI_Allreduce(
            &A,
            &SUM,
            1,
            MPI_DOUBLE,
            MPI_SUM,
            ourComm
        );
        printf("Process %d: SUM = %f\n",
               myRank, SUM);
    }
    double endTime = MPI_Wtime();
    double elapsedTime = endTime - startTime;
    printf("Process %d: %f seconds\n",
           myRank, elapsedTime);
    if (ourComm != MPI_COMM_NULL) {
        MPI_Comm_free(&ourComm);
    }
    MPI_Finalize();
    return 0;
}
