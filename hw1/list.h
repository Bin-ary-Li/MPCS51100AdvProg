#ifndef LIST_H_
#define LIST_H_

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


List * list_create(size_t valsiz);

void list_append(List *list, void * val);

void list_prepend(List *list, void * val);

void list_print(List *list, int tostr(const void *, char *), int strsz);

void list_insert(List *list, int index, void * val);

void * list_remove(List *list, int index);

void * list_element_at(List *list, int index);

int list_clear(List *list, int (destory) (void *));

int list_find_first(List *list, const void * val, int (*compar)(const void *, const void *));

int list_destroy(List *list);

#endif