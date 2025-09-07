#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * @func main
 * @desc Main function of the Huffman coding utility.
 *       This program compresses and decompresses text files
 *       using the Huffman coding algorithm for efficient
 *       lossless data compression.
 *
 * How it works:
 *
 * 1. File Compression:
 *    ./huff -c input.txt output.huff
 *
 *    - Reads input.txt character by character
 *    - Builds frequency table of each character's occurrence
 *    - Constructs a Huffman binary tree from frequencies
 *      (less frequent characters get longer codes)
 *    - Generates optimal prefix codes for each character
 *    - Writes code table header followed by compressed bitstream
 *    - Achieves compression by replacing frequent characters
 *      with shorter bit sequences
 *
 * 2. File Decompression:
 *    ./huff -d input.huff output.txt
 *
 *    - Reads the code table from .huff file header
 *    - Reconstructs the Huffman tree structure
 *    - Reads compressed bitstream and traverses the tree:
 *        0 bit → move to left child
 *        1 bit → move to right child
 *    - When leaf node is reached, writes the corresponding character
 *    - Repeats until end of file, perfectly reconstructing original
 *
 * .huff File Format:
 *   [Header]: Character-code pairs in format: [char][code-string],
 *             terminated by null byte (0x00)
 *   [Data]:   Bit-packed Huffman codes (may include padding bits
 *             at the end to complete final byte)
 *
 * Technical Details:
 *   - Supports all 256 ASCII characters (0-255)
 *   - Uses static Huffman coding (same tree for entire file)
 *   - Handles end-of-file padding transparently
 *   - Requires read access for input files
 *   - Overwrites output files if they exist
 *
 * Command Line Arguments:
 *   argc  - argument count (must be exactly 4)
 *   argv  - argument vector:
 *           argv[0] - program executable name
 *           argv[1] - operation mode: "-c" (compress) or "-d" (decompress)
 *           argv[2] - input file path
 *           argv[3] - output file path
 *
 * Return Codes:
 *   0 - Success
 *   1 - Invalid arguments or usage error
 *   2 - File I/O error (check perror output)
 */
int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "[USAGE]: %s -c|-d input output\n", argv[0]);
        fprintf(stderr, "  Example:\n");
        fprintf(stderr, "    %s -c document.txt document.huff  # Сжать\n", argv[0]);
        fprintf(stderr, "    %s -d document.huff document.txt  # Распаковать\n", argv[0]);
        return 1;
    }

    const char* mode = argv[1];
    const char* input = argv[2];
    const char* output = argv[3];

    // Обработка режима
    if (strcmp(mode, "-c") == 0) {
        printf("[COMMPRESSING] '%s' -> '%s'\n", input, output);
        compress(input, output);
    } else if (strcmp(mode, "-d") == 0) {
        printf("[DECOMPRESSING] '%s' -> '%s'\n", input, output);
        decompress(input, output);
    } else {
        fprintf(stderr, "[ERROR] : Mode must be '-c' (compress) or '-d' (decompress)\n");
        return 1;
    }

    return 0;
}
