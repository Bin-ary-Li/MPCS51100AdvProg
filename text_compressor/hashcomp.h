#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct dataentry_ {
    char* string;
    int count;
} DataEntry;

typedef struct entryid_ {
    unsigned int hashid;
    int listindex;
} EntryID;

typedef struct hashcompress_ {
    HashMap* map;
    int len_thres, freq_thres;
} HashCompressor;

HashCompressor* hashcomp_init(char *filename, int n_vocab, int len_thres, int freq_thres, unsigned int(*hash) (void*, int));
int hashcomp_destroy(HashCompressor* comp);
void hashcomp_encode(char *filename, HashCompressor* comp, char *outfile);
void hashcomp_decode(char *encodefile, HashCompressor *comp, char *outfile);
