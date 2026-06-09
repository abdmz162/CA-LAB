#include "compute.h"

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix,
             matrix_t **output_matrix) {

    if (!a_matrix || !b_matrix || !output_matrix)
        return -1;

    // Output dimensions
    uint32_t out_rows =
        a_matrix->rows - b_matrix->rows + 1;

    uint32_t out_cols =
        a_matrix->cols - b_matrix->cols + 1;

    // Allocate output matrix struct
    matrix_t *out = malloc(sizeof(matrix_t));
    if (!out)
        return -1;

    out->rows = out_rows;
    out->cols = out_cols;

    // Allocate output data array
    out->data = malloc(
        out_rows * out_cols * sizeof(int32_t)
    );

    if (!out->data) {
        free(out);
        return -1;
    }

    // Compute convolution
    for (uint32_t r = 0; r < out_rows; r++) {

        for (uint32_t c = 0; c < out_cols; c++) {

            int32_t sum = 0;

            for (uint32_t br = 0; br < b_matrix->rows; br++) {

                for (uint32_t bc = 0; bc < b_matrix->cols; bc++) {

                    // A[r+br][c+bc]
                    int32_t a_val =
                        a_matrix->data[
                            (r + br) * a_matrix->cols
                            + (c + bc)
                        ];

                    // Flipped B
                    int32_t b_val =
                        b_matrix->data[
                            (b_matrix->rows - 1 - br)
                            * b_matrix->cols
                            + (b_matrix->cols - 1 - bc)
                        ];

                    sum += a_val * b_val;
                }
            }

            out->data[
                r * out_cols + c
            ] = sum;
        }
    }

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
