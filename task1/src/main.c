#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int comm_sz, my_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (argc < 2) {
        if (my_rank == 0)
            fprintf(stderr, "Usage: %s <total_trials>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    long long total_trials = atoll(argv[1]);
    if (total_trials <= 0) {
        if (my_rank == 0) fprintf(stderr, "total_trials must be > 0\n");
        MPI_Finalize();
        return 1;
    }

    long long base = total_trials / comm_sz;
    long long rem = total_trials % comm_sz;
    long long local_n = base + (my_rank < rem ? 1 : 0);

    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(my_rank * 0x9e3779b9);

    long long local_hits = 0;

    double start_time = MPI_Wtime(); // старт таймера

    for (long long i = 0; i < local_n; ++i) {
        double x = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
        double y = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
        if (x*x + y*y <= 1.0) ++local_hits;
    }

    long long global_hits = 0;
    MPI_Reduce(&local_hits, &global_hits, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime(); // конец таймера
    double elapsed = end_time - start_time;

    if (my_rank == 0) {
        double pi_est = 4.0 * (double)global_hits / (double)total_trials;
        // вывод в CSV формате: processes,trials,hits,pi,time
        printf("%d,%lld,%lld,%.6f,%.6f\n", comm_sz, total_trials, global_hits, pi_est, elapsed);
    }

    MPI_Finalize();
    return 0;
}

