#include "huffman.h"

/* Bit buffer for accumulating bits before writing a byte */
static unsigned char buffer = 0;
/* The counter of bits in the current buffer */
static int bit_count = 0;
static FILE *out_file;

/*
 * @func  write_bit
 * @desc  Writes a single bit to the buffer. When 8 bits (bytes) have accumulated,
 *        writes it to the output file and flushes the buffer.
 * @param bit:  the bit to write (0 or 1)
 */
void write_bit(int bit){
  buffer |= (bit << bit_count);
  bit_count++;
  if(bit_count == 8){
    fputc(buffer, out_file);
    buffer = 0;
    bit_count = 0;
  }
}

/*
 * @func   write_code
 * @desc   Writes a sequence of bits, represented by a string of '0' and '1',
 *         to the output file via a bit buffer.
 * @param code_str:  a string with the Huffman code (e.g. "0101")
 */
void write_code(const char* code_str) {
    for (int i = 0; code_str[i]; i++) {
        write_bit(code_str[i] == '1' ? 1 : 0);
    }
}

/*
 * @func  flush_buffer
 * @desc  Forces the remaining bits in the buffer to be written to the output file,
 *        padding the partial byte with zeros. Called at the end of encoding.
 */
void flush_buffer() {
    if (bit_count > 0) {
        fputc(buffer, out_file);
        buffer = 0;
        bit_count = 0;
    }
}

/*
 * @func  encode_file
 * @desc  The main encoding function. Writes a table of Huffman codes to the beginning of the file,
 *        then encodes the contents of the input file using the Huffman codes.
 * @param input:  the input file to compress
 * @param output: the output file to write the compressed data to
 */
void encode_file(FILE* input, FILE* output) {
  out_file = output;

  for (int i = 0; i < MAX_SYMBOLS; i++) {
    if (codes[i].code[0] != '\0') {
      fputc(i, output); // символ
      int len = strlen(codes[i].code);
      fputc(len, output);
      for (int j = 0; j < len; j++) {
          fputc(codes[i].code[j], output);
      }
    }
  }
  fputc(0, output);

  int c;
  while ((c = fgetc(input)) != EOF) {
      write_code(codes[c].code);
  }
  flush_buffer();
}
