#include <mpi.h>
#include <stdio.h>

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

    double start_time = MPI_Wtime();

    while (ping_pong_count < PING_PONG_LIMIT) {
        if (rank == ping_pong_count % 2) {
            ping_pong_count++;
            MPI_Send(&ping_pong_count, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&ping_pong_count, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("pingpong_blocking time: %.4f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
