#include "../src/compute.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "tests.h"

/*

Function: now_sec()

Purpose:
- Returns high-precision current time in seconds
- Used for benchmarking execution time of convolution

Why CLOCK_MONOTONIC?
- It is NOT affected by system clock changes
- Ensures stable and reliable timing measurements
- Essential for performance benchmarking

Return:
- Time in seconds (double precision)

*/

double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // Convert seconds + nanoseconds into a single floating-point value
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/*

MAIN FUNCTION - BENCHMARK DRIVER

Purpose:
- Measures execution time of convolution operation
- Compares performance of implementations (naive/optimized)
- Computes average runtime over multiple runs

Key Idea:
- Large input matrix (N x N) is used to stress the system
- Kernel (K x K) is small (3x3) to simulate real convolution use-case
- Multiple runs reduce noise in timing results

*/

int main() {
    /*
    -------------------------------------------------------
    Matrix Size Configuration
    -------------------------------------------------------
    N     = Size of input matrix (N x N)
    K     = Kernel size (K x K)
    RUNS  = Number of benchmark repetitions for averaging
    -------------------------------------------------------
    */
    const int N = 10000;   // very large matrix for stress testing
    const int K = 3;       // small convolution kernel
    const int RUNS = 5;    // repeat runs to get stable average timing

    /*
    =======================================================
    STEP 1: Allocate Input Matrix A
    =======================================================
    - Dynamically allocate memory for NxN integers
    - Fill with deterministic pattern (i % 10)
      so memory access is predictable and cache-friendly
    =======================================================
    */

    int *a = malloc(N * N * sizeof(int));
    for (int i = 0; i < N * N; i++) {
        a[i] = i % 10;
    }

    /*
    =======================================================
    STEP 2: Define Convolution Kernel B (3x3 filter)
    =======================================================
    This kernel acts like an edge-detection filter (Sobel-like)
    Used only for benchmarking performance, not correctness.
    =======================================================
    */
    int b[] = {
        1, 0, -1,
        1, 0, -1,
        1, 0, -1
    };

    /*
    -------------------------------------------------------
    Convert raw arrays into matrix_t structures
    - make_matrix() handles row/column metadata + allocation
    -------------------------------------------------------
    */

    matrix_t *A = make_matrix(N, N, a);
    matrix_t *B = make_matrix(K, K, b);

     matrix_t *out = NULL; // output matrix (allocated inside convolve)

    /*
    =======================================================
    STEP 3: Warm-up Run (VERY IMPORTANT for benchmarking)
    =======================================================

    Why warm-up?
    - First run includes cold cache effects
    - May include memory allocation overhead
    - May trigger CPU frequency scaling
    - Can distort timing results

    Therefore:
    - We run convolve once before measuring time
    - Then immediately free output
    =======================================================
    */

    convolve(A, B, &out);
    free_matrix(out);
    out = NULL;

    /*
    =======================================================
    STEP 4: Benchmark Loop
    =======================================================
    - Run convolution multiple times (RUNS = 5)
    - Measure execution time per run
    - Accumulate total time for averaging

    Why multiple runs?
    - Reduces noise from OS scheduling
    - Averages out fluctuations in CPU load
    =======================================================
    */

    double total = 0;

    for (int i = 0; i < RUNS; i++) {
        // Start timer before computation
        double start = now_sec();

        // Perform convolution
        convolve(A, B, &out);

        // Stop timer after computation
        double end = now_sec();

        // Add elapsed time for this run
        total += (end - start);

        /*
        ---------------------------------------------------
        Free output matrix after each run
        - Prevents memory accumulation
        - Ensures each run is independent
        ---------------------------------------------------
        */

        free_matrix(out);
        out = NULL;
    }

    /*
    =======================================================
    STEP 5: Print Average Execution Time
    =======================================================
    - total / RUNS gives stable performance estimate
    - This is the final benchmark result
    =======================================================
    */

    printf("Average time over %d runs: %f seconds\n",
           RUNS, total / RUNS);

    /*
    =======================================================
    STEP 6: Cleanup
    =======================================================
    Free all allocated memory:
    - A (matrix wrapper)
    - B (kernel matrix)
    - a (raw array)
    =======================================================
    */
    free_matrix(A);
    free_matrix(B);
    free(a);

    return 0;
}