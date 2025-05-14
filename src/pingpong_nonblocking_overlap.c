#include <mpi.h>
#include <stdio.h>
#include <time.h>

void simulate_cpu_work(int ms) {
    clock_t start = clock();
    while ((clock() - start) * 1000 / CLOCKS_PER_SEC < ms) {
        // Busy wait – using the CPU
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, mpi_size;
    int ping_pong_count = 0;
    const int PING_PONG_LIMIT = 100;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    if (mpi_size != 2) {
        if (rank == 0)
            printf("This example requires exactly 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Request request;
    MPI_Status status;

    double start_time = MPI_Wtime();

    while (ping_pong_count < PING_PONG_LIMIT) {
        if (rank == ping_pong_count % 2) {
            // Sender
            ping_pong_count++;
            MPI_Isend(&ping_pong_count, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        } else {
            // Receiver: overlap computation while receiving
            MPI_Irecv(&ping_pong_count, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD, &request);
            simulate_cpu_work(10);  // work *during* receive
            MPI_Wait(&request, &status);
        }
    }

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("pingpong_nonblocking_overlap time: %.4f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
