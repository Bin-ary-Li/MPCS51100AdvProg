#include "slowcomp.h"
#define EndOfFile "\n"

static int strcompar(const void* a, const void*b) {
    char* k1 = (char*)(a); 
    char* k2 = (char*)(b);
    return strcmp(k1, k2);
}

static int strdestroy(void* a) {
    char* str = (char*) a;
    free(str);
    return 0;
}

SlowCompressor* slowcomp_create(int len_thres, int freq_thres) {
    SlowCompressor* new = malloc(sizeof(SlowCompressor));
    new->dict = slowdict_create(strdestroy, strcompar);
    new->len_thres = len_thres;
    new->freq_thres = freq_thres;
}

// add a word to be compressed
int slowcomp_add(SlowCompressor* comp, char* word, int len) {
    if (len < 4) return -1;
    assert(word != NULL);
    return slowdict_put(comp->dict, word);
}

int slowcomp_word2id(SlowCompressor* comp, char* word) {
    Entry* e = slowdict_search(comp->dict, word);
    if (e == NULL) return -1;
    if (e->count < comp->freq_thres) return -1;
    return e->index;
}

char* slowcomp_id2word(SlowCompressor* comp, int id) {
    return (char*) slowdict_getkey_byindex(comp->dict, id);
}

int slowcomp_destroy(SlowCompressor* comp) {
    int ret = slowdict_clear(comp->dict);
    ret += slowdict_destroy(comp->dict);
    free(comp);
    return ret;
}

static int parse_add (SlowCompressor* comp, char* string) {
    int isCompressable = 0;
    char* word = (char*) calloc(sizeof(char), (strlen(string)+1));
    int charcnt = 0;
    // strip non-alphabetic character to get the word
    // if non-alphabetic character is encounter, ignore all string after that
    // A special case is made for string started with "\n"
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
        return -1;
    }

    if (strlen(word) >= comp->len_thres) isCompressable = 1;
    if (isCompressable) {
        word[0] = tolower(word[0]); // convert first letter to lower case
        int ret = slowcomp_add(comp, word, strlen(word));
        if (ret != 0) free(word); // if the word is already presented in the compressor
    } else {
        free(word);
    }
    return 0;
}

SlowCompressor* slowcomp_init(char *filename, int len_thres, int freq_thres) { 
    SlowCompressor* comp = slowcomp_create(len_thres, freq_thres);
    char* string_main = NULL; 
    size_t strsize = 0;   
    ssize_t nread;
    char* word = NULL;
    char* string = NULL; 
    FILE *file = fopen(filename, "r");

    while ((nread = getdelim(&string_main, &strsize, ' ', file)) != -1) {
        string = strtok(string_main, "\n");
        while (string) {
            int ret = parse_add(comp, string); // parse_add return -1 when string is empty
            string = strtok(NULL, "\n");
        }
    }
    free(string_main);
    fclose(file);
    return comp;
}

static void encoding(SlowCompressor* comp, FILE* encoded, char* string) {
    int isCompressable = 0; // indicator of whether current word is compressable
    int wordID = -1; // compressed id of a word
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

    if (isCompressable) wordID = slowcomp_word2id(comp, word);
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

void slowcomp_encode(char *filename, SlowCompressor* comp, char *outfile) { 
    char* string_main = NULL;
    char* string = NULL; 
    char* word = NULL;
    size_t strsize = 0;   
    ssize_t nread; 
    FILE* file = fopen(filename, "r");
    FILE* encoded = fopen(outfile,"w");

    while ((nread = getdelim(&string_main, &strsize, ' ', file)) != -1) {
        string = strtok(string_main, "\n");
        while (string) {
            encoding(comp, encoded, string);
            string = strtok(NULL, "\n");
            if (string!=NULL) fprintf(encoded,"\n"); // if there are more to tokenize (i.e. more line break), add it back
        }
    }
    fprintf(encoded,EndOfFile);
    free(string_main);
    fclose(file);
    fclose(encoded);
}

static void decoding(SlowCompressor* comp, FILE* decoded, char* string) {
    int decodeNeeded = 0;
    char* word = (char*) calloc(sizeof(char), (strlen(string)+1));
    int charcnt = 0;
    int wordID = -1;
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
    } else if (isalpha(word[0])) {
        decodeNeeded = 0;
    } else {
        decodeNeeded = 1;
        wordID = atoi(word); // only decode when word is a number
    }

    // print non-alphabetic characters and decoded word
    decodeNeeded? fprintf(decoded,"%s", slowcomp_id2word(comp, wordID)) : fprintf(decoded,"%s", word);
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

void slowcomp_decode(char *encodefile, SlowCompressor* comp, char *outfile) { 
    char* string_main = NULL;
    char* string = NULL; 
    char* word = NULL;
    size_t strsize = 0;   
    ssize_t nread; 
    FILE* file = fopen(encodefile, "r");
    FILE* decoded = fopen(outfile,"w");

    while ((nread = getdelim(&string_main, &strsize, ' ', file)) != -1) {
        string = strtok(string_main, "\n");
        while (string) {
            decoding(comp, decoded, string);
            string = strtok(NULL, "\n");
            if (string!=NULL) fprintf(decoded,"\n");
        }
    }
    fprintf(decoded,EndOfFile);
    free(string_main);
    fclose(file);
    fclose(decoded);
}

// int main(int argc, char** argv) {
//     char* filename = "test.txt";
//     SlowCompressor* comp = slowcomp_init(filename, 4, 3);
//     slowcomp_encode(filename, comp);
//     slowcomp_decode(ENCODEFILE, comp);
//     slowcomp_destroy(comp);
// }