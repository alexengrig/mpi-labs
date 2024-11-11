#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int myRank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    // Receiving or Sending
    if (myRank == 0) {
        int senderRank;
        MPI_Status Status;
        printf("Hello from process %3d\n", myRank);
        double startTime = MPI_Wtime();
        for (int i = 1; i < numberOfProcesses; i++) {
            MPI_Recv(
                &senderRank, 1,
                MPI_INT, MPI_ANY_SOURCE,
                MPI_ANY_TAG, MPI_COMM_WORLD,
                &Status
            );
            printf("Hello from process %3d\n", senderRank);
        }
        double endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;
        printf("Elapsed time of process %d: %f seconds\n", myRank, elapsedTime);
    } else {
        double startTime = MPI_Wtime();
        MPI_Send(&myRank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        double endTime = MPI_Wtime();
        double elapsedTime = endTime - startTime;
        printf("Elapsed time of process %d: %f seconds\n", myRank, elapsedTime);
    }

    MPI_Finalize();
    return 0;
}
