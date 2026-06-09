#ifndef TESTS_H
#define TESTS_H

/*

HEADER FILE: tests.h

Purpose:
This file provides helper utility functions used in:
- test_driver.c (correctness testing)
- bench_test.c (benchmarking setup)

It defines:
1. make_matrix() → creates a matrix_t from raw array
2. free_matrix() → safely frees allocated memory
3. equal()       → compares two matrices for correctness

It depends on:
- io.h → defines the matrix_t structure

*/

#include "../src/io.h"
#include <stdlib.h>
#include <stdio.h>

/*

Function: make_matrix()

Purpose:
- Converts a raw integer array into a matrix_t structure
- Allocates memory dynamically for both:
    1. matrix structure
    2. internal data array

Parameters:
- rows : number of rows in matrix
- cols : number of columns in matrix
- data : pointer to input integer array (row-major order)

Return:
- Pointer to dynamically created matrix_t structure

*/
matrix_t* make_matrix(int rows, int cols, int data[])
{
    /*
    -------------------------------------------------------
    Step 1: Allocate memory for matrix structure itself
    -------------------------------------------------------
    matrix_t typically contains:
    - int rows
    - int cols
    - int32_t *data (flattened array)
    -------------------------------------------------------
    */
    matrix_t *m = malloc(sizeof(matrix_t));

    // Store dimensions
    m->rows = rows;
    m->cols = cols;

    /*
    -------------------------------------------------------
    Step 2: Allocate memory for matrix elements
    -------------------------------------------------------
    We store matrix in a flattened 1D array:
    size = rows * cols

    Why 1D array?
    - Better cache performance
    - Easier pointer arithmetic in convolution
    - Standard practice in low-level optimization tasks
    -------------------------------------------------------
    */
    m->data = malloc(
        rows * cols * sizeof(int32_t)
    );

    /*
    -------------------------------------------------------
    Step 3: Copy input data into allocated memory
    -------------------------------------------------------
    This ensures:
    - Matrix owns its own memory (no dependency on caller array)
    - Safe lifetime management
    -------------------------------------------------------
    */
    for(int i = 0; i < rows * cols; i++)
        m->data[i] = data[i];

    return m;
}

/*

Function: free_matrix()

Purpose:
- Frees all dynamically allocated memory for a matrix_t

Why needed?
- Avoids memory leaks
- Each make_matrix() must be paired with free_matrix()

Memory structure:
1. m->data (int array)
2. m (matrix structure itself)

*/
void free_matrix(matrix_t *m)
{
    free(m->data);  // free internal array first
    free(m);        // then free structure itself
}

/*

Function: equal()

Purpose:
- Checks whether two matrices are identical
- Used for correctness validation in test_driver.c

Comparison rules:
1. Same number of rows
2. Same number of columns
3. All corresponding elements must match exactly

Return:
- 1 → matrices are equal
- 0 → matrices are different

*/
int equal(matrix_t *a, matrix_t *b)
{
    /*
    -------------------------------------------------------
    Step 1: Compare dimensions
    If shape is different → matrices cannot be equal
    -------------------------------------------------------
    */
    if(a->rows != b->rows)
        return 0;

    if(a->cols != b->cols)
        return 0;

    /*
    -------------------------------------------------------
    Step 2: Compare elements
    Since matrix is stored as 1D array (row-major),
    total elements = rows * cols
    -------------------------------------------------------
    */
    int n = a->rows * a->cols;

    for(int i = 0; i < n; i++)
    {
        if(a->data[i] != b->data[i])
            return 0; // mismatch found → not equal
    }

    /*
    -------------------------------------------------------
    If no mismatches found → matrices are identical
    -------------------------------------------------------
    */
    return 1;
}

#endif