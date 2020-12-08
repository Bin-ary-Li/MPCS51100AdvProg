#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


typedef struct item_ {
    int priority;
    void* data;
} Item;

typedef struct pq_ {
    int size;
    int maxsize;
    Item **items; 
} PriorityQueue;

PriorityQueue * pq_init(int init_size);
void pq_insert(PriorityQueue *pq, void *data, int priority);
void* pq_pop(PriorityQueue *pq);
void pq_print(PriorityQueue *pq);
PriorityQueue * build_heap(void **data_array, int *priority_array, int size);
void pq_destroy(PriorityQueue * pq);