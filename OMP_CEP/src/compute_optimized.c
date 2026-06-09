#include <omp.h>//OpenMP
#include <x86intrin.h>//SIMD functions
#include "compute.h"

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
    
    //null pointer guard->Checks if any of the inputs is null
    if (!a_matrix || !b_matrix || !output_matrix)
        return -1;
    
    // Compute output dimensions based on "valid" convolution
    uint32_t out_rows = a_matrix->rows - b_matrix->rows + 1;
    uint32_t out_cols = a_matrix->cols - b_matrix->cols + 1;
    
    // Allocate output matrix struct
    matrix_t *out = malloc(sizeof(matrix_t));
    if (!out) return -1;


    out->rows = out_rows;
    out->cols = out_cols;

    // Allocate and zero-initialize output buffer (safe accumulation target)
    out->data = calloc(out_rows * out_cols, sizeof(int32_t));
    if (!out->data) {
        free(out);
        return -1;
    }

    // Cache kernel dimensions for faster repeated access
    uint32_t brs = b_matrix->rows;
    uint32_t bcs = b_matrix->cols;

    // Allocate space for flipped kernel (precomputed convolution kernel)
    int32_t *b_flip = malloc(brs * bcs * sizeof(int32_t));
    if (!b_flip) {
        free(out->data);
        free(out);
        return -1;
    }

    // Pre-flip kernel so convolution becomes straight dot-product
    // Flip kernel (algorithmic precompute)
    for (uint32_t r = 0; r < brs; r++) {
        for (uint32_t c = 0; c < bcs; c++) {
            b_flip[r * bcs + c] =
                b_matrix->data[(brs - 1 - r) * bcs + (bcs - 1 - c)];
        }
    }

    // Each output element is independent, so we can compute them in parallel(compiler optimization)[whereas SIMD is code optimization]
    // ===== OpenMP parallelization over output space =====
    #pragma omp parallel for collapse(2) schedule(static) //compiler directive
    //omp parallel for->split the loop accross multiple threads
    //collapse(2) means combining 2 loops into one
    //schedule(static) means divide work ahead of time;
    for (uint32_t r = 0; r < out_rows; r++) {
        for (uint32_t c = 0; c < out_cols; c++) {
            // Store result for this output cell
            int32_t total = 0;   // local accumulator (thread-safe)

            // Loop over each row of the kernel
            for (uint32_t br = 0; br < brs; br++) {
                // Pointer to the current position in input matrix row
                const int32_t *a_ptr =
                    &a_matrix->data[(r + br) * a_matrix->cols + c];

                // Pointer to the current row in the flipped kernel
                const int32_t *b_ptr =
                    &b_flip[br * bcs];
                
                // SIMD register used to sum 8 values at a time
                __m256i acc = _mm256_setzero_si256();//creates [0 0 0 0 0 0 0 0]

                uint32_t bc = 0;

                // Process 8 elements at once using SIMD
                for (; bc + 7 < bcs; bc += 8) {
                    // Load 8 values from input matrix
                    __m256i va = _mm256_loadu_si256((__m256i const*)(a_ptr + bc));//loads 8 integers (256=8*int32) unaligned
                    
                    // Load 8 values from kernel
                    __m256i vb = _mm256_loadu_si256((__m256i const*)(b_ptr + bc));
                    
                    // Multiply and add results into accumulator
                    acc = _mm256_add_epi32(//adds 8 integers in parallel
                        acc,
                        _mm256_mullo_epi32(va, vb)//multiplies v8*v8 in parallel
                    );
                }

                // Convert SIMD result to normal integers
                int32_t tmp[8];
                _mm256_storeu_si256((__m256i*)tmp, acc);//writes to normal memory from vector registers __m256i → int tmp[8]

                // Add up SIMD partial results
                int32_t sum =
                    tmp[0] + tmp[1] + tmp[2] + tmp[3] +
                    tmp[4] + tmp[5] + tmp[6] + tmp[7];

                // Handle leftover elements (not multiple of 8)
                for (; bc < bcs; bc++) {
                    sum += a_ptr[bc] * b_ptr[bc];
                }
                // Add this kernel row result to total
                total += sum;
            }
            // Store final result for this output position
            out->data[r * out_cols + c] = total;
        }
    }
    // Free temporary flipped kernel buffer
    free(b_flip);

    // Return result matrix via output pointer
    *output_matrix = out;
    return 0;
}
