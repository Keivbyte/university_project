#include "huffman.h"

/*
 * @func  decode_file
 * @desc  Decodes a file compressed using Huffman coding. Reads the Huffman code table
 *        from the input file header, reconstructs the Huffman tree, and then decodes
 *        the compressed data using the tree traversal algorithm.
 * @param input:  Compressed input file (contains code table + encoded data)
 * @param output: Output file for writing decoded data
 */
void decode_file(FILE* input, FILE* output) {
  root = new_node(0, 0, 0);
  if (root == NULL) {
    fprintf(stderr, "[ERROR] Cannot allocate root node\n");
    return;
  }

  int c;
  while ((c = fgetc(input)) != EOF && c != 0) {
    unsigned char symbol = (unsigned char)c;
    int len = fgetc(input);
    if (len <= 0 || len >= MAX_CODE_LEN) {
      fprintf(stderr, "[ERROR] Invalid code length: %d for symbol '%c'\n", len, symbol);
      return;
    }

    char code_str[MAX_CODE_LEN] = {0};
    for (int i = 0; i < len; i++) {
      code_str[i] = fgetc(input);
      if (code_str[i] != '0' && code_str[i] != '1') {
          fprintf(stderr, "[ERROR] Invalid character in code: '%c'\n", code_str[i]);
          return;
      }
    }

    Node* curr = root;
    for (int i = 0; i < len; i++) {
      if (code_str[i] == '0') {
        if (curr->left == NULL) {
          curr->left = new_node(0, 0, 0);
          if (curr->left == NULL) return;
        }
        curr = curr->left;
      } else if (code_str[i] == '1') {
        if (curr->right == NULL) {
          curr->right = new_node(0, 0, 0);
          if (curr->right == NULL) return;
        }
        curr = curr->right;
      }
    }
    curr->isleaf = 1;
    curr->symbol = symbol;
  }

    Node* curr = root;
    int byte;
    while ((byte = fgetc(input)) != EOF) {
        for (int i = 0; i < 8; i++) {
            int bit = (byte >> i) & 1;

            if (bit == 0) {
                if (curr->left == NULL) {
                    fprintf(stderr, "[ERROR] Invalid bit stream: no left child\n");
                    return;
                }
                curr = curr->left;
            } else {
                if (curr->right == NULL) {
                    fprintf(stderr, "[ERROR] Invalid bit stream: no right child\n");
                    return;
                }
                curr = curr->right;
            }

            if (curr->isleaf) {
                fputc(curr->symbol, output);
                curr = root;
            }
        }
    }
}
