#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct edgenode_ {
    int from;
    int to;
    int weight;
    struct edgenode_ *next;
} EdgeNode;

typedef struct graph_ {
    EdgeNode **edges;
    int  *outdegrees;
    int  *indegrees; 
    int  nvertices;
    int  nedges;
    int  nedges_stored;
} Graph;

Graph* readgraph(char *filename);
void graph_destroy(Graph *g);