#include "timer.h"
#include "qtree.h"
#include <omp.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#define DEFAULT_SIZE 20.0

int main(int argc, char **argv) {
    const float dt = 0.01;
    int numP = 200;
    int numIter = 200;
    char* outputfile = "nbody.csv";
    float err_classic = 0.0;
    float err_approx = 0.0;
    float err_trees = 0.0;
    int * err_classic_arr = malloc(numIter*sizeof(int));
    int * err_approx_arr = malloc(numIter*sizeof(int));
    int * err_trees_arr = malloc(numIter*sizeof(int));

    float distT = 1.;
    float max_width = DEFAULT_SIZE;
    float max_width_approx = DEFAULT_SIZE;
    float err_tolerance = 0.01;
    float max_tolerance = 5.0;
    int run_speed_analysis = 0;
    int run_speed_analysis_short = 1;

    if (argc > 0) {
        if (argc > 1) {
            assert(atoi(argv[1]) > 1);
            numP = atoi(argv[1]);
        }
        if (argc > 2) {
            assert(atoi(argv[2]) > 1);
            numIter = atoi(argv[2]);
        }
        assert(atoi(argv[1])*atoi(argv[2]) <  3000*200);

        if (argc > 3) {
            assert(strcmp(argv[3], "-speed_analysis_full") == 0 || strcmp(argv[3], "-speed_analysis_short") == 0);
            run_speed_analysis = 1;
            if (strcmp(argv[3], "-speed_analysis_full") == 0) {
                run_speed_analysis_short = 0;
            }
        }
    }

    Particle *p = malloc(numP*sizeof(Particle));
    Particle *approxP = malloc(numP*sizeof(Particle));
    Particle *refP = malloc(numP*sizeof(Particle));
    srand(time(0));
    // srand(0);
    // lattice_init(p, numP);
    // ran_init(p, numP, DEFAULT_SIZE);
    // twobox_init(p, numP, DEFAULT_SIZE);
    twocircle_init(p,numP,DEFAULT_SIZE);
    memmove(refP, p, numP*sizeof(Particle));
    memmove(approxP, p, numP*sizeof(Particle));

    QTNode *root = create_root(DEFAULT_SIZE);
    for(int i=0; i<numP; ++i) {
        qTree_insert(&p[i], root);
    }
    QTNode *root_approx = create_root(DEFAULT_SIZE);
    for(int i=0; i<numP; ++i) {
        qTree_insert(&approxP[i], root_approx);
    }

    FILE * hist = fopen(outputfile,"w");
    fprintf(hist, "\"iteration\",\"particle_index\",\"x\",\"y\",\"reference_x\",\"reference_y\",\"approx_x\",\"approx_y\"\n");
    for (int i=0; i<numP; ++i) {
        fprintf(hist, "%d,%d,%f,%f,%f,%f,%f,%f\n", 0, i, p[i].x, p[i].y, refP[i].x, refP[i].y, approxP[i].x, approxP[i].y);
    }

    // Error comparison
    for (int j=0; j<numIter; ++j) {
        for (int i=0;i<numP; ++i) { // calculate force and update momenta based on force and timestep
            Force * f = qTree_calcForce(&approxP[i], root_approx, distT); // Tree code approximation with 1.0 distance threshold 
            approxP[i].Fx = f->Fx; approxP[i].Fy = f->Fy;
            approxP[i].vx = dt*f->Fx; approxP[i].vy = dt*f->Fy;
            free(f);
            Force * force = qTree_calcForce(&p[i], root, .0); // Tree code approximation with 0.0 distance threshold 
            p[i].Fx = force->Fx; p[i].Fy = force->Fy;
            p[i].vx = dt*force->Fx; p[i].vy = dt*force->Fy;
            free(force);
        }

        calc_force(refP, dt, numP);  /* N^2 serial operation: compute interparticle forces and update vel */

        for (int i = 0 ; i < numP; ++i) {  /* compute new position based on new momenta */
            p[i].x += p[i].vx*dt; p[i].y += p[i].vy*dt;
            refP[i].x += refP[i].vx*dt; refP[i].y += refP[i].vy*dt;
            approxP[i].x += approxP[i].vx*dt; approxP[i].y += approxP[i].vy*dt;
            fprintf(hist, "%d,%d,%f,%f,%f,%f,%f,%f\n", j+1, i, p[i].x, p[i].y, refP[i].x, refP[i].y, approxP[i].x, approxP[i].y);
            max_width = fmax(2.0*fabsf(p[i].x), DEFAULT_SIZE); 
            max_width = fmax(2.0*fabsf(p[i].y), DEFAULT_SIZE);
            max_width_approx = fmax(2.0*fabsf(approxP[i].x), DEFAULT_SIZE); 
            max_width_approx = fmax(2.0*fabsf(approxP[i].y), DEFAULT_SIZE);
        }

        root = qTree_clear(root); // rebuild tree
        resize_root(root, max_width); // resize qtree root's domain by the longest side of the furtherest particle
        for(int i=0; i<numP; ++i) {
            qTree_insert(&p[i], root);
        }

        root_approx = qTree_clear(root_approx); // rebuild tree
        resize_root(root_approx, max_width_approx); // resize qtree root's domain by the longest side of the furtherest particle
        for(int i=0; i<numP; ++i) {
            qTree_insert(&approxP[i], root_approx);
        }

        int err_a = compare_particle(p, refP, numP, fminf((j+1.0)*err_tolerance, max_tolerance));
        int err_b = compare_particle(approxP, refP, numP, fminf((j+1.0)*err_tolerance, max_tolerance));
        int err_c = compare_particle(approxP, p, numP, fminf((j+1.0)*err_tolerance, max_tolerance));

        err_classic += (float) err_a;
        err_approx += (float) err_b;
        err_trees += (float) err_c;
        err_classic_arr[j] = err_a;
        err_approx_arr[j] = err_b;
        err_trees_arr[j] = err_c;
    }


    printf("\n---------Accuracy Measure------------\n");
    printf("Baseline: On average %f error per iteration in %d iterations of %d-body simulation.\n", err_classic/numIter,numIter, numP);
    printf("Barnes-Hut Approximation: On average %f error per iteration in %d iterations of %d-body simulation (distance threshold: %f). ", err_approx/numIter,numIter, numP, distT);
    printf("There are %f mismatched particles per iteration between Tree code with distance threshold %f and Tree code with threshold 0 (i.e. degenerating into N^2 algorithm).\n", err_trees/numIter,distT);

    FILE * error = fopen("sim_error.csv","w");
    fprintf(error, "\"total_body\",\"iteration\",\"error_baseline\",\"error_approximate\",\"error_between_tree\"\n");
    for (int i = 0;i < numIter; ++i) {
        fprintf(error, "%d,%d,%d,%d,%d\n", numP, i+1, err_classic_arr[i], err_approx_arr[i], err_trees_arr[i]);
    }
    fclose(error); fclose(hist);
    free(err_classic_arr); free(err_approx_arr); free(err_trees_arr);

    if (run_speed_analysis == 0) {
        printf("Exiting without running speed analysis.\n");
        free(p); free(approxP); free(refP); 
        qTree_clear(root); qTree_clear(root_approx);
        free(root); free(root_approx);
        exit(0);
    }

    // Speed comparison
    printf("\n---------Speed Measure------------\n");
    int max_thread = omp_get_max_threads();
    double classic_time;
    double tree_time;
    double tree_time_parallel;
    int nTest = 5;
    int pCnt_short[] = {400, 600, 1000, 1500, 2000};
    int pCnt_long[] = {1000, 2000, 4000, 8000, 10000};
    int *pCnt;
    if (run_speed_analysis_short) {
        pCnt = pCnt_short;
    } else {
        pCnt = pCnt_long;
    }
    numIter = 20;
    printf("\nStart Performance comparison between classical algorithm, " 
    "tree code with %f distance threshold, and the same tree code with %d-thread parallelism.\n", distT, max_thread);
    printf("---------------------------------------------------------\n"
        "\tSimulating %d-iteration n-body system\n", numIter);
    printf("---------------------------------------------------------\n"
        "Particle Count| \t\tRunning Time \t\t\n");
    printf("---------------------------------------------------------\n"
        "\t\t  Classical | Tree Code  | Parallel Tree \n");

    for (int k=0; k<nTest; ++k) {
        classic_time = 0.0;
        tree_time = 0.0;
        tree_time_parallel = 0.0;
        numP = pCnt[k];
        p = realloc(p, numP*sizeof(Particle));
        approxP = realloc(approxP, numP*sizeof(Particle));
        refP = realloc(refP, numP*sizeof(Particle));
        ran_init(p, numP, DEFAULT_SIZE);
        memmove(refP, p, numP*sizeof(Particle));
        memmove(approxP, p, numP*sizeof(Particle));
        qTree_clear(root); 
        qTree_clear(root_approx);
        resize_root(root, DEFAULT_SIZE);
        resize_root(root_approx, DEFAULT_SIZE);
        for(int i=0; i<numP; ++i) {
            qTree_insert(&p[i], root);
            qTree_insert(&approxP[i], root_approx);
        }

        // classical algorithm
        for (int iter=0; iter<numIter; ++iter) {
            StartTimer();
            calc_force(refP, dt, numP);        /* N^2 serial operation: compute interparticle forces and update vel */
            for (int i = 0 ; i < numP; i++) {  /* compute new position */
                refP[i].x += refP[i].vx*dt; 
                refP[i].y += refP[i].vy*dt;
            }
            const double tElapsed = GetTimer() / 1000.0;
            if (iter > 1) {                          /* First iter is warm up */
            classic_time += tElapsed; 
            }
        }

        // Tree code
        for (int iter=0; iter<numIter; ++iter) {
            StartTimer();
            for (int i=0;i<numP; ++i) {  
                Force * f = qTree_calcForce(&approxP[i], root_approx, distT);
                approxP[i].vx = dt*f->Fx; approxP[i].vy = dt*f->Fy;
                free(f);
            }
            // max_width = 20.0; // default domain size for the root node is 2.0
            for (int i = 0 ; i < numP; ++i) {  /* compute new position based on new momenta */
                approxP[i].x += approxP[i].vx*dt; 
                approxP[i].y += approxP[i].vy*dt;
                max_width_approx = fmax(2.0*fabsf(approxP[i].x), DEFAULT_SIZE); 
                max_width_approx = fmax(2.0*fabsf(approxP[i].y), DEFAULT_SIZE);
            }

            root_approx = qTree_clear(root_approx); // rebuild tree
            resize_root(root_approx, max_width_approx); // resize qtree root's domain by the longest side of the furtherest particle
            for(int i=0; i<numP; ++i) {
                qTree_insert(&approxP[i], root_approx);
            }

            const double tElapsed = GetTimer() / 1000.0;
            if (iter > 1) {                          /* First iter is warm up */
                tree_time += tElapsed; 
            }
        }

        // Tree code with parallelism
        for (int iter=0; iter<numIter; ++iter) {
            StartTimer();
            #pragma omp parallel for schedule(static) num_threads(max_thread) 
            for (int i=0;i<numP; ++i) {  
                Force * f = qTree_calcForce(&p[i], root, distT);
                p[i].vx = dt*f->Fx; p[i].vy = dt*f->Fy;
                free(f);
            }
            // max_width = 20.0; // default domain size for the root node is 2.0
            for (int i = 0 ; i < numP; ++i) {  /* compute new position based on new momenta */
                p[i].x += p[i].vx*dt; 
                p[i].y += p[i].vy*dt;
                max_width = fmax(2.0*fabsf(p[i].x), DEFAULT_SIZE); 
                max_width = fmax(2.0*fabsf(p[i].y), DEFAULT_SIZE);
            }

            root = qTree_clear(root); // rebuild tree
            resize_root(root, max_width); // resize qtree root's domain by the longest side of the furtherest particle
            for(int i=0; i<numP; ++i) {
                qTree_insert(&p[i], root);
            }

            const double tElapsed = GetTimer() / 1000.0;
            if (iter > 1) {                          /* First iter is warm up */
                tree_time_parallel += tElapsed; 
            }
        }
        printf("---------------------------------------------------------\n"
        "\t%5.d | %10.6f  | %10.6f |  %10.6f  \n", numP, classic_time, tree_time, tree_time_parallel);
    }
    printf("-------------------------End of test---------------------\n");

    free(p); free(approxP); free(refP); 
    qTree_clear(root); qTree_clear(root_approx);
    free(root); free(root_approx);
}