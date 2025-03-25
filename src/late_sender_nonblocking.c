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
    MPI_Request request;
    MPI_Status status;

    if (rank == 0) {
        // Receiver is ready immediately
        printf("Rank 0: posting Irecv...\n");
        MPI_Irecv(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);

//         Do some work while waiting
//        simulate_cpu_work(100);  // e.g. 100 ms
        MPI_Wait(&request, &status);
        printf("Rank 0: received (non-blocking).\n");
    } else if (rank == 1) {
        // Simulate computation before sending
        simulate_cpu_work(1000);  // 1 s delay
        printf("Rank 1: sending...\n");
        MPI_Isend(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        printf("Rank 1 sent data: %d\n", data);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
