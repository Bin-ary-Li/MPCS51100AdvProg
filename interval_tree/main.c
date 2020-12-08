#include "timer.h"
#include "modifiedarray.h"

void ran_init(double *data, int n, double max, double min) {
  for (int i = 0; i < n; i++) {
    data[i] = 2.0 * (max-min) * (rand() / (float)RAND_MAX) - (max-min);
  }
}

int main(int argc, char** argv) {
    double query = 50.4;
    double buildtime_union, buildtime_aug;
    char   *filename = "arrays.txt";
    array2D* original = read(filename);

    StartTimer();
    UnionArray* u = unionize(original);
    buildtime_union = GetTimer() / 1000.0; 

    StartTimer();
    AugmentArray* a = augment(original);
    buildtime_aug = GetTimer() / 1000.0;

    if (argc == 2) {
        assert(atof(argv[1]) != INFINITY && atoi(argv[1]) != NAN);
        query = atof(argv[1]);
    }
    printf("Verifying that the three methods are correctly implemented...\n");
    printf("\nFinding the value close to %g in many different arrays...\n", query);
    test(u, a, original, query);
    printf("\n ------- end of query --------\n");
    printf("\n ------- Please visually verify that the method outputs are consistent --------\n");

    printf("\nStart performance comparison between the serial binary search (BS) on each array, " 
    "search with the unionized array,\nand with the augmented arrays.\n");
    printf("---------------------------------------------------------\n"
        "\tQuerying values in a large dataset\n");
    printf("---------------------------------------------------------\n"
        " Num of Queries  | \t Total Running Time (sec) \t\t\n");
    printf("---------------------------------------------------------\n"
        "\t\t       BS      |  Unionized |   Augmented \n");
    int* index = malloc(sizeof(int)*original->outersize);
    double bs_time, union_time, aug_time;
    int n_test = 4;
    int n_query[] = {10000, 100000, 1000000, 2500000};
    // int n_query[] = {100, 1000, 10000, 20000};
    double* q_arr;
    
    for (int test = 0; test<n_test ; ++test) {
        bs_time = union_time = aug_time = 0.0;
        q_arr = malloc(sizeof(double)*n_query[test]);
        ran_init(q_arr, n_query[test], 150.0, 0.0);

        for(int i=0;i<n_query[test];++i) {
            StartTimer();
            getInd_bs(original, q_arr[i], &index);
            const double tElapsed = GetTimer() / 1000.0;
            bs_time += tElapsed; 
        }
        for(int i=0;i<n_query[test];++i) {
            StartTimer();
            getInd_union(u, q_arr[i], &index);
            const double tElapsed = GetTimer() / 1000.0;
            union_time += tElapsed; 
        }
        for(int i=0;i<n_query[test];++i) {
            StartTimer();
            getInd_aug(a, q_arr[i], &index);
            const double tElapsed = GetTimer() / 1000.0;
            aug_time += tElapsed; 
        }

        printf("---------------------------------------------------------\n"
            "\t%8.d | %10.6f  | %10.6f |  %10.6f  \n", n_query[test], bs_time, union_time, aug_time);
        free(q_arr);
    }
    printf("\n-------------------------Build Time----------------------\n");
    printf("\n  Unionized array: %8.6f, Augmented arrays: %8.6f   \n", buildtime_union, buildtime_aug);

    printf("\n-------------------------End of test---------------------\n");
    free(index);



    destroy_union(u);
    destroy_aug(a);
    destroy_arr(&original);
}
