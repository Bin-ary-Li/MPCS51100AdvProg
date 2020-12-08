#ifndef LIST_H_
#define LIST_H_
#include <stdio.h>
#include <stdlib.h>

typedef struct List_element{
    void* key;
    void* data;
    struct List_element *next;
    struct List_element *prev;
} List_element;

typedef struct iterator_{
    List_element * nextEl;
} Iterator;

typedef struct List{
    List_element *head;
    List_element *tail;
    Iterator * iter;
    char * (*tostr)(const void *, char *);
    int (*destroy_key) (void *);
    int (*destroy_data) (void *);
    int (*compar)(const void *, const void *);
    int strsz;
    int size;
} List;


List * list_create(char * (*tostr)(const void *, char *), int (*destroy_key) (void *), int (*destroy_data) (void *),int (*compar)(const void *, const void *), int strsz);

void list_append(List *list, void* key, void* data);

void list_prepend(List *list, void* key, void * data);

void list_print(List *list);

void list_insert(List *list, int index, void* key, void * data);

void * list_remove(List *list, int index);

void * list_val_at(List *list, int index);

void * list_key_at(List *list, int index);

int list_clear(List *list);

int list_find_first(List *list, const void* key);

int list_destroy(List **list);

Iterator * list_getIterator(List *list);

void * next(Iterator * iter);

int hasNext(const Iterator* iter);

#endif