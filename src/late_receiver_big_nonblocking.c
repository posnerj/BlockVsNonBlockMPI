#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
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

    const int size = 1000000; // ~4 MB
    int* data = (int*)malloc(size * sizeof(int));
    MPI_Request request;
    MPI_Status status;

    if (rank == 0) {
        simulate_cpu_work(1000); // Empfänger kommt zu spät
        printf("Rank 0: posting Irecv...\n");
        MPI_Irecv(data, size, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        printf("Rank 0: received (non-blocking).\n");
    } else if (rank == 1) {
        printf("Rank 1: starting Isend...\n");
        MPI_Isend(data, size, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
// Optional:  Optional: a little "real work" while receicving
//        simulate_cpu_work(100);  // 100 ms busy work
        MPI_Wait(&request, &status);
        printf("Rank 1: send complete (non-blocking).\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
    free(data);
    MPI_Finalize();
    return 0;
}
