#include <omp.h>
#include <x86intrin.h>
#include "compute.h"

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
    if (!a_matrix || !b_matrix || !output_matrix)
        return -1;

    uint32_t out_rows = a_matrix->rows - b_matrix->rows + 1;
    uint32_t out_cols = a_matrix->cols - b_matrix->cols + 1;

    matrix_t *out = malloc(sizeof(matrix_t));
    if (!out) return -1;

    out->rows = out_rows;
    out->cols = out_cols;

    out->data = calloc(out_rows * out_cols, sizeof(int32_t));
    if (!out->data) {
        free(out);
        return -1;
    }

    uint32_t brs = b_matrix->rows;
    uint32_t bcs = b_matrix->cols;

    int32_t *b_flip = malloc(brs * bcs * sizeof(int32_t));
    if (!b_flip) {
        free(out->data);
        free(out);
        return -1;
    }

    // Flip kernel (algorithmic precompute)
    for (uint32_t r = 0; r < brs; r++) {
        for (uint32_t c = 0; c < bcs; c++) {
            b_flip[r * bcs + c] =
                b_matrix->data[(brs - 1 - r) * bcs + (bcs - 1 - c)];
        }
    }

    // ===== OpenMP parallelization over output space =====
    #pragma omp parallel for collapse(2) schedule(static)
    for (uint32_t r = 0; r < out_rows; r++) {
        for (uint32_t c = 0; c < out_cols; c++) {

            int32_t total = 0;   // local accumulator (thread-safe)

            for (uint32_t br = 0; br < brs; br++) {

                const int32_t *a_ptr =
                    &a_matrix->data[(r + br) * a_matrix->cols + c];

                const int32_t *b_ptr =
                    &b_flip[br * bcs];

                __m256i acc = _mm256_setzero_si256();

                uint32_t bc = 0;

                // SIMD loop (8-wide)
                for (; bc + 7 < bcs; bc += 8) {
                    __m256i va = _mm256_loadu_si256((__m256i const*)(a_ptr + bc));
                    __m256i vb = _mm256_loadu_si256((__m256i const*)(b_ptr + bc));

                    acc = _mm256_add_epi32(
                        acc,
                        _mm256_mullo_epi32(va, vb)
                    );
                }

                int32_t tmp[8];
                _mm256_storeu_si256((__m256i*)tmp, acc);

                int32_t sum =
                    tmp[0] + tmp[1] + tmp[2] + tmp[3] +
                    tmp[4] + tmp[5] + tmp[6] + tmp[7];

                // tail
                for (; bc < bcs; bc++) {
                    sum += a_ptr[bc] * b_ptr[bc];
                }

                total += sum;
            }

            out->data[r * out_cols + c] = total;
        }
    }

    free(b_flip);
    *output_matrix = out;
    return 0;
}

// Executes a task
int execute_task(task_t *task) {
  matrix_t *a_matrix, *b_matrix, *output_matrix;

  char *a_matrix_path = get_a_matrix_path(task);
  if (read_matrix(a_matrix_path, &a_matrix)) {
    printf("Error reading matrix from %s\n", a_matrix_path);
    return -1;
  }
  free(a_matrix_path);

  char *b_matrix_path = get_b_matrix_path(task);
  if (read_matrix(b_matrix_path, &b_matrix)) {
    printf("Error reading matrix from %s\n", b_matrix_path);
    return -1;
  }
  free(b_matrix_path);

  if (convolve(a_matrix, b_matrix, &output_matrix)) {
    printf("convolve returned a non-zero integer\n");
    return -1;
  }

  char *output_matrix_path = get_output_matrix_path(task);
  if (write_matrix(output_matrix_path, output_matrix)) {
    printf("Error writing matrix to %s\n", output_matrix_path);
    return -1;
  }
  free(output_matrix_path);

  free(a_matrix->data);
  free(b_matrix->data);
  free(output_matrix->data);
  free(a_matrix);
  free(b_matrix);
  free(output_matrix);
  return 0;
}
