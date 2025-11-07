#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Размер матрицы
    int N = (argc > 1) ? atoi(argv[1]) : 1600;

    // Выделяем память для локальной части матрицы
    int local_cols = N / comm_sz;
    if (my_rank < N % comm_sz) local_cols++;  // распределяем остаток

    double *A = malloc(N * local_cols * sizeof(double));
    double *x = malloc(N * sizeof(double));
    double *y_local = malloc(N * sizeof(double));

    // Инициализация данных
    if (my_rank == 0) {
        for (int i = 0; i < N; i++)
            x[i] = 1.0;  // вектор единиц
    }
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    srand(time(NULL) + my_rank);
    for (int i = 0; i < N * local_cols; i++)
        A[i] = rand() % 10;

    // Таймер
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    // Умножение матрицы на вектор (разбиение по столбцам)
    for (int i = 0; i < N; i++)
        y_local[i] = 0.0;

    for (int j = 0; j < local_cols; j++) {
        for (int i = 0; i < N; i++) {
            y_local[i] += A[i * local_cols + j] * x[j + my_rank * (N / comm_sz)];
        }
    }

    // Сбор результата
    double *y = NULL;
    if (my_rank == 0)
        y = malloc(N * sizeof(double));

    MPI_Reduce(y_local, y, N, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();

    // Вывод: размер, число процессов, время
    if (my_rank == 0)
        printf("%d %d %lf\n", N, comm_sz, t_end - t_start);

    free(A);
    free(x);
    free(y_local);
    if (my_rank == 0) free(y);

    MPI_Finalize();
    return 0;
}

