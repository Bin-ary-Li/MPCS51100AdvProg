#include "modifiedarray.h"

static int fequal(double a, double b, double epsi) {
    return fabs(a-b) < epsi;
}

// return the min value and fill in a list of indices that have the min value, ignore the indices that marked as finished
static double findmin(double * arr, int arrsize, int** indices, int* finished_array) {
    assert(*indices == NULL);
    *indices = calloc(arrsize, sizeof(int));
    double min = DBL_MAX;
    for (int i = 0; i<arrsize; ++i) {
        if (finished_array[i] == 1) {
            continue;
        }
        if (arr[i] == min) {
            (*indices)[i] = 1;
            continue;
        } else if (arr[i] > min) {
            continue;
        } else {
            free(*indices);
            *indices = calloc(arrsize, sizeof(int));
            (*indices)[i] = 1;
            min = arr[i];
        }
    }
    return min;
}

// get values by their indices in a 2d array
static void getValues(int* indices, double** array, double* vals, int size) {
    for (int i=0; i<size; ++i) {
        vals[i] = array[i][indices[i]];
    }
}

// get the sum of the values in a integer array
static int arraySum(int* array, int size) {
    int ret = 0;
    for (int i=0; i<size;++i) {
        ret += array[i];
    }
    return ret;
}

// Increment the selected indices by one if have not reached the cap, return a new array of indices
static int* updateIndices(int* indToIncre, int* indices, int* cap, int size, int* finished_array) {
    int* newInd = malloc(sizeof(int)*size);
    for (int i = 0; i<size;++i) {
        if (indToIncre[i] == 1) {
            if (indices[i] < cap[i]-1) {
                newInd[i] = indices[i]+1;
            } else {
                finished_array[i] = 1;
                newInd[i] = indices[i];
            }
        } else {
            newInd[i] = indices[i];
        }
    }
    return newInd;
}

// binary search an double array, return the index of the closest value
int binarysearch(double* arr, double key, int size) {
    if (arr == NULL) return -1;
    int start = 0;
    int end = size - 1;
    int ret = -1;
    while (start <= end) {
        int mid = (start + end) / 2;
        if (arr[mid] == key) {
            return mid;
        } else if (arr[mid] < key) {
            if (size - 1 < mid + 1) {
                ret = size - 1;
            } else {
                ret = mid + 1;
            } 
            start = mid + 1;
        } else {
            if (0 > mid - 1) {
                ret = 0;
            } else {
                ret = mid - 1;
            } 
            end = mid - 1;
        }
    }
    return ret;
}

UnionArray* unionize(array2D* original) {
    int num_val = 0; int num_array = original->outersize;
    int maxnum_val = arraySum(original->innersize, num_array);
    UnionArray* u = malloc(sizeof(UnionArray));
    u->num_arr = num_array;
    List* list_of_array = list_create(NULL, NULL, NULL, 0);
    int* minInd = NULL;
    double* val_union = calloc(maxnum_val, sizeof(double));
    int* indices = calloc(num_array, sizeof(int));
    int* finished_array = calloc(num_array, sizeof(int));
    double* vals = calloc(num_array, sizeof(double));

    for (int i=0; i<maxnum_val; ++i) {
        list_append(list_of_array, indices);
        ++num_val;
        getValues(indices, original->array, vals, num_array);
        double min = findmin(vals, num_array, &minInd, finished_array);
        if (min == DBL_MAX) {
            free(minInd);
            break;
        }
        val_union[i] = min;
        indices = updateIndices(minInd, indices, original->innersize, num_array, finished_array); // indices are updated
        free(minInd);
        minInd = NULL;
        if (i == maxnum_val - 1) free(indices); // the last one is meaningless
    }

    u->valsize = num_val;
    val_union = realloc(val_union, num_val*sizeof(double));
    u->val = val_union;

    u->unionarray = malloc(sizeof(int*)*num_val);
    for(int i=0; i<num_val;++i) {
        u->unionarray[i] = (int *) list_remove(list_of_array, 0);
    }

    list_destroy(&list_of_array);
    free(vals);
    free(finished_array);
    return u;
}

static AtomicArray* skipSample(double* arr, int size, int skip) {
    int newsize = (int) size/skip;
    double* newArr = malloc(sizeof(double)*newsize);
    int* ogInd = malloc(sizeof(int)*newsize);
    AtomicArray* ret = malloc(sizeof(AtomicArray));
    for (int i = 0; i<newsize; ++i) {
        newArr[i] = arr[1+i*skip];
        ogInd[i] = 1+i*skip;
    }
    ret->originalIndex = ogInd;
    ret->vals = newArr;
    ret->size = newsize;
    return ret;
}

static AtomicArray* getSingleAugArr(double* og, int ogsize, double* aug, int augsize) {
    AtomicArray* ret = malloc(sizeof(AtomicArray));
    if (aug == NULL) { // if no aug, return og form
        ret->vals = malloc(sizeof(double)*ogsize);
        ret->originalIndex = malloc(sizeof(int)*ogsize);
        ret->augmentIndex = malloc(sizeof(int)*ogsize);
        memmove(ret->vals, og, sizeof(double)*ogsize);
        for (int i=0;i<ogsize;++i) {
            ret->originalIndex[i] = i;
            ret->augmentIndex[i] = 0;
        }
        ret->size = ogsize;
        return ret;
    }
    AtomicArray* skipAugment = skipSample(aug, augsize, 2);
    double* skipAug = skipAugment->vals;
    int skipSize = skipAugment->size;
    int* skipOgInd = skipAugment->originalIndex;
    
    int newsize = ogsize + skipAugment->size;
    int ogcurr = 0;
    int augcurr = 0;
    double* vals = malloc(sizeof(double)*newsize);
    int* ogInd = malloc(sizeof(int)*newsize);
    int* augInd = malloc(sizeof(int)*newsize);
    int ogi, augi; ogi = augi = 0;
    double val = 0;
    for (int i=0; i<newsize; ++i) {
        if (ogcurr >= ogsize) { //reach the end of original array
            val = skipAug[augcurr];
            augi = skipOgInd[augcurr];
            ogi = ogsize - 1; // index limited to the last(biggest) element of the array
            ++augcurr;
        } else if (augcurr >= skipAugment->size) { // reach the end of the augmented array
            val = og[ogcurr];
            augi = augsize - 1; // limit to the last(biggest) element of the array
            ogi = ogcurr;
            ++ogcurr; 
        } else if (og[ogcurr] <= skipAug[augcurr]) { // if og value smaller than augment value
            val = og[ogcurr];
            augi = skipOgInd[augcurr];
            ogi = ogcurr;
            ++ogcurr; 
        } else {
            val = skipAug[augcurr];
            augi = skipOgInd[augcurr];
            ogi = ogcurr;
            ++augcurr;
        }
        vals[i] = val;
        ogInd[i] = ogi;
        augInd[i] = augi;
    }
    ret->augmentIndex = augInd;
    ret->originalIndex = ogInd;
    ret->vals = vals;
    ret->size = newsize;

    free(skipAugment->originalIndex);
    free(skipAugment->vals);
    free(skipAugment);
    return ret;
}

AugmentArray* augment(array2D* original) {
    int num_array = original->outersize;
    AugmentArray* ret = malloc(sizeof(AugmentArray));
    ret->outersize = original->outersize;
    ret->list_of_array = malloc(sizeof(AtomicArray *)*num_array);
    ret->innersize = malloc(sizeof(int)*num_array);
    AtomicArray* last = getSingleAugArr(original->array[num_array -1], original->innersize[num_array-1], NULL, 0);
    ret->list_of_array[num_array-1] = last; // final arrays are the same
    ret->innersize[num_array-1] = original->innersize[num_array-1];

    for (int i = num_array-2; i > -1; --i) {
        double * og = original->array[i];
        int ogsize = original->innersize[i];
        AtomicArray* aug = ret->list_of_array[i+1];
        double* augment = aug->vals;
        int augsize = aug->size;
        AtomicArray* newaug = getSingleAugArr(og, ogsize, augment, augsize);
        ret->list_of_array[i] = newaug;
        ret->innersize[i] = newaug->size;
    }

    return ret;
}

void getInd_aug(AugmentArray* a, double key, int** indices) {
    int currArrInd = 0;
    AtomicArray* arr = a->list_of_array[currArrInd];
    int index = binarysearch(arr->vals, key, arr->size);
    (*indices)[currArrInd] = arr->originalIndex[index];
    int nextInd = arr->augmentIndex[index];

    for (currArrInd=1;currArrInd < a->outersize;++currArrInd) {
        arr = a->list_of_array[currArrInd]; //next array
        double val = arr->vals[nextInd];
        double prev;
        if (nextInd == 0) {
            prev = val;
        } else {
            prev = arr->vals[nextInd-1];
        }

        if (key > prev) { 
            (*indices)[currArrInd] = arr->originalIndex[nextInd];
            nextInd = arr->augmentIndex[nextInd];
        } else { // choosing previous value
            (*indices)[currArrInd] = arr->originalIndex[nextInd-1];
            nextInd = arr->augmentIndex[nextInd-1];
        }
    }

}

void getInd_union(UnionArray* u, double key, int** indices) {
    int pos = binarysearch(u->val, key, u->valsize);
    for(int i=0; i<u->num_arr; ++i) (*indices)[i] = u->unionarray[pos][i];
}

void getInd_bs(array2D* array, double key, int** indices) {
    for (int i=0; i< array->outersize; ++i) {
        (*indices)[i] = binarysearch(array->array[i], key, array->innersize[i]);
    }
}

void destroy_union(UnionArray* u) {
    free(u->val);
    for (int i=0; i< u->valsize; i++) free(u->unionarray[i]);
    free(u->unionarray);
    free(u);
}

void destroy_atomic(AtomicArray* arr) {
    free(arr->augmentIndex);
    free(arr->originalIndex);
    free(arr->vals);
    free(arr);
}

void destroy_aug(AugmentArray* a) {
    free(a->innersize);
    for (int i=0;i<a->outersize;++i) destroy_atomic(a->list_of_array[i]);
    free(a->list_of_array);
    free(a);
}

array2D* buildExample() {
    double* a1 = malloc(sizeof(double)*5);
    double* a2 = malloc(sizeof(double)*3);
    double* a3 = malloc(sizeof(double)*4);
    double* a4 = malloc(sizeof(double)*5);
    a1[0]=1.3;a1[1]=2.2;a1[2]=4.1;a1[3]=7.3;a1[4]=8.8;
    a2[0]=4.8;a2[1]=5.1;a2[2]=7.4;
    a3[0]=2.6;a3[1]=2.8;a3[2]=4.7;a3[3]=8.2;
    a4[0]=1.1;a4[1]=5.2;a4[2]=6.1;a4[3]=6.7;a4[4]=9.2;
    int outersize = 4;
    int* innersize = malloc(sizeof(double)*4);
    innersize[0] = 5;innersize[1] = 3;innersize[2] = 4;innersize[3] = 5;
    double** examplearr = malloc(sizeof(double*)*4);
    examplearr[0] = a1;examplearr[1] = a2;examplearr[2] = a3;examplearr[3] = a4;
    array2D* example = malloc(sizeof(array2D));
    example->array = examplearr;
    example->innersize = innersize;
    example->outersize = outersize;
    return example;
}

void test(UnionArray* u, AugmentArray* a, array2D* original, double query) {
    int* index = malloc(sizeof(int)*original->outersize);
    getInd_union(u, query, &index);
    printf("\n ------ Unionized array -------- \n");
    printf("- Indices of the query value in the corresponding arrays \n");
    for (int j = 0;j<original->outersize;++j) printf("%d ", index[j]);
    printf("\n- Values stored at those indices of corresponding arrays \n");
    for (int j = 0;j<original->outersize;++j) printf("%g ", original->array[j][index[j]]);
    printf("\n");

    getInd_aug(a, query, &index);
    printf("\n ------ Augmented arrays -------- \n");
    printf("- Indices of the query value in the corresponding arrays \n");
    for (int j = 0;j<original->outersize;++j) printf("%d ", index[j]);
    printf("\n- Values stored at those indices of corresponding arrays \n");
    for (int j = 0;j<original->outersize;++j) printf("%g ", original->array[j][index[j]]);
    printf("\n");

    getInd_bs(original, query, &index);
    printf("\n ------ Binary search -------- \n");
    printf("- Indices of the query value in the corresponding arrays \n");
    for (int j = 0;j<original->outersize;++j) printf("%d ", index[j]);
    printf("\n- Values stored at those indices of corresponding arrays \n");
    for (int j = 0;j<original->outersize;++j) printf("%g ", original->array[j][index[j]]);
    printf("\n");

    free(index);
}

// int main(int argc, char** argv) {
//     char   *filename = "arrays.txt";
//     array2D* original = read(filename);
//     UnionArray* u = unionize(original);
//     AugmentArray* a = augment(original);
//     test(u, a, original, 5.0);

//     // array2D* original = buildExample();
//     // UnionArray* u = unionize(original);
//     // AugmentArray* a = augment(original);
//     // AtomicArray* arr = a->list_of_array[0];
//     // test(u, a, original, 2.3);

//     // double arr[] = {1.1,1.3,2.2,2.6,2.8,4.1,4.6,4.8};
//     // printf("bs: %d\n", binarysearch(arr, 4.7,8));

//     destroy_union(u);
//     destroy_aug(a);
//     destroy_arr(&original);
// }