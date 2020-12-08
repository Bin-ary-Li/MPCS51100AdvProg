#include "hashcomp.h"
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

static int entrydestroy(void* a) {
    free(a);
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

HashCompressor* hashcomp_create(int n_vocab, int len_thres, int freq_thres, unsigned int(*hash) (void*, int)) {
    HashCompressor* comp = malloc(sizeof(HashCompressor));
    comp->map = hashmap_create(strdestroy, entrydestroy, strcompar, hash, n_vocab);
    comp->freq_thres = freq_thres;
    comp->len_thres = len_thres;
}

// return 0 if success, -1 if key has already been added
int hashcomp_add(HashCompressor* comp, char* word) {
    assert(word != NULL);
    DataEntry* e = malloc(sizeof(DataEntry));
    e->string = word;
    e->count = 1;
    int success = hashmap_put(comp->map, word, e);
    if (success != 0) { // if key is already in the map, find that item and increment the counter
        DataEntry* exist = (DataEntry*) hashmap_search(comp->map, word);
        ++(exist->count);
        free(e);
    }
    return success;
}

EntryID* hashcomp_word2id(HashCompressor* comp, char* word) {
    DataEntry* entry = (DataEntry*) hashmap_search(comp->map, word);
    if (entry == NULL) return NULL;
    if (entry->count < comp->freq_thres) return NULL; // not compressing infrequent words
    unsigned int hashid = hashmap_getid(comp->map, word);
    int listind = hashmap_search_index(comp->map, word);
    EntryID* eid = malloc(sizeof(EntryID));
    eid->hashid = hashid;
    eid->listindex = listind;
    return eid;
}

char* hashcomp_id2word(HashCompressor* comp, unsigned int hashid, int listind) {
    DataEntry* e = (DataEntry*) hashmap_search_byID(comp->map, hashid, listind);
    if (e == NULL) return NULL;
    return e->string;
}

int hashcomp_destroy(HashCompressor* comp) {
    if (comp == NULL) return -1;
    int ret = hashmap_destroy(comp->map);
    if (ret == 0) free(comp);
    return ret;
}

static int parse_add (HashCompressor* comp, char* string) {
    int isCompressable = 0;
    char *word = (char *) calloc(sizeof(char), (strlen(string)+1));
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
        return -1;
    }

    if (strlen(word) >= comp->len_thres) isCompressable = 1;
    if (isCompressable) {
        word[0] = tolower(word[0]); // convert first letter to lower case
        int success = hashcomp_add(comp, word); // return 0 if the word hasn't been added before
        if (success != 0) {
            free(word);
            return -1;
        } 
    } else {
        free(word);
        return -1;
    }
    return 0;
}

HashCompressor* hashcomp_init(char *filename, int n_vocab, int len_thres, int freq_thres, unsigned int(*hash) (void*, int)) { 
    HashCompressor* comp = hashcomp_create(n_vocab, len_thres, freq_thres, hash);
    char *string = NULL; 
    char *substr = NULL;
    size_t strsize = 0;   
    ssize_t nread;
    FILE *file = fopen(filename, "r");

    // looping through all the word and put them in the compressor
    while ((nread = getdelim(&string, &strsize, ' ', file)) != -1) {
        substr = strtok(string, "\n");
        while (substr) {
            int ret = parse_add(comp, substr);
            substr = strtok(NULL, "\n");
        }
    }
    free(string);
    fclose(file);
    return comp;
}

static void encoding(HashCompressor *comp, FILE *encoded, char *string) {
    int isCompressable = 0; // indicator of whether current word is compressable
    EntryID* wordID = NULL; // compressed id of the word
    char *word = (char*) calloc(sizeof(char), (strlen(string)+1));
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

    if (isCompressable) wordID = hashcomp_word2id(comp, word);
    if (wordID == NULL) isCompressable = 0;

    // print non-alphabetic characters and encoded word
    isCompressable? fprintf(encoded,"%d^%d", wordID->hashid, wordID->listindex) : fprintf(encoded,"%s", word);
    int reachDelim = 0;
    for (int i = 0; i < strlen(string); ++i) { // print rest of the uncompress character
        if (isalpha(string[i]) == 0) { 
            ++reachDelim;
        }
        if(reachDelim > 0) {
            fprintf(encoded, "%c", string[i]);
        }
    }
    if (isCompressable) free(wordID);
    free(word);
}

void hashcomp_encode(char *filename, HashCompressor* comp, char* outfile) { 
    char *string = NULL; 
    char *substr = NULL;
    char *word = NULL;
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

static void decoding(HashCompressor *comp, FILE *decoded, char *string) {
    int decodeNeeded = 0;
    unsigned int hashid = -1;
    int listindex = -1;
    char *word = (char*) calloc(sizeof(char), (strlen(string)+1));  
    int charcnt = 0;
    // strip non-alphanumeric character to get the word
    // if non-alphanumeric character is encounter (except for '^'), ignore all string after that
    for (int i = 0; i < strlen(string); ++i) {
        if (isalnum(string[i]) == 0 && string[i] != '^') {
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
        char *end;
        char *token = strtok_r(word, "^", &end);
        hashid = atoi(token);
        token = strtok_r(NULL, "^", &end);
        listindex = atoi(token);
        token = strtok_r(NULL, "^", &end);
        assert(token == NULL); // assert that decode symbol is strictly in the format of "num^num".
    }

    // print non-alphabetic characters and decoded word
    decodeNeeded? fprintf(decoded,"%s", hashcomp_id2word(comp, hashid, listindex)) : fprintf(decoded,"%s", word);
    int reachDelim = 0;
    for (int i = 0; i < strlen(string); ++i) { // print rest of the uncompress character
        if (isalnum(string[i]) == 0 && string[i] != '^') { 
            ++reachDelim;
        }
        if(reachDelim > 0) {
            fprintf(decoded, "%c", string[i]);
        }
    }
    free(word);
}

void hashcomp_decode(char *encodefile, HashCompressor *comp, char *outfile) { 
    char *string = NULL; 
    char *substr = NULL;
    size_t strsize = 0;   
    ssize_t nread; 
    FILE *file = fopen(encodefile, "r");
    FILE *decoded = fopen(outfile,"w");

    while ((nread = getdelim(&string, &strsize, ' ', file)) != -1) {
        char *end;
        substr = strtok_r(string, "\n", &end);
        while (substr) {
            decoding(comp, decoded, substr);
            substr = strtok_r(NULL, "\n", &end);
            if (substr!=NULL) fprintf(decoded,"\n");
        }
    }
    fprintf(decoded,EndOfFile);
    free(string);
    fclose(file);
    fclose(decoded);
}

// int main(int argc, char** argv) {
//     HashCompressor* comp = hashcomp_init("test_document.txt", 2000, 4, 2);
//     hashcomp_encode("test_document.txt", comp);
//     hashcomp_decode(ENCODEFILE, comp);
//     hashcomp_destroy(comp);
// }
