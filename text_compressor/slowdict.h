#include "list.h"

typedef struct entry_ {
    int count;
    int index;
} Entry;

typedef struct slowdict_ {
    List* dict_list;
} SlowDict;

SlowDict* slowdict_create (int (*destroy_key) (void *), int(*compar) (const void*, const void*));
int slowdict_destroy (SlowDict* dict);
int slowdict_clear (SlowDict* dict);
Entry* slowdict_search (SlowDict* dict, void* key);
Entry* slowdict_search_byindex (SlowDict* dict, int index);
void* slowdict_getkey_byindex (SlowDict* dict, int index);
int slowdict_put (SlowDict* dict, void* key);
int slowdict_getsize (SlowDict* dict);