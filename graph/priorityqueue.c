#include "priorityqueue.h"

PriorityQueue * pq_init(int init_size) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    pq->size = 0;
    pq->maxsize = init_size + 1; // should be actual maxsize + 1 to accommendate empty 0 index
    pq->items = malloc(pq->maxsize*sizeof(Item*));
    pq->items[0] = NULL;
    return pq;
}

static void resize(PriorityQueue *pq) {
    if (pq->size == pq->maxsize - 1) {
        pq->maxsize = (pq->size*2)+1;
        Item** newitems = malloc(pq->maxsize*sizeof(Item*));
        newitems[0] = NULL;
        for (int i = 1; i <= pq->size; ++i) {
            newitems[i] = pq->items[i];
        }
        free(pq->items);
        pq->items = newitems;
    }
}

static void percolate_up(PriorityQueue *pq) {
    int i = pq->size;
    while (i / 2 > 0) {
        if (pq->items[i]->priority < pq->items[i / 2]->priority) {
            Item* temp = pq->items[i / 2];
            pq->items[i / 2] = pq->items[i];
            pq->items[i] = temp;
        }
        i = i / 2;
    }
}

void pq_insert(PriorityQueue *pq, void *data, int priority) {
    assert(pq != NULL && priority > 0);
    assert(data != NULL);
    Item* item = malloc(sizeof(Item));
    item->data = data;
    item->priority = priority;
    pq->items[(pq->size)+1] = item;

    ++(pq->size);
    percolate_up(pq);
    resize(pq);
}

static int min_child(PriorityQueue *pq, int i) {
    assert(pq != NULL && i <= (pq->size)/2 );
    if (i * 2 + 1 > pq->size) {
        return i * 2;
    }

    if (pq->items[i * 2]->priority < pq->items[i * 2 + 1]->priority) {
        return i * 2;
    }

    return i * 2 + 1;
}

static void percolate_down(PriorityQueue *pq, int i) {
    while (i * 2 <= pq->size) {
        int mc = min_child(pq, i);
        if (pq->items[i]->priority > pq->items[mc]->priority) {
            Item* temp = pq->items[i];
            pq->items[i] = pq->items[mc];
            pq->items[mc] = temp;
        }
        i = mc;
    }
}

void* pq_pop(PriorityQueue *pq) {
    Item* return_item = pq->items[1];
    assert(pq->items[pq->size] != NULL);
    void* return_value = return_item->data;
    pq->items[1] = pq->items[pq->size];
    pq->items[pq->size] = NULL;
    --(pq->size);
    percolate_down(pq, 1);
    free(return_item);
    return return_value;
}

void pq_print(PriorityQueue *pq) {
    printf("(");
    for (int i = 1; i <= pq->size; ++i) {
        printf("%d ", pq->items[i]->priority);
    }
    printf(")\n");
}

PriorityQueue * build_heap(void **data_array, int *priority_array, int size) {
    PriorityQueue* pq = pq_init(size);
    for(int i = 0; i < size; ++i) {
        Item* item = malloc(sizeof(Item));
        item->data = data_array[i];
        item->priority = priority_array[i];
        pq->items[i+1] = item;
    }
    pq->size = size;


    int i = pq->size / 2;
    while (i > 0) {
        percolate_down(pq, i);
        i = i - 1;
    }
    return pq;
}

void pq_destroy(PriorityQueue * pq) {
    assert(pq != NULL);
    for (int i = 1; i <= pq->size; ++i) {
        free(pq->items[i]);
    }
    free(pq->items);
    free(pq);
}


// int main() {
//     PriorityQueue *pq = pq_init(5);
//     int i = -1;
//     pq_insert(pq, &i, 1);
//     pq_insert(pq, NULL, 4);
//     pq_insert(pq, NULL, 7);
//     pq_insert(pq, &i, 1);
//     printf("before resize: max=%d\n", pq->maxsize);
//     pq_insert(pq, NULL, 7);
//     pq_insert(pq, NULL, 100);
//     pq_insert(pq, NULL, 100);
//     printf("after resize: max=%d\n", pq->maxsize);
//     pq_insert(pq, NULL, 3000);
//     printf("num of item: %d, ", pq->size);
//     pq_print(pq);
//     printf("pop: %d\n" , * (int*)pq_pop(pq));
//     printf("num of item: %d, ", pq->size);
//     pq_print(pq);
//     pq_pop(pq);
//     printf("num of item: %d, ", pq->size);
//     pq_print(pq);
//     pq_pop(pq);
//     printf("num of item: %d, ", pq->size);
//     pq_print(pq);
//     pq_destroy(pq);
// }
