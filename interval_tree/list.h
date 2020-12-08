#ifndef LIST_H_
#define LIST_H_

typedef struct List_element{
    void * data;
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
    int (*destroy) (void *);
    int (*compar)(const void *, const void *);
    int strsz;
    int size;
} List;


List * list_create(char * (*tostr)(const void *, char *), int (*destroy) (void *),int (*compar)(const void *, const void *), int strsz);

void list_append(List *list, void * data);

void list_prepend(List *list, void * data);

void list_print(List *list);

void list_insert(List *list, int index, void * data);

void * list_remove(List *list, int index);

void * list_val_at(List *list, int index);

int list_clear(List *list);

int list_find_first(List *list, const void * data);

int list_destroy(List **list);

Iterator * list_getIterator(List *list);

void * next(Iterator * iter);

int hasNext(const Iterator* iter);

#endif