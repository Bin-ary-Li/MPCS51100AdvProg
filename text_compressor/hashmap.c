#include "hashmap.h"

static void init_bucket (Bucket** emptyarr, int n_bucket, int (*destroy_key) (void *), int (*destroy_data) (void *), int(*compar) (const void*, const void*)) {
    for (int i = 0; i< n_bucket; ++i) {
        emptyarr[i] = malloc(sizeof(Bucket));
        emptyarr[i]->key = i;
        emptyarr[i]->item = list_create(NULL, destroy_key, destroy_data, compar, 0);
    }
}

HashMap* hashmap_create (int (*destroy_key) (void *), int (*destroy_data) (void *), int(*compar) (const void*, const void*), unsigned int(*hash) (void*, int), int n_bucket) {
    HashMap* new = malloc(sizeof(HashMap));
    new->bucket = malloc(sizeof(Bucket*) * n_bucket);
    init_bucket(new->bucket, n_bucket, destroy_key, destroy_data, compar);
    new->hash = hash;
    new->n_bucket = n_bucket;
    new->n_item = 0;
    return new;
}

static void clear_bucket (Bucket** b, int n_bucket) {
    for (int i = 0; i < n_bucket; ++i) {
        list_clear(b[i]->item);
        list_destroy(&(b[i]->item));
        free(b[i]);
        b[i] = NULL;
    }
}

int hashmap_destroy (HashMap* map) {
    clear_bucket(map->bucket, map->n_bucket);
    free(map->bucket);
    free(map);
    return 0;
}

void* hashmap_search (HashMap* map, void* key) {
    int bindex = map->hash(key, map->n_bucket);
    Bucket* b = map->bucket[bindex];
    int kindex = list_find_first(b->item, key);
    
    if (kindex == -1) { // if not found in bucket
        return NULL;
    } else {
        return list_val_at(b->item, kindex);
    }
}

int hashmap_search_index (HashMap* map, void* key) {
    int bindex = map->hash(key, map->n_bucket);
    Bucket* b = map->bucket[bindex];
    return list_find_first(b->item, key);
}

void* hashmap_search_byID (HashMap* map, unsigned int hashid, int listind) {
    if (hashid >= map->n_bucket) return NULL;
    Bucket* b = map->bucket[hashid];
    return list_val_at(b->item, listind);
}

// Put key-item pair into the hashmap, return -1 if the key is already in the map
int hashmap_put (HashMap* map, void* key, void* item) {
    unsigned int bindex = map->hash(key, map->n_bucket);
    Bucket* b = map->bucket[bindex];
    int kindex = list_find_first(b->item, key);
    
    if (kindex != -1) { // if found in bucket
        return -1;
    } else {
        list_append(b->item, key, item);
        ++(map->n_item);
        return 0;
    }  
}

int hashmap_getsize (HashMap* map) {
    return map->n_item;
}

unsigned int hashmap_getid (HashMap* map, void* key) {
    return map->hash(key, map->n_bucket);
}
