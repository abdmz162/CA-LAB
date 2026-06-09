#include "../src/compute.h"
#include "tests.h"


double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void run_test(
    char *name,
    matrix_t *A,
    matrix_t *B,
    matrix_t *Expected
)
{
    matrix_t *out=NULL;

    int result=
        convolve(A,B,&out);

    if(result!=0)
    {
        printf("%s FAILED\n",name);
        return;
    }

    if(equal(out,Expected))
        printf("%s PASSED\n",name);
    else
        printf("%s FAILED\n",name);

    free_matrix(A);
    free_matrix(B);
    free_matrix(Expected);
    free_matrix(out);
}


int main()
{
    double start = now_sec();
    // Test 1

    int a1[]={
        1,2,3,
        4,5,6,
        7,8,9
    };

    int b1[]={
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


    // Test 2

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


    // Test 3

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

    // same-size kernel

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


    // rectangular
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

    // 5x5 with 3x3 kernel

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


    // negatives

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

    double end = now_sec();
    printf("Time: %f seconds\n", end - start);
    return 0;
}