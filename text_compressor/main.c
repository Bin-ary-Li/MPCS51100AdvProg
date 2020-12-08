#include <string.h>
#include <assert.h>
#include <errno.h>
#include "slowcomp.h"
#include "treecomp.h"
#include "hashcomp.h"
#include "timer.h"

unsigned int naive_hash(void* data, int nbins) {
    char *word = (char*) data;
    unsigned int h = 0;
    int c;
    while(c = *word++)
        h += c;
    return h % nbins;
}

unsigned int bernstein_hash(void *data, int nbins) {
    char *word = (char*) data;
    unsigned int h = 5381;
    int c;
    while(c = *word++)
        h = 33 * h + c;
    return h % nbins;
}

unsigned int FNV_hash(void *data, int nbins) {
    char *word = (char*) data;
    unsigned long h = 14695981039346656037lu;
    char c;
    while(c = *word++)
    {
        h = h * 1099511628211lu;
        h = h ^ c;
    }
    return h % nbins;
}

long int findSize(char file_name[]) { 
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r"); 
  
    // checking if the file exist or not 
    if (fp == NULL) { 
        printf("File Not Found!\n"); 
        return -1; 
    } 
  
    fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int res = ftell(fp); 
  
    // closing the file 
    fclose(fp); 
  
    return res; 
}

// // Simple unit testing
// void unit_test() {
//     char* k1 = "key1";
//     char* v1 = "val1";
//     char* k2 = "key2";
//     char* v2 = "val2";
//     SlowDict* dict = slowdict_create(strdestroy_pseudo, strcompar);
//     slowdict_put(dict, k1, v1);
//     assert( strcmp((char*)slowdict_search(dict, k1), "val1") == 0);
//     slowdict_put(dict, k2, v2);
//     slowdict_put(dict, k2, v2);
//     assert(strcmp((char*)slowdict_search(dict, k2), "val2") == 0);
//     assert(slowdict_getsize(dict) == 2);
//     slowdict_clear(dict);
//     assert(slowdict_search(dict, k2) == NULL);
//     assert(slowdict_getsize(dict) == 0);
//     slowdict_destroy(dict);

//     TreeDict* tdict = treedict_create(strdestroy_pseudo, strcompar);
//     treedict_put(tdict, k1, v1);
//     assert( strcmp((char*)treedict_search(tdict, k1), "val1") == 0);
//     treedict_put(tdict, k2, v2);
//     treedict_put(tdict, k2, v2);
//     assert(strcmp((char*)treedict_search(tdict, k2), "val2") == 0);
//     assert(treedict_getsize(tdict) == 2);
//     treedict_clear(tdict);
//     assert(treedict_search(tdict, k2) == NULL);
//     assert(treedict_getsize(tdict) == 0);
//     treedict_destroy(tdict);

//     HashMap* map = hashmap_create(strdestroy_pseudo, strcompar, naive_hash, 2);
//     hashmap_put(map, k1, v1);
//     assert( strcmp((char*)hashmap_search(map, k1), "val1") == 0);
//     hashmap_put(map, k2, v2);
//     hashmap_put(map, k2, v2);
//     assert(strcmp((char*)hashmap_search(map, k2), "val2") == 0);
//     assert(hashmap_getsize(map) == 2);
//     hashmap_destroy(map);
// }

int main(int argc, char** argv) {
    int len_thres = 4;
    int freq_thres = 2;
    int n_vocab = 2000;
    char* filename = "test.txt";


    if (argc == 4) {
        assert(atoi(argv[1]) > 0 && atoi(argv[2]) > 0 && atoi(argv[3]) >0);
        len_thres = atoi(argv[1]);
        freq_thres = atoi(argv[2]);
        n_vocab = atoi(argv[3]);
    } else if (argc == 5) {
        assert(atoi(argv[1]) > 0 && atoi(argv[2]) > 0);
        len_thres = atoi(argv[1]);
        freq_thres = atoi(argv[2]);
        n_vocab = atoi(argv[3]);
        filename = argv[4];
    }
    // verifying the file is readable
    FILE * pf = fopen(filename, "r");
    int errnum;            
    if (pf == NULL) {
        errnum = errno;
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
        fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
        exit(-1);
    } 
    fclose(pf);

    char *en_slow = "coded_files/slow_encoded.txt";
    char *de_slow = "coded_files/slow_decoded.txt";
    char *en_tree = "coded_files/tree_encoded.txt";
    char *de_tree = "coded_files/tree_decoded.txt";
    char *en_hash_n = "coded_files/hash_encoded_naive.txt";
    char *de_hash_n = "coded_files/hash_decoded_naive.txt";
    char *en_hash_b = "coded_files/hash_encoded_bernstein.txt";
    char *de_hash_b = "coded_files/hash_decoded_bernstein.txt";
    char *en_hash_fnv = "coded_files/hash_encoded_FNV.txt";
    char *de_hash_fnv = "coded_files/hash_decoded_FNV.txt";
    double t_en_slow, t_de_slow, t_en_tree, t_de_tree, t_en_hash_n, t_de_hash_n,t_en_hash_b, t_de_hash_b, t_en_hash_fnv, t_de_hash_fnv;
    double ratio_slow, ratio_tree, ratio_hash_n, ratio_hash_b, ratio_hash_fnv;
    // running slow compressor
    StartTimer();
    SlowCompressor* slowcomp = slowcomp_init(filename, len_thres, freq_thres);
    slowcomp_encode(filename, slowcomp, en_slow);
    t_en_slow = GetTimer() / 1000.0;
    StartTimer();
    slowcomp_decode(en_slow, slowcomp, de_slow);
    t_de_slow = GetTimer() / 1000.0;
    slowcomp_destroy(slowcomp);

    // running tree compressor
    StartTimer();
    TreeCompressor* treecomp = treecomp_init(filename, len_thres, freq_thres);
    treecomp_encode(filename, treecomp, en_tree);
    t_en_tree = GetTimer() / 1000.0;
    StartTimer();
    treecomp_decode(en_tree, treecomp, de_tree);
    t_de_tree = GetTimer() / 1000.0;
    treecomp_destroy(treecomp);

    // running hash compressor with naive hash
    StartTimer();
    HashCompressor* hashcomp_naive = hashcomp_init(filename, n_vocab, len_thres, freq_thres, naive_hash);
    hashcomp_encode(filename, hashcomp_naive, en_hash_n);
    t_en_hash_n = GetTimer() / 1000.0;
    StartTimer();
    hashcomp_decode(en_hash_n, hashcomp_naive, de_hash_n);
    t_de_hash_n = GetTimer() / 1000.0;
    hashcomp_destroy(hashcomp_naive);

    // running hash compressor with bernstein hash
    StartTimer();
    HashCompressor* hashcomp_bern = hashcomp_init(filename, n_vocab, len_thres, freq_thres, bernstein_hash);
    hashcomp_encode(filename, hashcomp_bern, en_hash_b);
    t_en_hash_b = GetTimer() / 1000.0;
    StartTimer();
    hashcomp_decode(en_hash_b, hashcomp_bern, de_hash_b);
    t_de_hash_b = GetTimer() / 1000.0;
    hashcomp_destroy(hashcomp_bern);

    // running hash compressor with fnv hash
    StartTimer();
    HashCompressor* hashcomp_fnv = hashcomp_init(filename, n_vocab, len_thres, freq_thres, FNV_hash);
    hashcomp_encode(filename, hashcomp_fnv, en_hash_fnv);
    t_en_hash_fnv = GetTimer() / 1000.0;
    StartTimer();
    hashcomp_decode(en_hash_fnv, hashcomp_fnv, de_hash_fnv);
    t_de_hash_fnv = GetTimer() / 1000.0;
    hashcomp_destroy(hashcomp_fnv);

    long int og_size = findSize(filename);
    ratio_hash_n = (double) og_size / (double) findSize(en_hash_n);
    ratio_hash_b = (double) og_size / (double) findSize(en_hash_b);
    ratio_hash_fnv = (double) og_size / (double) findSize(en_hash_fnv);
    ratio_slow = (double) og_size / (double) findSize(en_slow);
    ratio_tree = (double) og_size / (double) findSize(en_tree);

    printf("\nFinish compression, the encoded and decoded files can be found in the directory ./coded_files for verification.\n\n");

    printf("------------------------------------------------------------------------\n");
    printf("\t\tCompression Performance Comparison\t\n");
    printf("------------------------------------------------------------------------\n"
        "Compressing %s, word length threshold: %d, frequency threshold: %d.\n", filename, len_thres, freq_thres);
    printf("------------------------------------------------------------------------\n"
        " Compression Scheme | Encoding Time | Decoding Time | Compression Ratio \n");
    printf("------------------------------------------------------------------------\n"
        "    Naive Solution  |    %3.5f    |    %3.5f    |   %0.3f:1 \n", t_en_slow, t_de_slow, ratio_slow);
    printf("------------------------------------------------------------------------\n"
        "     Tree-based     |    %3.5f    |    %3.5f    |   %0.3f:1 \n", t_en_tree, t_de_tree, ratio_tree);
    printf("------------------------------------------------------------------------\n"
        "     Naive Hash     |    %3.5f    |    %3.5f    |   %0.3f:1 \n", t_en_hash_n, t_de_hash_n, ratio_hash_n);
    printf("------------------------------------------------------------------------\n"
        "   Bernstein Hash   |    %3.5f    |    %3.5f    |   %0.3f:1 \n", t_en_hash_b, t_de_hash_b, ratio_hash_b);
    printf("------------------------------------------------------------------------\n"
        "      FNV Hash      |    %3.5f    |    %3.5f    |   %0.3f:1 \n", t_en_hash_fnv, t_de_hash_fnv, ratio_hash_fnv);
    printf("------------------------------------------------------------------------\n");
    printf("PS: Number of hash buckets is set to %d. \n", n_vocab);
    printf("    For optimal performance of hash-based solutions, \n" 
    "    this number should be equal to the number of unique words in the corpus\n");
    printf("------------------------------------------------------------------------\n");
}