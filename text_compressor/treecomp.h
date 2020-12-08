#include "treedict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct treecompress_ {
    TreeDict* word2id;
    TreeDict* id2word;
    int len_thres, freq_thres;
} TreeCompressor;

TreeCompressor* treecomp_init(char *filename, int len_thres, int freq_thres);
int treecomp_destroy (TreeCompressor* comp);
void treecomp_encode(char *filename, TreeCompressor* comp, char* outfile);
void treecomp_decode(char *encodefile, TreeCompressor* comp, char* outfile);
