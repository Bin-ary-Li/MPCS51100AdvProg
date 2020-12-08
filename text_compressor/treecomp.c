#include "treecomp.h"
#define EndOfFile "\n"

static int strcompar(const void* a, const void*b) {
    char* k1 = (char*)(a); 
    char* k2 = (char*)(b);
    return strcmp(k1, k2);
}

static int strdestroy(void* a) {
    char* stra = (char*) a;
    free(stra);
    return 0;
}

static int intcompar(const void* a, const void*b) {
    int* k1 = (int*)(a); 
    int* k2 = (int*)(b);
    if (*k1 < *k2) return -1;
    if (*k1 == *k2) return 0;
    if (*k1 > *k2) return 1;
}

static int intdestroy(void* a) {
    int* stra = (int*) a;
    free(stra);
    return 0;
}

TreeCompressor* treecomp_create(int len_thres, int freq_thres) {
    TreeCompressor* comp = malloc(sizeof(TreeCompressor));
    comp->word2id = treedict_create(intdestroy, strcompar);
    comp->id2word = NULL;
    comp->freq_thres = freq_thres;
    comp->len_thres = len_thres;
}

int treecomp_add(TreeCompressor* comp, char* word, int* id) {
    assert(word != NULL);
    return treedict_put(comp->word2id, word, id);
}

int treecomp_word2id(TreeCompressor* comp, char* word) {
    Node* n = treedict_search_node(comp->word2id, word);
    if (n == NULL) return -1;
    if (n->count < comp->freq_thres) return -1; // not compressing infrequent words 
    return *((int*)n->item);
}

char* treecomp_id2word(TreeCompressor* comp, int* id) {
    char* word = (char*) treedict_search(comp->id2word, id);
    if (word == NULL) return NULL;
    return word;
}

int treecomp_destroy (TreeCompressor* comp) {
    int ret = treedict_clear(comp->word2id);
    ret += treedict_clear(comp->id2word);
    ret += treedict_destroy(comp->word2id);
    ret += treedict_destroy(comp->id2word);
    if (ret == 0) free(comp);
    return ret;
}

static int parse_add (TreeCompressor* comp, char* string, int id) {
    int isCompressable = 0;
    char* word = (char*) calloc(sizeof(char), (strlen(string)+1));
    int * idptr = (int*) malloc(sizeof(int));
    int charcnt = 0;
    // strip non-alphabetic character to get the word
    // if non-alphabetic character is encounter, ignore all string after that
    for (int i = 0; i < strlen(string); ++i) {
        if (isalpha(string[i]) == 0) {
            break;
        } 
        word[charcnt] = string[i];
        ++charcnt;
    }
    // if no word detected in this portion of the text
    if (strlen(word) == 0) {
        free(word);
        free(idptr);
        return -1;
    }

    if (strlen(word) >= comp->len_thres) isCompressable = 1;
    if (isCompressable) {
        word[0] = tolower(word[0]); // convert first letter to lower case
        *idptr = id;
        int success = treecomp_add(comp, word, idptr); // return 0 if not encounter by the compressor before
        if (success != 0) { // update id only if the word has never been encounter
            free(word);
            free(idptr);
            return -1;
        } else {
            return 0;
        }
    } else {
        free(word);
        free(idptr);
        return -1;
    }
}

TreeCompressor* treecomp_init(char *filename, int len_thres, int freq_thres) { 
    TreeCompressor* comp = treecomp_create(len_thres, freq_thres);
    char* string = NULL;
    char* substr = NULL; 
    char* word = NULL;
    size_t strsize = 0;   
    ssize_t nread;
    int isCompressable = 0; 
    int id = 0;
    int* idptr = NULL;
    FILE *file = fopen(filename, "r");

    // looping through all the word and put them in the compressor
    while ((nread = getdelim(&string, &strsize, ' ', file)) != -1) {
        substr = strtok(string, "\n");
        while (substr) {
            int ret = parse_add(comp, substr, id); // parse_add return 0 if string is successfully added to the compressor
            if (ret == 0) ++id;
            substr = strtok(NULL, "\n");
        }
    }
    comp->id2word = treedict_reverse(comp->word2id, strdestroy, intcompar);
    free(string);
    fclose(file);
    return comp;
}

static void encoding(TreeCompressor* comp, FILE* encoded, char* string) {
    int isCompressable = 0;
    int wordID = -1;
    char* word = (char*) calloc(sizeof(char), (strlen(string)+1));
    int charcnt = 0;
    // strip non-alphabetic character to get the word
    // if non-alphabetic character is encounter, ignore all string after that
    for (int i = 0; i < strlen(string); ++i) {
        if (isalpha(string[i]) == 0) {
            break;
        } 
        word[charcnt] = string[i];
        ++charcnt;
    }
    if (strlen(word) > 0) word[0] = tolower(word[0]); // convert first letter to lower case
    if (strlen(word) >= comp->len_thres) isCompressable = 1; // don't compress shorter word

    if (isCompressable) wordID = treecomp_word2id(comp, word);
    if (wordID < 0) isCompressable = 0;

    // print non-alphabetic characters and encoded word
    isCompressable? fprintf(encoded,"%d", wordID) : fprintf(encoded,"%s", word);
    int reachDelim = 0;
    for (int i = 0; i < strlen(string); ++i) { // print rest of the uncompress character
        if (isalpha(string[i]) == 0) { 
            ++reachDelim;
        }
        if(reachDelim > 0) {
            fprintf(encoded, "%c", string[i]);
        }
    }
    free(word);
}

void treecomp_encode(char *filename, TreeCompressor* comp, char* outfile) { 
    char* string = NULL; 
    char* substr = NULL;
    char* word = NULL;
    size_t strsize = 0;   
    ssize_t nread; 
    FILE* file = fopen(filename, "r");
    FILE* encoded = fopen(outfile,"w");

    while ((nread = getdelim(&string, &strsize, ' ', file)) != -1) {
        substr = strtok(string, "\n");
        while (substr) {
            encoding(comp, encoded, substr);
            substr = strtok(NULL, "\n");
            if (substr!=NULL) fprintf(encoded,"\n");
        }
    }
    fprintf(encoded,EndOfFile);
    free(string);
    fclose(file);
    fclose(encoded);
}

static void decoding(TreeCompressor* comp, FILE* decoded, char* string) {
    int decodeNeeded = 0;
    int wordID = -1;
    char* word = (char*) calloc(sizeof(char), (strlen(string)+1));
    int charcnt = 0;
    // strip non-alphanumeric character to get the word
    // if non-alphanumeric character is encounter, ignore all string after that
    for (int i = 0; i < strlen(string); ++i) {
        if (isalnum(string[i]) == 0) {
            break;
        } 
        word[charcnt] = string[i];
        ++charcnt;
    }

    if (strlen(word) == 0) {
        decodeNeeded = 0;
    } else if (isalpha(word[0]) || isalnum(word[0]) == 0) {
        decodeNeeded = 0;
    } else {
        decodeNeeded = 1;
        wordID = atoi(word); // only decode when word is a number
    }

    // print non-alphabetic characters and decoded word
    decodeNeeded? fprintf(decoded,"%s", treecomp_id2word(comp, &wordID)) : fprintf(decoded,"%s", word);
    int reachDelim = 0;
    for (int i = 0; i < strlen(string); ++i) { // print rest of the uncompress character
        if (isalnum(string[i]) == 0) { 
            ++reachDelim;
        }
        if(reachDelim > 0) {
            fprintf(decoded, "%c", string[i]);
        }
    }
    free(word);
}

void treecomp_decode(char *encodefile, TreeCompressor* comp, char* outfile) { 
    char* string = NULL; 
    char* substr = NULL;
    char* word = NULL;
    size_t strsize = 0;   
    ssize_t nread; 
    int decodeNeeded = 0; // indicator of whether current word need to decode
    int wordID = -1;
    FILE* file = fopen(encodefile, "r");
    FILE* decoded = fopen(outfile,"w");

    while ((nread = getdelim(&string, &strsize, ' ', file)) != -1) {
        substr = strtok(string, "\n");
        while (substr) {
            decoding(comp, decoded, substr);
            substr = strtok(NULL, "\n");
            if (substr!=NULL) fprintf(decoded,"\n");
        }
    }
    fprintf(decoded,EndOfFile);
    free(string);
    fclose(file);
    fclose(decoded);
}

// int main(int argc, char** argv) {
//     char* filename = "test.txt";
//     TreeCompressor* comp = treecomp_init(filename, 4, 2);
//     treecomp_encode(filename, comp);
//     treecomp_decode(ENCODEFILE, comp);
//     treecomp_destroy(comp);
// }
