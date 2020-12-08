#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* list_create creates a list structure, with the ability to store any type of data provided that the user 
supplies the constructor with function as to how to stringfy (tostr()), destruct (destroy()), and 
compare (compar()) the stored data. */
List * list_create(char * (*tostr)(const void *, char *), int (*destroy) (void *),int (*compar)(const void *, const void *), int strsz){
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->iter = NULL;
    list->size = 0;
    list->tostr = tostr;
    list->compar = compar;
    list->destroy = destroy;
    list->strsz = strsz;
    return list;
}

/* get an iterator of a list, which can be passed to next() funtion to get the next element in the list 
and to hasNext() function to see if the iterator has reached the end of the list.  */
Iterator * list_getIterator(List *list) {
    Iterator * it;
    if (list->iter != NULL) {
        list->iter->nextEl = list->head;
        return list->iter;
    }
    it = (Iterator*) malloc(sizeof(Iterator));
    it->nextEl = list->head;
    list->iter = it;
    return it;
}

void * next(Iterator * iter) {
    List_element * el;
    void * data;
    el = iter->nextEl; 
    if (el == NULL) {
        return NULL;
    }
    data = el->data;
    iter->nextEl = el->next;
    return data;
} 

int hasNext(const Iterator* iter) {
    if (iter->nextEl == NULL) {
        return 0;
    } else {
        return 1;
    }
}

/* The usual append function, place the input data at the end of the list */
void list_append(List *list, void * data){
    List_element *curr, *prev;

    if (list == NULL) {
        return;
    }

    prev = list->tail;
    curr = malloc(sizeof(List_element));
    curr->data = data;

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

/* The usual prepend function, place the input data at the start of the list. */
void list_prepend(List *list, void * data){
    List_element *curr, *next;

    if (list == NULL) {
        return;
    }

    next = list->head;
    curr = malloc(sizeof(List_element));
    curr->data = data;

    list->head = curr;
    curr->prev = NULL;
    curr->next = next;

    /* handle case of empty list */
    if (next == NULL) 
        list->tail = curr;

    ++(list->size);
}

/* The print function use the tostr() method supplies by the user to fill a character buffer with the stringfied
information of a data and print these information for each data point in the list.  */
void list_print(List *list){
    List_element *el;
    char * res;

    if (list == NULL) {
        return;
    }

    if(list->size == 0){
        printf("[]\n");
        return;
    }

    res = (char *) malloc(list->strsz);
    printf("[");
    for (el = list->head; el != NULL; el = el->next) {
        res = list->tostr(el->data, res);
        if (res == NULL){
            printf("Error when stringfying data value");
        } else {
            printf("%s\n", res);
        }
    }
    printf("]\n");
    free(res);
}

/* The insert function put the input data in the specified index position in the list, if index goes pass
the end of list, append to the end of the list. */
void list_insert(List  *list, int index, void * data){
    List_element *el, *next, *new;
    int count;

    if (list == NULL) {
        return;
    }

    if (list->size < index + 1){
        printf("Index value %d past end of list: appending\n", index);
        list_append(list,data);
        return;
    }

    for (count = 0, el = list->head; el != NULL; el = el->next, ++count){
        if (count == index){
            next = el;
            break;
        }
    }

    new = malloc(sizeof(List_element));
    new->data = data;
    new->next = next;
    new->prev = next->prev;
    new->prev->next = new;
    next->prev = new;
    ++(list->size);
}

/* Remove the data at the index location of the list and return the removed data. */
void * list_remove(List  *list, int index){
    List_element *el;
    void *re;
    int count;

    if (list == NULL) {
        return NULL;
    }
    if (list->size == 0) {
        printf("Error: list is empty.\n");
        return NULL;
    }
    if (index > list->size -1 || index < 0){
        printf("Error: index out of bound.\n");
        return NULL;
    }

    for (count = 0, el = list->head; el != NULL; el = el->next, ++count){
        if (count == index){
            break;
        }
    }

    if (index == 0) {
        list->head = el->next;
    } else if (index == list->size - 1){
        list->tail = el->prev;
        el->prev->next = NULL;
    } else {
        el->prev->next = el->next;
        el->next->prev = el->prev;
    }

    re = el->data;
    free(el);
    --(list->size);
    return re;
}

/* Return the data at the index location of the list without removing it. */
void * list_val_at(List *list, int index){
    List_element *el;
    int count;
    void * ret;
    ret = NULL;

    if (list == NULL) {
        return NULL;
    }
    if (index > list->size -1 || index < 0) {
        printf("Error: Index out of bound\n");
        return NULL;
    }

    for (count = 0, el = list->head; el != NULL; el = el->next, ++count){
        if (count == index){
            ret = el->data;
        }
    }
    return ret;
}

/* Find the first index position in the list that stores the same data as the input data. How to compare them is
determined by the comparator function that the list stores when constructed. */
int list_find_first(List *list, const void * data){
    int index;
    List_element *el;

    if (list == NULL) {
        return -1;
    }

    for (index=0, el = list->head; el != NULL; el = el->next, ++index) {
        if (list->compar(el->data, data) == 0) {
            return index;
        }
    }
    return -1;
}

/* Empty up all the data stored in a list, return the list to the state when it was just created. */
int list_clear(List *list){
    List_element *el, *curr;

    if (list == NULL) {
        return -1;
    }

    el = list->head;
    while(el != NULL){
        curr = el;
        el = el->next;
        if (list->destroy(curr->data) < 0) {
            return -1;
        } else {
            free(curr);
        }
    }

    if (list->iter != NULL) {
        free(list->iter);
        list->iter = NULL;
    }

    list->head = NULL;
    list->tail = list->head;
    list->size = 0;
    return 0;
}

/* Destruct a list instance. This function requires user to pass in a double pointer to a list in order
to modify the list instance itself. Upon success, the pointer to the list will be set to NULL for safeguarding. */
int list_destroy(List **list) {
    List * l; 
    if (list == NULL || *list == NULL) {
        return -1;
    }
    l = *list;

    if (l->head!=NULL) {
        printf("Unsuccessful, list is nonemtpy, call clear_list first\n");
        return -1;
    } else {
        free(l);
        *list = NULL;
        return 0;
    }
}