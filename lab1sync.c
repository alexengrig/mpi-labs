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
        printf("\nHello from process %3d\n", myRank);
        for (int sourceRank = 1; sourceRank < numberOfProcesses; sourceRank++) {
            MPI_Recv(
                &senderRank, 1,
                MPI_INT, sourceRank,
                MPI_ANY_TAG, MPI_COMM_WORLD,
                &Status
            );
            printf("Hello from process %3d\n", senderRank);
        }
    } else {
        MPI_Send(&myRank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
