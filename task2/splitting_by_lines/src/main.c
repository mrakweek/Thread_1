#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void init(double *A, double *x, int n) {
    for (int i = 0; i < n * n; i++)
        A[i] = rand() % 10;
    for (int i = 0; i < n; i++)
        x[i] = rand() % 10;
}

void print_matrix(double *A, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%6.1f ", A[i * n + j]);
        printf("\n");
    }
}

void print_vector(double *x, int n) {
    for (int i = 0; i < n; i++)
        printf("%6.1f ", x[i]);
    printf("\n");
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0)
            printf("Usage: %s <matrix_size>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);
    double *A = NULL, *x = NULL, *y = NULL;
    double *local_A, *local_y;
    int rows_per_proc = n / size;

    local_A = (double *)malloc(rows_per_proc * n * sizeof(double));
    local_y = (double *)malloc(rows_per_proc * sizeof(double));

    if (rank == 0) {
        A = (double *)malloc(n * n * sizeof(double));
        x = (double *)malloc(n * sizeof(double));
        y = (double *)malloc(n * sizeof(double));

        srand(time(NULL));
        init(A, x, n);

        /*if (n <= 10) {
            printf("Исходная матрица A:\n");
            print_matrix(A, n);
            printf("\nИсходный вектор x:\n");
            print_vector(x, n);
            printf("\n");
        }*/
    }

    if (rank == 0)
        MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    else {
        x = (double *)malloc(n * sizeof(double));
        MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    MPI_Scatter(A, rows_per_proc * n, MPI_DOUBLE,
                local_A, rows_per_proc * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start = MPI_Wtime();

    for (int i = 0; i < rows_per_proc; i++) {
        local_y[i] = 0.0;
        for (int j = 0; j < n; j++)
            local_y[i] += local_A[i * n + j] * x[j];
    }

    double end = MPI_Wtime();

    MPI_Gather(local_y, rows_per_proc, MPI_DOUBLE,
               y, rows_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("%d %d %f\n", n, size, end - start);

      /*if (n <= 10) {
            printf("\nРезультирующий вектор y = A * x:\n");
            print_vector(y, n);
        }*/
    }

    free(local_A);
    free(local_y);
    free(x);
    if (rank == 0) {
        free(A);
        free(y);
    }

    MPI_Finalize();
    return 0;
}

