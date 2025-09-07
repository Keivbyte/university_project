#include "huffman.h"

/*
 * @func  _generate_code
 * @desc  helper function for code generation
 */
void _generate_code(Node* node, char* code, int depth) {
    if (!node) return;

    if (node->isleaf) {
        code[depth] = '\0';
        strcpy(codes[node->symbol].code, code);
        codes[node->symbol].symbol = node->symbol;
    } else {
        code[depth] = '0';
        _generate_code(node->left, code, depth + 1);
        code[depth] = '1';
        _generate_code(node->right, code, depth + 1);
    }
}

/*
 * @func  generate_huffman_codes
 * @desc  function for recursive code generation
 */
void generate_huffman_codes(Node* root) {
    char code[MAX_CODE_LEN];
    _generate_code(root, code, 0);
}
