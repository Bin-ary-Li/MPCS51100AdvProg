#include <stdio.h>
#include <stdlib.h>

typedef struct node_ {
    void* key;
    void* item;
    struct node_* left;
    struct node_* right; 
    int count;
    int(*compar) (const void*, const void*);
    int(*destroy) (void *);
} Node;

typedef struct treedict_ {
    Node* root;
    int size;
    int(*compar) (const void*, const void*);
    int(*destroy) (void *);
} TreeDict;

TreeDict* treedict_create (int(*destroy) (void *), int(*compar) (const void*, const void*));
int treedict_destroy (TreeDict* dict);
int treedict_clear (TreeDict* dict);
void* treedict_search (TreeDict* dict, const void* key);
Node* treedict_search_node (TreeDict* dict, const void* key);
int treedict_put (TreeDict* dict, void* key, void* item);
void* treedict_remove (TreeDict* dict, const void* key);
int treedict_getsize (TreeDict* dict);
TreeDict* treedict_reverse (TreeDict* dict, int(*destroy) (void *), int(*compar) (const void*, const void*));