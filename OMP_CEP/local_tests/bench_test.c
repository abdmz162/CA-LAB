#include "../src/compute.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "tests.h"


double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main() {
    const int N = 10000;
    const int K = 3;
    const int RUNS = 5;

    // -------------------------
    // Allocate and build A
    // -------------------------
    int *a = malloc(N * N * sizeof(int));
    for (int i = 0; i < N * N; i++) {
        a[i] = i % 10;
    }

    // -------------------------
    // Kernel B (3x3)
    // -------------------------
    int b[] = {
        1, 0, -1,
        1, 0, -1,
        1, 0, -1
    };

    matrix_t *A = make_matrix(N, N, a);
    matrix_t *B = make_matrix(K, K, b);

    matrix_t *out = NULL;

    // -------------------------
    // Warm-up run (important!)
    // -------------------------
    convolve(A, B, &out);
    free_matrix(out);
    out = NULL;

    // -------------------------
    // Benchmark loop
    // -------------------------
    double total = 0;

    for (int i = 0; i < RUNS; i++) {
        double start = now_sec();

        convolve(A, B, &out);

        double end = now_sec();
        total += (end - start);

        free_matrix(out);
        out = NULL;
    }

    printf("Average time over %d runs: %f seconds\n",
           RUNS, total / RUNS);

    // cleanup
    free_matrix(A);
    free_matrix(B);
    free(a);

    return 0;
}