#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct List_element{
    void * data;
    struct List_element *next;
    struct List_element *prev;
} List_element;


typedef struct List{
    List_element *head;
    List_element *tail;
    int size;
    size_t datsiz;
} List;


List * list_create(size_t valsiz){
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->datsiz = valsiz;
    list->size = 0;
    return list;
}

void list_append(List *list, const void * val){
    List_element *curr, *prev;
    void * dat;

    prev = list->tail;
    curr = malloc(sizeof(List_element));
    /*curr->data = val;*/
    dat = malloc(sizeof(list->datsiz));
    curr->data = memcpy(dat, val, list->datsiz);
    curr->next = NULL;
    list->tail = curr;

    if (list->head == NULL){
        list->head = curr;
        curr->prev = NULL;
    }
    else{
        curr->prev = prev;
        prev->next = curr;
    }

    ++(list->size);
}

void list_prepend(List *list, const void * val){
    List_element *curr, *next;
    void * dat;

    next = list->head;
    curr = malloc(sizeof(List_element));
    /*curr->data = val;*/
    dat = malloc(sizeof(list->datsiz));
    curr->data = memcpy(dat, val, list->datsiz);

    list->head = curr;
    curr->prev = NULL;
    curr->next = next;

    /* handle case of empty list */
    if (next == NULL) 
        list->tail = curr;

    ++(list->size);
}

void list_print(List *list, char* (*tostr) (const void *)){
    List_element *el;
    char* res;

    printf("[");
    for (el = list->head; el != NULL; el = el->next) {
        res = tostr(el->data);
        printf("%s ", res);
        free(res);
    }
    printf("]\n");
}


void list_insert(List  *list, int index, const void * val){
    List_element *el, *prev, *new;
    void * dat;
    int count;

    if (list->size < index + 1){
        printf("Index value %d past end of list: appending\n", index);
        list_append(list,val);
        return;
    }

    for (count = 0, el = list->head; el != NULL; el = el->next, ++count){
        if (count == index){
            prev = el;
            break;
        }
    }

    new = malloc(sizeof(List_element));
    dat = malloc(sizeof(list->datsiz));
    new->data = memcpy(dat, val, list->datsiz);
    new->prev = prev;
    new->next = prev->next;
    prev->next = new;
    ++(list->size);
}

void list_remove(List  *list, int index){
    List_element *el;
    int count;

    for (count = 0, el = list->head; el != NULL; el = el->next, ++count){
        if (count == index){
            break;
        }
    }
    el->prev->next = el->next;
    el->next->prev = el->prev;

    free(el->data);
    free(el);
    --(list->size);
}

void * list_element_at(List *list, int index){
    List_element *el;
    int count;
    void * ret;
    ret = NULL;

    for (count = 0, el = list->head; el != NULL; el = el->next, ++count){
        if (count == index){
            ret = el->data;
        }
    }
    return ret;
}

void list_clear(List *list){
    List_element *el, *curr;
    el = list->head;
    while(el != NULL){
        curr = el;
        el = el->next;
        free(curr->data);
        free(curr);
    }

    list->head = NULL;
    list->tail = list->head;
    list->size = 0;
}

int list_find_first(List *list, const void * val, int (*compar)(const void *, const void *)){
    int index;
    List_element *el;
    for (index=0, el = list->head; el != NULL; el = el->next, ++index) {
        if (compar(el->data, val) == 0) {
            return index;
        }
    }
    return -1;
}

char * tostr(const void *a) {
    char * out;
    int arg1, size;
    arg1 = *(const int*) a;
    size = snprintf(NULL, 0, "%d", arg1); /* This is not ANSI C (C99 feature)*/
    out = (char *) malloc(sizeof(size));
    sprintf(out, "%d", arg1);
    return out;
}

int comp(const void *a, const void *b) {
    int arg1 = *(const int *) a;
    int arg2 = *(const int *) b;
    return arg1 == arg2;
}

int main(int argc, char **argv){
    List *list;
    int index;
    int * prep;
    int * app;
    int * in;
    prep = (int *) malloc(sizeof(int));
    app = (int *) malloc(sizeof(int));
    in = (int *) malloc(sizeof(int));
    *prep = 1;
    *app = 2;
    *in = 3;

    list = list_create(sizeof(int));

    list_prepend(list, prep);
    list_append(list, app);
    list_append(list, app);

    list_prepend(list, prep);
    list_prepend(list, prep);

    list_insert(list, 3, in);
    list_insert(list, 100, in);
    list_print(list, tostr);
    list_remove(list, 3);
    list_print(list, tostr);
    index = list_find_first(list, in, comp);
    printf("-99 is at index: %d\n", index);
    printf("%d\n", *(int *)list_element_at(list, 0));
    list_clear(list);
    list_print(list, tostr);

    free(list);
    free(prep);
    free(app);
    free(in);

    return 0;
}