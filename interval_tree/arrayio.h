#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct array2d_ {
    double ** array;
    int outersize;
    int* innersize;
} array2D;

#define NUM_ARRAY 68

array2D* read(char *filename);
void destroy_arr(array2D** array);