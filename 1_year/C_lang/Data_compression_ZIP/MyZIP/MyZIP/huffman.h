#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 256
#define MAX_CODE_LEN 32

/* tree */
typedef struct Node {
    unsigned char symbol;
    double freq;
    int isleaf;
    struct Node *left, *right, *parent;
} Node;

/* symbol code */
typedef struct {
    unsigned char symbol;
    char code[MAX_CODE_LEN];
} CodeEntry;

/* global vvalues */
extern CodeEntry codes[MAX_SYMBOLS];
extern Node *root;

/* function */
Node* new_node(unsigned char symbol, double freq, int isleaf);
void build_frequency_table(FILE *fp, double freq[]);
Node* build_huffman_tree(double freq[]);
void generate_huffman_codes(Node* root);
void free_huffman_tree(Node* node);

void encode_file(FILE* input, FILE* output);
void decode_file(FILE* input, FILE* output);

void compress(const char* infile, const char* outfile);
void decompress(const char* infile, const char* outfile);


#endif
