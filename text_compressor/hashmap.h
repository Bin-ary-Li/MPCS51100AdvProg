#include "list.h"

typedef struct bucket_ {
    int key;
    List* item;
} Bucket;

typedef struct hashmap_ {
    Bucket** bucket;
    int n_bucket;
    int n_item;
    unsigned int(*hash) (void*, int);
} HashMap;

HashMap* hashmap_create (int (*destroy_key) (void *), int (*destroy_data) (void *), int(*compar) (const void*, const void*), unsigned int(*hash) (void*, int), int n_bucket);
int hashmap_destroy (HashMap* map);
int hashmap_clear (HashMap* map);
void* hashmap_search (HashMap* map, void* key);
void* hashmap_search_byID (HashMap* map, unsigned int hashid, int listind);
int hashmap_search_index (HashMap* map, void* key);
int hashmap_put (HashMap* map, void* key, void* item);
void* hashmap_remove (HashMap* map, const void* key);
int hashmap_getsize (HashMap* map);
unsigned int hashmap_getid (HashMap* map, void* key);
