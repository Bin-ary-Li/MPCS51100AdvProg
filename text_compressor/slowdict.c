#include "slowdict.h"

static int entrydestroy(void* a) {
    free(a);
    return 0;
}

SlowDict* slowdict_create (int (*destroy_key) (void *), int(*compar) (const void*, const void*)) {
    SlowDict* dict = malloc(sizeof(SlowDict));
    dict->dict_list = list_create(NULL, destroy_key, entrydestroy, compar, 0);
    return dict;
}

int slowdict_clear (SlowDict* dict) {
    if (list_clear(dict->dict_list) != 0) return -1;
    return 0;
}

int slowdict_destroy (SlowDict* dict) {
    if (list_destroy(&(dict->dict_list)) != 0) return -1;
    free(dict);
    return 0;
}

Entry* slowdict_search (SlowDict* dict, void* key) {
    int index = list_find_first(dict->dict_list, key);
    if (index == -1) return NULL;
    return (Entry*) list_val_at(dict->dict_list, index);
}

Entry* slowdict_search_byindex (SlowDict* dict, int index) {
    return (Entry*) list_val_at(dict->dict_list, index);;
}

void* slowdict_getkey_byindex (SlowDict* dict, int index) {
    return list_key_at(dict->dict_list, index);
}

int slowdict_put (SlowDict* dict, void* key) {
    int index = list_find_first(dict->dict_list, key);
    if (index != -1) { // if key is found in the dict
        Entry* ret = (Entry*) list_val_at(dict->dict_list, index);
        ++(ret->count);
        return -1;
    } 

    Entry* new = malloc(sizeof(Entry));
    new->index = dict->dict_list->size;
    new->count = 1;
    list_append(dict->dict_list, key, new);
    return 0;
}

int slowdict_getsize (SlowDict* dict) {
    return dict->dict_list->size;
}

