#include "treedict.h"
#include "list.h"

static Node* create_node (void* key, void* item, int(*compar) (const void*, const void*), int(*destroy) (void *)) {
    Node* new = malloc(sizeof(Node));
    new->key = key;
    new->item = item;
    new->count = 1;
    new->compar = compar;
    new->destroy = destroy;
    new->left = new->right = NULL;
    return new;
}

// return the node that contain the key or the closest node
static Node* tree_search (Node* root, const void* key) {
    if (root == NULL) return NULL;

    if (root->left == NULL && root->right == NULL) {
        return root;
    }

    if (root->compar(key, root->key) == 0) {
        return root;
    } else if (root->compar(key, root->key) > 0) {
        if (root->right == NULL) return root;
        return tree_search(root->right, key);
    } else {
        if (root->left == NULL) return root;
        return tree_search(root->left, key);
    }
}

static int tree_destroy (Node* root) {
    if (root == NULL) return -1;
    
    List* l = list_create(NULL, NULL, NULL, NULL, 0);
    list_append(l, root, root);
    while(l->size != 0) {
        Node* n = (Node*) list_remove(l, 0);
        if (n->left != NULL) list_append(l, n->left, n->left);
        if (n->right != NULL) list_append(l, n->right, n->right);
        n->destroy(n->item);
        free(n);
    }

    list_destroy(&l);
    return 0;
}

TreeDict* treedict_create (int(*destroy) (void *), int(*compar) (const void*, const void*)) {
    TreeDict* dict = malloc(sizeof(TreeDict));
    dict->compar = compar;
    dict->destroy = destroy;
    dict->size = 0;
    dict->root = NULL;
    return dict;
}

int treedict_clear (TreeDict* dict) {
    if (dict->size == 0) return -1;

    tree_destroy(dict->root);
    dict->root = NULL;
    dict->size = 0;
    return 0;
}

int treedict_destroy (TreeDict* dict) {
    if (dict->size != 0) return -1;

    free(dict);
    return 0;
}

void* treedict_search (TreeDict* dict, const void* key) {
    Node* n = tree_search(dict->root, key);
    if (n == NULL) return NULL;
    if (n->compar(n->key, key) == 0) {
        return n->item;
    } else {
        return NULL;
    }
}

Node* treedict_search_node (TreeDict* dict, const void* key) {
    return tree_search(dict->root, key);   
}

// put key-item pair to the tree, return # of encounter if key has already been put in the dict
int treedict_put (TreeDict* dict, void* key, void* item) {
    if (dict->root == NULL) { // if nothing in the dict
        dict->root = create_node(key, item, dict->compar, dict->destroy);
        ++(dict->size);
        return 0;
    }

    Node* n = tree_search(dict->root, key); 
    if (n->compar(key, n->key) == 0) { // if found matching node
        ++(n->count);
        return n->count;
    } 

    Node* new = create_node(key, item, dict->compar, dict->destroy);

    if (n->compar(key, n->key) > 0) {
        n->right = new;
    } else {
        n->left = new;
    }
    ++(dict->size);
    return 0;
}

int treedict_getsize (TreeDict* dict) {
    return dict->size;
}

// build a new tree with by reversing the keys and items of the old tree, assuming items are unique.
TreeDict* treedict_reverse (TreeDict* dict, int(*destroy) (void *), int(*compar) (const void*, const void*)) {
    List* l = list_create(NULL, NULL, NULL, NULL, 0);
    TreeDict* new = treedict_create(destroy, compar);
    list_append(l, dict->root, dict->root); 
    while(l->size != 0) {
        Node* n = (Node*) list_remove(l, 0);
        treedict_put(new, n->item, n->key);
        if (n->left != NULL) list_append(l, n->left, n->left);
        if (n->right != NULL) list_append(l, n->right, n->right);
    }
    list_destroy(&l);
    return new;
}
