#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "io.h"

// Executes a task
int execute_task(task_t *task);

// Computes convolution
int convolve(matrix_t *a_matrix,
             matrix_t *b_matrix,
             matrix_t **output_matrix);