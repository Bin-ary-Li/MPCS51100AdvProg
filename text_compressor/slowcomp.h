#include "slowdict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef struct slowcompress_ {
    SlowDict* dict;
    int len_thres, freq_thres;
} SlowCompressor;


SlowCompressor* slowcomp_init(char *filename, int len_thres, int freq_thres);
int slowcomp_destroy(SlowCompressor* comp);
void slowcomp_encode(char *filename, SlowCompressor* comp, char *outfile);
void slowcomp_decode(char *encodefile, SlowCompressor* comp, char *outfile);

