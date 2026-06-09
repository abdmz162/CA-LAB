#ifndef TESTS_H
#define TESTS_H

#include "../src/io.h"
#include <stdlib.h>
#include <stdio.h>

matrix_t* make_matrix(int rows,int cols,int data[])
{
    matrix_t *m=malloc(sizeof(matrix_t));

    m->rows=rows;
    m->cols=cols;

    m->data=malloc(
        rows*cols*sizeof(int32_t)
    );

    for(int i=0;i<rows*cols;i++)
        m->data[i]=data[i];

    return m;
}

void free_matrix(matrix_t *m)
{
    free(m->data);
    free(m);
}

int equal(matrix_t *a,matrix_t *b)
{
    if(a->rows!=b->rows)
        return 0;

    if(a->cols!=b->cols)
        return 0;

    int n=a->rows*a->cols;

    for(int i=0;i<n;i++)
    {
        if(a->data[i]!=b->data[i])
            return 0;
    }

    return 1;
}

#endif