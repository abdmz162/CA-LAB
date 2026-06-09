#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "io.h"

/**
 * Computes the 2D convolution of a_matrix (input) with b_matrix (kernel).
 * The kernel is flipped both horizontally and vertically (standard convolution).
 * Output matrix is allocated inside and returned via output_matrix.
 * Returns 0 on success, -1 on error.
 */
int convolve(matrix_t *a_matrix,
             matrix_t *b_matrix,
             matrix_t **output_matrix);