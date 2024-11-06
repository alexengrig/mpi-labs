#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DATA_TAG 0
#define PACKET_SIZE 22

typedef struct {
    int dest;
    int source;
    char data[PACKET_SIZE]; // data or approve or kill
} Packet;

void processZero(int numberOfProcesses, int benchmark) {
    // Communication
    MPI_Status status;
    Packet packet;
    for (int i = 0; i < (numberOfProcesses - 1) * 2; i++) {
        // Receiving
        if (benchmark != 1) {
            printf("Process 0 waits packet from any process\n");
        }
        MPI_Recv(
            &packet, sizeof(Packet),
            MPI_BYTE, MPI_ANY_SOURCE,
            DATA_TAG, MPI_COMM_WORLD,
            &status
        );
        if (benchmark != 1) {
            printf("Process 0 received packet from process %d"
                   " for process %d with data: %s\n",
                   packet.source, packet.dest, packet.data);
        }
        // Sending
        if (benchmark != 1) {
            printf("Process 0 sends packet to process %d"
                   " from process %d with data: %s\n",
                   packet.dest, packet.source, packet.data);
        }
        MPI_Send(
            &packet, sizeof(Packet),
            MPI_BYTE, packet.dest,
            DATA_TAG, MPI_COMM_WORLD
        );
        if (benchmark != 1) {
            printf("Process 0 sent packet to process %d"
                   " from process %d with data: %s\n",
                   packet.dest, packet.source, packet.data);
        }
    }
    // Termination
    packet.source = 0;
    snprintf(packet.data, PACKET_SIZE, "SIGTERM"); // str to array
    for (int rank = 1; rank < numberOfProcesses; rank++) {
        if (benchmark != 1) {
            printf("Process 0 sends SIGTERM to process %d\n", rank);
        }
        packet.dest = rank;
        MPI_Send(
            &packet, sizeof(Packet),
            MPI_BYTE, rank,
            DATA_TAG, MPI_COMM_WORLD
        );
        if (benchmark != 1) {
            printf("Process 0 sent SIGTERM to process %d\n", rank);
        }
    }
}

void otherProcess(int numberOfProcesses, int myRank, int benchmark) {
    // Generation
    Packet packet;
    packet.source = myRank;
    srand(time(NULL) + myRank); // set seed (time + rank) for rand()
    do {
        packet.dest = 1 + rand() % (numberOfProcesses - 1);
    } while (packet.dest == myRank);
    snprintf(packet.data, PACKET_SIZE, "Hello from %d", myRank); // str to array
    // Sending
    MPI_Status status;
    int zeroRank = 0;
    if (benchmark != 1) {
        printf("Process %d sends packet to process %d"
               " for process %d with data: %s\n",
               myRank, zeroRank, packet.dest, packet.data);
    }
    MPI_Send(
        &packet, sizeof(Packet),
        MPI_BYTE, zeroRank,
        DATA_TAG, MPI_COMM_WORLD
    );
    if (benchmark != 1) {
        printf("Process %d sent packet to process %d"
               " for process %d with data: %s\n",
               myRank, zeroRank, packet.dest, packet.data);
    }
    // Receiving and sending confirmation
    for (int i = 0; i < 1 + (numberOfProcesses - 1); i++) {
        // Receiving
        if (benchmark != 1) {
            printf("Process %d waits packet from process 0\n", myRank);
        }
        MPI_Recv(
            &packet, sizeof(Packet),
            MPI_BYTE, zeroRank,
            DATA_TAG, MPI_COMM_WORLD,
            &status
        );
        // Termination
        if (strcmp(packet.data, "SIGTERM") == 0) {
            if (benchmark != 1) {
                printf("Process %d is terminating\n", myRank);
            }
            break;
        }
        printf("Process %d received packet from process %d"
               " by process 0 with data: %s\n",
               myRank, packet.source, packet.data);
        // Sending confirmation
        if (strncmp(packet.data, "Received by ", 12) != 0) {
            packet.dest = packet.source;
            packet.source = myRank;
            snprintf(packet.data, PACKET_SIZE, "Received by %d", myRank);
            // Sending confirmation
            if (benchmark != 1) {
                printf("Process %d sends confirmation to process %d"
                       " by process 0 with data: %s\n",
                       myRank, packet.dest, packet.data);
            }
            MPI_Send(
                &packet, sizeof(Packet),
                MPI_BYTE, zeroRank,
                DATA_TAG, MPI_COMM_WORLD
            );
            if (benchmark != 1) {
                printf("Process %d sent confirmation to process %d"
                       " by process 0 with data: %s\n",
                       myRank, packet.dest, packet.data);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int myRank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
    if (numberOfProcesses < 3) {
        if (myRank == 0) {
            printf("Please increase the number of processes more than 2.\n");
        }
    } else {
        int benchmark = argc > 1 && strcmp(argv[1], "benchmark") == 0 ? 1 : 0;
        // Running
        double startTime = MPI_Wtime();
        if (myRank == 0) {
            processZero(numberOfProcesses, benchmark);
        } else {
            otherProcess(numberOfProcesses, myRank, benchmark);
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
