#include "huffman.h"


/* allocate memory for variables */
CodeEntry codes[MAX_SYMBOLS];
Node* root = NULL;

/*
 * @func  compress
 * @desc  Compresses a file using Huffman coding algorithm. Reads input file,
 *        builds frequency table, constructs Huffman tree, generates codes,
 *        and writes compressed data to output file.
 * @param infile:  Path to input file to compress
 * @param outfile: Path to output compressed file
 */
void compress(const char* infile, const char* outfile) {
    FILE *in = fopen(infile, "r");
    FILE *out = fopen(outfile, "wb");
    if (!in || !out) {
        perror("[FILE ERROR]");
        exit(1);
    }

    double freq[MAX_SYMBOLS] = {0};
    build_frequency_table(in, freq);
    root = build_huffman_tree(freq);
    generate_huffman_codes(root);

    encode_file(in, out);

    fclose(in);
    fclose(out);
    free_huffman_tree(root);
    printf("[COMPRESSED] : %s → %s\n", infile, outfile);
}

/*
 * @func  decompress
 * @desc  Decompresses a file compressed with Huffman coding. Reads compressed file,
 *        reconstructs Huffman tree from header, and decodes data to original form.
 * @param infile:  Path to compressed input file
 * @param outfile: Path to output decompressed file
 */
void decompress(const char* infile, const char* outfile) {
    FILE *in = fopen(infile, "rb");
    FILE *out = fopen(outfile, "w");
    if (!in || !out) {
        perror("[FILE ERROR]");
        exit(1);
    }

    decode_file(in, out);

    fclose(in);
    fclose(out);
    free_huffman_tree(root);
    printf("[DECOMPRESSED] : %s → %s\n", infile, outfile);
}

/*
 * @func  free_huffman_tree
 * @desc  Recursively deallocates memory for the entire Huffman tree.
 *        Performs a post-order traversal to free all nodes starting from
 *        the leaves back to the root, ensuring safe memory management.
 *        This function should be called after compression/decompression
 *        to prevent memory leaks.
 * @param node: Root node of the Huffman tree to be freed
 */
void free_huffman_tree(Node* node) {
    if (node == NULL) return;
    free_huffman_tree(node->left);
    free_huffman_tree(node->right);
    free(node);
}




