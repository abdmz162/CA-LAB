#include "../src/compute.h"
#include "tests.h"

/*

Function: now_sec()
Purpose  : Returns current time in seconds with high precision
           using a monotonic clock.

Why MONOTONIC?
- CLOCK_MONOTONIC ensures time is always increasing
- It is NOT affected by system clock changes (NTP, manual change)
- Perfect for benchmarking execution time

Return:
- double value representing time in seconds (including nanoseconds)

*/

double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    // Convert seconds + nanoseconds into a single floating-point value
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/*

Function: run_test()

Purpose:
- Runs a single convolution test case
- Compares computed output with expected output
- Prints PASS/FAIL result
- Frees all dynamically allocated memory

Parameters:
- name     : Name of the test case (for printing result)
- A        : Input matrix (image / signal)
- B        : Kernel matrix (filter)
- Expected : Expected output matrix (ground truth)

Key Idea:
- convolve(A, B, &out) computes convolution result
- equal(out, Expected) checks correctness

*/

void run_test(
    char *name,
    matrix_t *A,
    matrix_t *B,
    matrix_t *Expected
)
{
    matrix_t *out=NULL;  // output matrix will be created by convolve()

     /*
    -------------------------------------------------------
    Perform convolution operation
    - result == 0 → success
    - result != 0 → error in computation or invalid input
    -------------------------------------------------------
    */

    int result=
        convolve(A,B,&out);

    if(result!=0)
    {
        printf("%s FAILED\n",name);
        return;
    }

    /*
    -------------------------------------------------------
    Compare computed output with expected output
    - equal() checks element-wise equality
    -------------------------------------------------------
    */

    if(equal(out,Expected))
        printf("%s PASSED\n",name);
    else
        printf("%s FAILED\n",name);

     /*
    -------------------------------------------------------
    Memory management:
    All matrices are dynamically allocated using make_matrix().
    Therefore we MUST free them to avoid memory leaks.
    -------------------------------------------------------
    */


    free_matrix(A);
    free_matrix(B);
    free_matrix(Expected);
    free_matrix(out);
}


/*

MAIN FUNCTION - TEST SUITE DRIVER


This function runs multiple test cases for convolution:
- Small examples (manual verification)
- Edge cases (negative values, 1x1 kernel)
- Medium and large matrices

Also measures total execution time of all tests.

*/

int main()
{
    double start = now_sec(); // start timer for full test suite
    
    // =====================================================
    // TEST 1: Basic 3x3 image with 2x2 kernel
    // Purpose: sanity check for normal convolution
    // =====================================================

    int a1[]={
        1,2,3,
        4,5,6,
        7,8,9
    };

    int b1[]={  // it(kernel) will be flipped 
        1,2,
        3,4
    };

    int e1[]={
        23,33,
        53,63
    };

    run_test(
        "basic",
        make_matrix(3,3,a1),
        make_matrix(2,2,b1),
        make_matrix(2,2,e1)
    );


    // =====================================================
    // TEST 2: 1x1 kernel
    // Purpose: kernel is scalar → output should scale input
    // =====================================================
    int a2[]={
        1,2,
        3,4
    };

    int b2[]={5};

    int e2[]={
        5,10,
        15,20
    };

    run_test(
        "1x1 kernel",
        make_matrix(2,2,a2),
        make_matrix(1,1,b2),
        make_matrix(2,2,e2)
    );


    // =====================================================
    // TEST 3: Negative values
    // Purpose: ensures convolution handles negatives correctly
    // =====================================================
    int a3[]={
        1,-2,
        3,-4
    };

    int b3[]={
        1,2,
        3,4
    };

    int e3[]={0};

    run_test(
        "negative",
        make_matrix(2,2,a3),
        make_matrix(2,2,b3),
        make_matrix(1,1,e3)
    );

    // ---------- test_tiny ----------

    // =====================================================
    // TEST 4: Same-size kernel and input
    // Purpose: output becomes single value (full overlap)
    // =====================================================
    int a4[]={
        1,2,
        3,4
    };

    int b4[]={
        1,2,
        3,4
    };

    int e4[]={20};

    run_test(
        "tiny_same_size",
        make_matrix(2,2,a4),
        make_matrix(2,2,b4),
        make_matrix(1,1,e4)
    );



    // =====================================================
    // TEST 5: Rectangular input matrix
    // Purpose: non-square image convolution test
    // =====================================================
    int a5[] = {
        1,2,3,4,
        5,6,7,8
    };

    int b5[] = {
        1,2,
        3,4
    };

    int e5_true[] = {
        26, 36, 46
    };

    run_test(
        "tiny_rectangular",
        make_matrix(2,4,a5),
        make_matrix(2,2,b5),
        make_matrix(1,3,e5_true)
    );


    // ---------- test_small ----------

    // =====================================================
    // TEST 6: Small 5x5 with 3x3 kernel
    // Purpose: standard mid-size convolution test
    // =====================================================
    int a6[]={
        1,2,3,4,5,
        6,7,8,9,10,
        11,12,13,14,15,
        16,17,18,19,20,
        21,22,23,24,25
    };

    int b6[]={
        1,2,3,
        4,5,6,
        7,8,9
    };

    int e6[]={
        219,264,309,
        444,489,534,
        669,714,759
    };

    run_test(
        "small_5x5",
        make_matrix(5,5,a6),
        make_matrix(3,3,b6),
        make_matrix(3,3,e6)
    );


    // =====================================================
    // TEST 7: Negative matrix values
    // Purpose: mixed sign robustness test
    // =====================================================
    int a7[]={
        -1,-2,-3,
        4, 5, 6,
        -7,-8,-9
    };

    int b7[]={
        1,2,
        3,4
    };

    int e7[] = {
    3, -1,
    9, 13
    };

    run_test(
        "small_negative",
        make_matrix(3,3,a7),
        make_matrix(2,2,b7),
        make_matrix(2,2,e7)
    );



        // ---------- test_large ----------
    // =====================================================
    // TEST 8: Large 10x10 matrix
    // Purpose: stress test for correctness + performance
    // =====================================================
    int a_large[] = {
        1,2,3,4,5,6,7,8,9,10,
        11,12,13,14,15,16,17,18,19,20,
        21,22,23,24,25,26,27,28,29,30,
        31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,
        51,52,53,54,55,56,57,58,59,60,
        61,62,63,64,65,66,67,68,69,70,
        71,72,73,74,75,76,77,78,79,80,
        81,82,83,84,85,86,87,88,89,90,
        91,92,93,94,95,96,97,98,99,100
    };

    int b_large[] = {
        1,2,3,
        4,5,6,
        7,8,9
    };

    int e_large[] = {
        354, 399, 444, 489, 534, 579, 624, 669,
        804, 849, 894, 939, 984, 1029, 1074, 1119,
        1254, 1299, 1344, 1389, 1434, 1479, 1524, 1569,
        1704, 1749, 1794, 1839, 1884, 1929, 1974, 2019,
        2154, 2199, 2244, 2289, 2334, 2379, 2424, 2469,
        2604, 2649, 2694, 2739, 2784, 2829, 2874, 2919,
        3054, 3099, 3144, 3189, 3234, 3279, 3324, 3369,
        3504, 3549, 3594, 3639, 3684, 3729, 3774, 3819
    };
    run_test(
        "large_10x10",
        make_matrix(10, 10, a_large),
        make_matrix(3, 3, b_large),
        make_matrix(8, 8, e_large)
    );

    /*
    =====================================================
    END TIMER
    - Measures total execution time of full test suite
    =====================================================
    */
    double end = now_sec();
    printf("Time: %f seconds\n", end - start);
    return 0;
}