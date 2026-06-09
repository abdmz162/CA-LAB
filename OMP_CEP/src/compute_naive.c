#include "compute.h"

/**
 * Naive convolution implementation.
 * Uses four nested loops, flipping the kernel on the fly by index arithmetic.
 */
int convolve(matrix_t *a_matrix, matrix_t *b_matrix,
             matrix_t **output_matrix) {
    
    //null pointer guard->Checks if any of the inputs is null
    if (!a_matrix || !b_matrix || !output_matrix)
        return -1;

    // Convolution output size:
    // (input dimension - kernel dimension + 1)
    uint32_t out_rows =
        a_matrix->rows - b_matrix->rows + 1;

    uint32_t out_cols =
        a_matrix->cols - b_matrix->cols + 1;

    // Allocate memory for the output matrix structure.
    matrix_t *out = malloc(sizeof(matrix_t));
    if (!out)
        return -1;
    // Store computed output dimensions.
    out->rows = out_rows;
    out->cols = out_cols;

    // Allocate contiguous memory for all output elements.
    out->data = malloc(
        out_rows * out_cols * sizeof(int32_t)
    );
    // Clean up and fail if data allocation was unsuccessful.
    if (!out->data) {
        free(out);
        return -1;
    }

    // Compute convolution
    // Slide the kernel across every valid output position[Outer loops (r, c)]
    for (uint32_t r = 0; r < out_rows; r++) {

        for (uint32_t c = 0; c < out_cols; c++) {
            // Accumulator for the current output cell.
            int32_t sum = 0;
            // Inner loops (br, bc): for each position, multiply every kernel element by the corresponding input element and accumulate the sum.
            // Iterate through every element of the kernel.
            for (uint32_t br = 0; br < b_matrix->rows; br++) {

                for (uint32_t bc = 0; bc < b_matrix->cols; bc++) {

                    // A[r+br][c+bc] indexing input matrix
                    int32_t a_val =
                        a_matrix->data[
                            (r + br) * a_matrix->cols
                            + (c + bc)
                        ];

                    // Flipped B(on the fly)
                    int32_t b_val =
                        b_matrix->data[
                            (b_matrix->rows - 1 - br)//last row becomes first
                            * b_matrix->cols
                            + (b_matrix->cols - 1 - bc)//last column becomes first
                        ];
                    // Accumulate the element-wise product.
                    sum += a_val * b_val;
                }
            }
            // Store the completed convolution result.
            out->data[
                r * out_cols + c
            ] = sum;
        }
    }
    // Return the newly allocated output matrix to the caller.
    *output_matrix = out;

    return 0;
}
