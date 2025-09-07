#include "huffman.h"

/*
 * @func  build_frequency_table
 * @desc  Frequency counting
 */
void build_frequency_table(FILE *fp, double freq[]){
  int c;
  long total = 0;

  while( (c=fgetc(fp)) != EOF){
    freq[c]++;
    total++;
  }

  for(int i=0; i<MAX_SYMBOLS; i++){
    if(total > 0){
      freq[i] /= total;
    }
  }

  rewind(fp);
}

/*
 * @func  new_node
 * @desc  Create new node
 */
Node* new_node(unsigned char symbol, double freq, int isleaf){
  Node *node = (Node*)malloc(sizeof(Node));
  if(!node) return NULL;
  node->symbol = symbol;
  node->freq = freq;
  node->isleaf = isleaf;
  node->left = node->right = node->parent = NULL;
  return node;
}

/*
 * @func  build_huffman_tree
 * @desc  Tree building
 */
Node* build_huffman_tree(double freq[]){
  Node *nodes[MAX_SYMBOLS];
  int n = 0;

  for(int symbol=0; symbol<MAX_SYMBOLS; symbol++){
    if( freq[symbol] > 0){
      nodes[n++] = new_node(symbol, freq[symbol], 1);
    }
  }

  while (n > 1) {
    int i1 = 0, i2 = 1;
    if (nodes[i1]->freq > nodes[i2]->freq){
      int temp = i1;
      i1 = i2;
      i2 = temp;
    }
    for (int i = 2; i < n; i++) {
        if (nodes[i]->freq < nodes[i1]->freq) {
            i2 = i1;
            i1 = i;
        } else if (nodes[i]->freq < nodes[i2]->freq) {
            i2 = i;
        }
    }

    Node* parent = new_node(0, nodes[i1]->freq + nodes[i2]->freq, 0);
    parent->left = nodes[i1]; nodes[i1]->parent = parent;
    parent->right = nodes[i2]; nodes[i2]->parent = parent;

    nodes[i1] = parent;
          nodes[i2] = nodes[--n];
      }

      return nodes[0];
}
























