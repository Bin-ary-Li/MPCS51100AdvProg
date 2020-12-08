# Homework 5: Compression, Three Ways

# Name: Bin Li

# Instruction 

Compile the code with `make compressor`. The program has 4 argument, `./compressor [word length threshold] [word frequency threshold] [number of hash buckets] [file to compress]`. 

Take the example of `./compressor 6 2 346 ./test_document.txt`.

* `word length threshold` = `6`, means that the compressor will not compress any word less than the length of 6.
* `word frequency threshold` = `2`, means that compressor will only compress word when it appears at least twice in the file.
* `number of hash buckets` = `346`, determine the number of hash buckets used in the hashing compressors. 
* `file to compress` = `./test_document.txt` is the file to be compressed.

After running the program, the compressed and decompressed files will be placed in the `coded_files` directory. A compression performance comparison will outputed at the end of the program. 

Two helper Python script files `count_vocab.py` and `verify.py` are also provided. 

* `count_vocab.py` takes a filename as argument and prints out the number of unique words in that document. This can be used to help specify the number of bucket to allocate in the hash solution.
* `verify.py` takes a filename as argument and verify that file with the files in the `coded_files` directory. This is used for verifying the output of the compressor and the original file. After finish running the program, run this script with the uncompressed original filename as the argument for the verification test.

# Background

Imagine having a large document with many repeated long strings. A simple way to compress the file is to replace frequently occurring words with a simple abbreviation. For example in the text:

>But good morning! Good morning to ye and thou! I’d say to all my patients, because I was the worst of the hypocrites, of all the hypocrites, the cruel and phony hypocrites, I was the very worst.

If we create a simple rule, e.g., that all words with at least four characters and one or more occurrence are encoded, then we might encode according the following abbreviation table:

| word        | occurrences | byte code |
|-------------|-------------|-----------|
| hyprocrites |      3      |     1     |
| morning     |      2      |     2     |
| good        |      2      |     3     |
| worst       |      2      |     4     |

and thus the encoded document becomes:

>But 3 2! 3 2 to ye and thou! I’d say to all my patients, because I was the 4 of the 1, of all the 1, the cruel and phony 1, I was the very 4.

This project implements three solutions for this scheme of text compression: 

 * A naive solution with O(N^2) time complexity.
 * a binary tree solution. 
 * a hashmap solution that can work with any hash function. This project tests the following:

```
unsigned int naive_hash(char *word, int nbins)
{
    unsigned int h = 0;
    int c;
    while(c = *word++)
        h += c;
    return h % nbins;
}
```
```
unsigned int bernstein_hash(char *word, int nbins)
{
    unsigned int h = 5381;
    int c;
    while(c = *word++)
        h = 33 * h + c;
    return h % nbins;
}
```
```
unsigned int FNV_hash(char *word, int nbins)
{
    unsigned long h = 14695981039346656037lu;
    char c;
    while(c = *word++)
    {
        h = h * 1099511628211lu;
        h = h ^ c;
    }
    return h % nbins;
}
```

# Design Interface

The three solutions are implemented with similar interfaces, `slowcomp.h`, `treecomp.h`, `hashcomp.h` respectively. 

Extra care was taken to ensure good level of abstraction from the lower level realization. The interface only makes explicit four methods, initiation: `[compressor]_init`, encoding: `[compressor]_encode`, decoding: `[compressor]_decode`, and destroyer `[compressor]_destroy`.

# Solutions

## Naive solution

The naive compressor is implemented by a linked-list. During the initiation of the compressor, unique words are put into the list and occurances are documented. The encoded number of the word corresponds to the index of the word in the linked list. 

**Pros**

* Easy to implement.

**Cons**

* Can be really slow.

## Tree solution

The tree compressor is implemented using two binary-search tree, one uses stored words as key and the encoding numbers as associated item (i.e., a word2id tree), and the other reverses it (a id2word tree). IDs are assigned to each unique words as they are added to the tree compressor during the initiation step to create a word2id tree. At the end of the initiation, a id2word tree is created based on the word2id tree. 

The encoding id is found by traversing the word2id tree, and to decode id using id2word tree. 

**Pros**

* Can be a lot faster than the naive solution.

**Cons**

* Constructing two trees can be costly in term of time and memory complexity.
* Not guarantee balancing (in this implementation, can be remedied by using self-balancing variation like R-B tree). 

## Hash solution

The hash compressor is implemented using a hashmap with a specified hash function. The hashing collision is resolved by chaining each bucket with a linked list. 

The hash id of each word string is calculated and forms part of the encoded number. The other part of the encoded number is the index of the word string stored in the chaining linked list. In this way, the encoded number is formated as `[hashID]^[listIndex]`, with `^` functioning as a deliminator. For example `245^1` denotes the word in the 245th bucket and the second item of that chaining list. 

**Pros**

* Can be faster than the naive solution on decoding.
* Can be a lot faster than the naive solution and tree solution on encoding, because of the constant time accessing when storing word and checking for duplicate.

**Cons**

* Compression ratio might be lower than the other two solution, due to the fact that the current solution requires extra bit to store the chaining list index `^[listindex]`, which needs at least 2 more bits than the other two solutions.
* Optimal performance requires specifying the number of unique words to be stored in advance. 

# Compression Performance Comparison 

This is obtained by using the program to compress the small and big files provided. 

```
Finish compression, the encoded and decoded files can be found in the directory ./coded_files for verification.

------------------------------------------------------------------------
                Compression Performance Comparison
------------------------------------------------------------------------
Compressing ./test_document.txt, word length threshold: 6, frequency threshold: 2.
------------------------------------------------------------------------
 Compression Scheme | Encoding Time | Decoding Time | Compression Ratio 
------------------------------------------------------------------------
    Naive Solution  |    0.39295    |    0.13678    |   1.687:1 
------------------------------------------------------------------------
     Tree-based     |    0.21699    |    0.11931    |   1.687:1 
------------------------------------------------------------------------
     Naive Hash     |    0.20830    |    0.10072    |   1.274:1 
------------------------------------------------------------------------
   Bernstein Hash   |    0.20392    |    0.10022    |   1.277:1 
------------------------------------------------------------------------
      FNV Hash      |    0.21771    |    0.10041    |   1.296:1 
------------------------------------------------------------------------
PS: Number of hash bucket is set to 346. 
    For optimal performance of hash-based solutions, 
    this number should be equal to the number of unique words in the corpus
------------------------------------------------------------------------
```

```
Finish compression, the encoded and decoded files can be found in the directory ./coded_files for verification.

------------------------------------------------------------------------
                Compression Performance Comparison
------------------------------------------------------------------------
Compressing ./test_document_big_paragraphs.txt, word length threshold: 6, frequency threshold: 2.
------------------------------------------------------------------------
 Compression Scheme | Encoding Time | Decoding Time | Compression Ratio 
------------------------------------------------------------------------
    Naive Solution  |    3.52312    |    1.37816    |   1.731:1 
------------------------------------------------------------------------
     Tree-based     |    1.91494    |    1.11467    |   1.731:1 
------------------------------------------------------------------------
     Naive Hash     |    1.86398    |    0.96523    |   1.308:1 
------------------------------------------------------------------------
   Bernstein Hash   |    1.90778    |    0.94541    |   1.330:1 
------------------------------------------------------------------------
      FNV Hash      |    1.99654    |    0.95159    |   1.315:1 
------------------------------------------------------------------------
PS: Number of hash bucket is set to 179. 
    For optimal performance of hash-based solutions, 
    this number should be equal to the number of unique words in the corpus
------------------------------------------------------------------------
```

# Limitations

The known limitations to this program are: 

* The decoded file is not exactly identical to the original file due to parsing. Particularly, the encoding/decoding is not case-sensitive. The decoded file will be cast to all lower case. Also extra new lines are stripped during the process, for example `word1.\n\n\nword2` will become `word1.\nword2`.

* More optimization can be done on the tree and hash solutions. The tree solution can be benefited from using self-balancing tree as underlying structure, and the hash solution can use some small performance tweak. 

* Current encoding/decoding scheme of the hash solution is not ideal for compression. Switching to open addressing should improve the ratio of compression, with the cost in encoding/decoding performance.
