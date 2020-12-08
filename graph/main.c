#include "readgraph.h"
#include "priorityqueue.h"
#include "timer.h"
#include <limits.h>
#include <omp.h>

typedef struct path_ {
    int from;
    int to;
    int distance;
} Path;

// return the longest paths from origin
Path** dijkstra(Graph* g, int origin, int getNum) {
    int nprocessed = 0;
    int* processed = malloc(g->nvertices*sizeof(int));
    int* distance = malloc(g->nvertices*sizeof(int));
    // int* via = malloc(g->nvertices*sizeof(int));
    PriorityQueue* pq = pq_init(g->nedges);
    int current = 0;
    Path** returnPaths = malloc(sizeof(Path*)*getNum);
    for (int i = 0; i < getNum; ++i) returnPaths[i] = NULL;

    for (int i = 0; i < g->nvertices; ++i) {
        processed[i] = 0;
        distance[i] = INT_MAX;
        // via[i] = -1;
    }

    // process origin vertex 
    distance[origin] = 0;
    processed[origin] = 1;
    // via[origin] = origin;
    ++(nprocessed);
    EdgeNode* e = g->edges[origin];
    while(e != NULL) { // enqueue all the incident edge
        if (distance[e->to] > (distance[e->from] + e->weight)) {
            distance[e->to] = (distance[e->from] + e->weight);
            // via[e->to] = e->from;
            pq_insert(pq, &(e->to), distance[e->to]);
        }
        e = e->next;
    }

    // while don't have path to all vertices or pq empty (i.e., from origin can't reach everywhere)
    while(nprocessed != g->nvertices && pq->size != 0) { 
        int from = *(int*) pq_pop(pq);
        if (processed[from] == 1) continue;
        processed[from] = 1;
        ++(nprocessed);

        EdgeNode* e = g->edges[from];
        while(e != NULL) { // enqueue all the incident edges
            assert(&(e->to) != NULL);
            if (distance[e->to] > (distance[e->from] + e->weight)) {
                distance[e->to] = (distance[e->from] + e->weight);
                // via[e->to] = e->from;
                pq_insert(pq, &(e->to), distance[e->to]);
            }
            e = e->next;
        }

        // the last item has the longest path
        Path* p = malloc(sizeof(Path));
        p->from = origin; p->to = from; p->distance = distance[from];
        if (returnPaths[current % getNum] != NULL) free(returnPaths[current % getNum]);
        returnPaths[current % getNum] = p;
        ++(current);
    }

    pq_destroy(pq);
    free(distance);
    free(processed);
    // free(via);

    return returnPaths;
}

Path** dijkstra_slowsearch(Graph* g, int origin, int getNum) {
    int nprocessed = 0;
    int current = 0;
    int* processed = malloc(g->nvertices*sizeof(int));
    int* distance = malloc(g->nvertices*sizeof(int));
    Path** returnPaths = malloc(sizeof(Path*)*getNum);

    for (int i = 0; i < getNum; ++i) returnPaths[i] = NULL;
    for (int i = 0; i < g->nvertices; ++i) {
        processed[i] = 0;
        distance[i] = INT_MAX;
    }

    // process origin vertex 
    distance[origin] = 0;
    processed[origin] = 1;
    ++(nprocessed);
    EdgeNode* e = g->edges[origin];
    while(e != NULL) { // enqueue all the incident edge
        if (distance[e->to] > (distance[e->from] + e->weight)) {
            distance[e->to] = (distance[e->from] + e->weight);
        }
        e = e->next;
    }

    // while don't have path to all vertices or pq empty (i.e., from origin can't reach everywhere)
    while(nprocessed != g->nvertices) { 
        int from, dist;
        int no_more = 0;
        /* find new min: naive approach, replace with min-heap priority queue */
        for (int i=0, dist=INT_MAX; i < g->nvertices; ++i) {
            if ((processed[i] == 0) && (dist > distance[i])) {
                dist = distance[i];
                from = i;
            }

            // if after the search the next closest vertice is unreachable, break loop
            if (i == g->nvertices - 1 && dist == INT_MAX) {
                no_more = 1;
            }
        }

        if (no_more == 1) break;
        if (processed[from] == 1) continue;
        processed[from] = 1;
        ++(nprocessed);

        EdgeNode* e = g->edges[from];
        while(e != NULL) { // enqueue all the incident edges
            assert(&(e->to) != NULL);
            if (distance[e->to] > (distance[e->from] + e->weight)) {
                distance[e->to] = (distance[e->from] + e->weight);
            }
            e = e->next;
        }

        // the last item has the longest path
        Path* p = malloc(sizeof(Path));
        p->from = origin; p->to = from; p->distance = distance[from];
        if (returnPaths[current % getNum] != NULL) free(returnPaths[current % getNum]);
        returnPaths[current % getNum] = p;
        ++(current);
    }

    free(distance);
    free(processed);

    return returnPaths;
}

int main(int argc, char** argv) {
    char* filename = "graph1.txt";
    Graph* g = readgraph(filename);
    int threads = omp_get_max_threads();

    if (argc == 3) {
        assert(atoi(argv[2]) > 0);
        filename = argv[1];
        threads =atoi(argv[2]);
    }

    int num_v = g->nvertices;
    // int num_v = 1000;
    int topn = 10;
    int arraysize = num_v * topn;
    Path** allp = malloc(sizeof(Path*)*arraysize);
    int* distance = malloc(sizeof(int)*arraysize);


    printf("----------------------------------\n");
    printf("The top %d longest paths: \n", topn);
    printf("----------------------------------\n");

    StartTimer();
    #pragma omp parallel for num_threads(threads) schedule(dynamic)
    for (int i = 0; i < arraysize; i+=topn) {
        // Path** p = dijkstra_slowsearch(g, i/topn, topn);
        Path** p = dijkstra(g, i/topn, topn);
        for (int j = 0; j < topn; ++j) {
            allp[i+j] = p[j];
        }
        // if (i/topn % 1000 == 0) {
        //     double time = GetTimer() / 1000.0;
        //     printf("--- Processing %d vertice, projected time to complete: %d\n", i / topn, );
        // }
        free(p);
    }

    for (int i = 0; i < arraysize; ++i) {
        if (allp[i] == NULL) {
            distance[i] = INT_MAX;
        } else {
            distance[i] = -(allp[i]->distance);
        }
    }

    PriorityQueue* allpath_pq = build_heap((void**)allp, distance, arraysize);

    for (int i = 0; i < topn; ++i) {
        Path* p = (Path*) pq_pop(allpath_pq);
        printf("path from %d to %d with distance %d\n", p->from, p->to, p->distance);
    }
    double running_time = GetTimer() / 1000.0;

    printf("----------------------------------\n");
    printf("Running time: %f s (~%d min)\n", running_time, (int) running_time/60);
    printf("Thread counts: %d\n", threads);
    printf("----------------------------------\n");

    pq_destroy(allpath_pq);
    for (int i = 0; i < num_v*topn; ++i) if(allp[i] != NULL) free(allp[i]);
    free(allp);
    free(distance);
    graph_destroy(g);
}