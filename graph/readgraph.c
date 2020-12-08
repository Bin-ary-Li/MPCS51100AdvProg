#include "readgraph.h"

static Graph* initialize_graph(int nedges, int nvertices) {
    Graph *g = malloc(sizeof(Graph));
    g->nvertices = nvertices;
    g->nedges = nedges;
    g->nedges_stored = 0;
    g->outdegrees = calloc(nvertices, sizeof(int));
    g->indegrees = calloc(nvertices, sizeof(int));
    g->edges = malloc(nvertices*sizeof(EdgeNode*));
    for (int i = 0; i < nvertices; ++i) g->edges[i] = NULL;
    return g;
}

static void insert_edge(Graph* g, int from, int to, int weight) {
    EdgeNode* e = malloc(sizeof(EdgeNode));
    e->from = from; e->to = to; e->weight = weight;
    e->next = g->edges[from];
    g->edges[from] = e;
    ++(g->nedges_stored);
    ++(g->outdegrees[from]);
    ++(g->indegrees[from]);
}

void graph_destroy(Graph *g) {
    for(int i = 0; i < g->nvertices; ++i) {
        EdgeNode* e = g->edges[i];
        while (e != NULL) {
            EdgeNode* temp = e;
            e = e->next;
            free(temp);
        }
    }
    free(g->edges);
    free(g->indegrees);
    free(g->outdegrees);
    free(g);
}

Graph* readgraph(char *filename) { 
  char*  string = NULL; 
  size_t strsize = 0;   
  ssize_t nread; 
  int linecnt = 0;  
  Graph* g = NULL;
  FILE *file = fopen(filename, "r");
  while ((nread = getline(&string, &strsize, file)) != -1) {
        // ignore comment line
        if(string[0] == '#') continue;

        // assume first line contains graph information: nvertices and nedges
        if (linecnt == 0) { 
            char* token = strtok(string, " ");
            int nvertices = atoi(token);
            token = strtok(NULL, " ");
            int nedges = atoi(token);
            g = initialize_graph(nedges, nvertices);
            assert(strtok(NULL, " ") == NULL);
            token = strtok(NULL, " ");
            ++linecnt;
            continue;
        }
        
        // assume all other lines store edge information
        if (linecnt > 0) {
            char* token = strtok(string, " ");
            int from = atoi(token);
            token = strtok(NULL, " ");
            int to = atoi(token);
            token = strtok(NULL, " ");
            int weight = atoi(token);
            assert(g != NULL);
            insert_edge(g, from, to, weight);
            assert(strtok(NULL, " ") == NULL);
            ++linecnt;
        }
  }
  free(string);
  fclose(file);
  return g;
}

// int main() {
//     Graph* g = readgraph("graph1.txt");
//     printf("edge: from %d to %d with weight %d\n", g->edges[0]->from,g->edges[0]->to,g->edges[0]->weight);
// }
