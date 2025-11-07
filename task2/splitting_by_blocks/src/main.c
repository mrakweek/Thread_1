#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int N = (argc > 1) ? atoi(argv[1]) : 1600;

    // Разбиваем матрицу на блоки по процессам (приблизительно квадратное разбиение)
    int dims[2] = {0, 0};
    MPI_Dims_create(comm_sz, 2, dims);

    int block_rows = N / dims[0] + (my_rank / dims[1] < N % dims[0]);
    int block_cols = N / dims[1] + (my_rank % dims[1] < N % dims[1]);

    double *A = malloc(block_rows * block_cols * sizeof(double));
    double *x = malloc(N * sizeof(double));
    double *y_local = calloc(block_rows, sizeof(double));

    if (my_rank == 0) {
        for (int i = 0; i < N; i++) x[i] = 1.0;
    }
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    srand(time(NULL) + my_rank);
    for (int i = 0; i < block_rows * block_cols; i++) {
        A[i] = rand() % 10;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    for (int i = 0; i < block_rows; i++) {
        for (int j = 0; j < block_cols; j++) {
            int global_col = (my_rank % dims[1]) * (N / dims[1]) + j;
            y_local[i] += A[i * block_cols + j] * x[global_col];
        }
    }

    double *y = NULL;
    if (my_rank == 0) y = malloc(N * sizeof(double));

    MPI_Reduce(y_local, y, block_rows, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();

    if (my_rank == 0) {
        printf("%d %d %lf\n", N, comm_sz, t_end - t_start);
    }

    free(A);
    free(x);
    free(y_local);
    if (my_rank == 0) free(y);

    MPI_Finalize();
    return 0;
}

