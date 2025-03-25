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
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    if (mpi_size != 2) {
        if (rank == 0)
            printf("This example requires exactly 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    int data = 42;

    if (rank == 0) {
        // Simulate work before receiving
        simulate_cpu_work(1000);  // 1 s delay
        printf("Rank 0: receiving...\n");
        MPI_Recv(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank 0 received data: %d\n", data);
    } else if (rank == 1) {
        // Send immediately
        // Because of small message the eager protocol exploits buffers and the call does not block
        printf("Rank 1: sending...\n");
        MPI_Send(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Rank 1 sent data: %d\n", data);
    }

    MPI_Finalize();
    return 0;
}
