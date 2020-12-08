#include "arrayio.h"
#include "list.h"
#include <math.h>
#include <float.h>

#define EPSILON 1e-40

typedef struct unionarr_ {
    double* val;
    int valsize;
    int num_arr;
    int** unionarray;
} UnionArray;

typedef struct atomicarr_ {
    double* vals;
    int size;
    int* originalIndex;
    int* augmentIndex;
} AtomicArray;

typedef struct augarr_ {
    int* innersize;
    int outersize;
    AtomicArray** list_of_array;
} AugmentArray;

// binary search an double array, return the index of the closest value
int binarysearch(double* arr, double key, int size);

UnionArray* unionize(array2D* original); 

AugmentArray* augment(array2D* original);

void getInd_aug(AugmentArray* a, double key, int** indices);

void getInd_union(UnionArray* u, double key, int** indices);

void getInd_bs(array2D* array, double key, int** indices);

void destroy_union(UnionArray* u);

void destroy_atomic(AtomicArray* arr);

void destroy_aug(AugmentArray* a);

array2D* buildExample();

void test(UnionArray* u, AugmentArray* a, array2D* original, double query);