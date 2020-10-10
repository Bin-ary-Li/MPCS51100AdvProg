#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

List * list_create(size_t valsiz){
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->tail = NULL;
    list->datsiz = valsiz;
    list->size = 0;
    return list;
}

void list_append(List *list, void * val){
    List_element *curr, *prev;

    if (list == NULL || val == NULL) {
        return;
    }

    prev = list->tail;
    curr = malloc(sizeof(List_element));
    curr->data = val;

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

void list_prepend(List *list, void * val){
    List_element *curr, *next;

    if (list == NULL || val == NULL) {
        return;
    }

    next = list->head;
    curr = malloc(sizeof(List_element));
    curr->data = val;

    list->head = curr;
    curr->prev = NULL;
    curr->next = next;

    /* handle case of empty list */
    if (next == NULL) 
        list->tail = curr;

    ++(list->size);
}

void list_print(List *list, char * tostr(const void *, char *), int strsz){
    List_element *el;
    char * res;

    if (list == NULL) {
        return;
    }

    if(list->size == 0){
        printf("[]\n");
        return;
    }

    res = (char *) malloc(strsz);
    printf("[");
    for (el = list->head; el != NULL; el = el->next) {
        res = tostr(el->data, res);
        if (res == NULL){
            printf("Error when stringfying data value");
        } else {
            printf("%s\n", res);
        }
    }
    printf("]\n");
    free(res);
}


void list_insert(List  *list, int index, void * val){
    List_element *el, *prev, *new;
    int count;

    if (list == NULL || val == NULL) {
        return;
    }

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
    new->data = val;
    new->prev = prev;
    new->next = prev->next;
    prev->next = new;
    ++(list->size);
}

void * list_remove(List  *list, int index){
    List_element *el;
    void *re;
    int count;

    if (list == NULL) {
        return NULL;
    }

    if (index > list->size -1 || list->size == 0){
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

void * list_val_at(List *list, int index){
    List_element *el;
    int count;
    void * ret;
    ret = NULL;

    if (list == NULL) {
        return NULL;
    }

    if (index > list->size -1) {
        return NULL;
    }

    for (count = 0, el = list->head; el != NULL; el = el->next, ++count){
        if (count == index){
            ret = el->data;
        }
    }
    return ret;
}

int list_clear(List *list, int (destroy) (void *)){
    List_element *el, *curr;

    if (list == NULL) {
        return -1;
    }

    el = list->head;
    while(el != NULL){
        curr = el;
        el = el->next;
        if (destroy(curr->data) < 0) {
            return -1;
        } else {
            free(curr);
        }
    }

    list->head = NULL;
    list->tail = list->head;
    list->size = 0;
    return 0;
}

int list_find_first(List *list, const void * val, int (*compar)(const void *, const void *)){
    int index;
    List_element *el;

    if (list == NULL || val == NULL || compar == NULL) {
        return -1;
    }

    for (index=0, el = list->head; el != NULL; el = el->next, ++index) {
        if (compar(el->data, val) == 0) {
            return index;
        }
    }
    return -1;
}

int list_destroy(List **list) {
    if (list == NULL || *list == NULL) {
        return -1;
    }
    List * l; 
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