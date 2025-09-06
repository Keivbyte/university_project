#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 255
#define MAX_LEN 7

/*
 * Node id Huffman tree and priority queue
 */
struct tnode{
  struct tnode *left; /* use in tree */
  struct tnode *right; /* use in tree */
  struct tnode *parent; /* use in tree */
  struct tnode *next; /* use in list */
  float freq;
  int isleaf;
  char symbol;
};

/* global variables */
char code[MAX_SYMBOLS][MAX_LEN];
struct tnode *root = NULL;
struct tnode *qhead = NULL; /* Head of priority queue (in ascending order of frequency) */

/*
 * @func talloc
 * @desc Allocates and initializes a new node
 */
struct tnode* talloc(int symbol, float freq){
  struct tnode *p = (struct tnode*)malloc(sizeof(struct tnode));
  if(p != NULL){
    p->left = p->right = p->parent = p->next = NULL;
    p->symbol = symbol;
    p->freq = freq;
    p->isleaf = 1;
  }
  return p;
}

/*
 * @func pq_display
 * @desc Displays the current state of the queue (for debugging)
 */
void pq_display(struct tnode *head){
  printf("[LIST] : ");
  for(struct tnode *p=head; p != NULL; p=p->next){
    printf("(%c,%.2f) ", p->symbol, p->freq);
  }
  putchar('\n');
}

/*
 * @func pq_insert
 * @desc Inserts a node into a priority queue (in ascending freq order)
 */
void pq_insert(struct tnode *p){
  struct tnode *curr = qhead;
  struct tnode *prev = NULL;

  printf("[INSERTING] : %c, %.2f\n", p->symbol, p->freq);

  while( curr != NULL && curr->freq < p->freq ){
    prev = curr;
    curr = curr->next;
  }

  if(prev == NULL){
    /* insert at the beginning */
    p->next = qhead;
    qhead = p;
  } else {
    /* insert between prev and curr */
    p->next = curr;
    prev->next = p;
  }
}

/*
 * @func pq_pop
 * @desc Removes and returns the first element (lowest frequency)
 */
struct tnode* pq_pop(){
  if (qhead == NULL) return NULL;
  struct tnode *p = qhead;
  qhead = qhead->next;
  printf("[POPPED] : %c, %.2f\n", p->symbol, p->freq);
  return p;
}

/*
 * @func  generate_code
 * @desc  Recursively traverses the tree and builds codes from the root to the leaves
 *        The code is written to the code[symbol] array by backtracking through parent
 */
void generate_code(struct tnode *node, int depth){
  if (node == NULL) return;

  if(node->isleaf){
    char symbol = node->symbol;
    code[symbol][depth] = '\0';

    struct tnode *current = node;
    struct tnode *parent = current->parent;
    int pos = depth - 1;

    while (parent != NULL){
      if (parent->left == current)
        code[symbol][pos] = '0';
      else
        code[symbol][pos] = '1';
      current = parent;
      parent = current->parent;
      pos--;
    }

     printf("[BUILT CODE] : %c, %s\n", symbol, code[symbol]);
  } else {
    generate_code(node->left, depth + 1);
    generate_code(node->right, depth + 1);
  }
}

/*
 * @func dump_code
 * @desc outputs a code table to a file or to the screen
 */
void dump_code(FILE *fp) {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (code[i][0] != '\0') {  // если код не пуст
            fprintf(fp, "%c %s\n", i, code[i]);
        }
    }
}

/*
 * @func encode
 * @desc replaces each character of a string with its Huffman code
 */
void encode(char *str, FILE *fout) {
    while (*str) {
        fprintf(fout, "%s", code[(unsigned char)*str]);
        str++;
    }
}

/*
 * @func free_tree
 * @desc recursively frees tree memory
 */
void free_tree(struct tnode* node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

/*
 * @func print_tree
 * @desc Visualization of a tree in pseudographics
 */
void print_tree(struct tnode *node, int depth, int is_last) {
    if (node == NULL) return;

    // Не выводим префикс для корня
    for (int i = 0; i < depth; i++) {
        if (i == 0) continue; // корень — без отступа
        printf((i == depth - 1) ? (is_last ? "   " : "|  ") : "|  ");
    }

    if (depth > 0) {
        printf(is_last ? "`-- " : "|-- ");
    }

    if (node->isleaf) {
        printf("'%c' (f=%.2f)\n", node->symbol, node->freq);
    } else {
        printf("* (f=%.2f)\n", node->freq);
    }

    // Рекурсивно выводим детей
    if (node->left || node->right) {
        int child_count = 0;
        int total_children = (node->left ? 1 : 0) + (node->right ? 1 : 0);
        int child_index = 0;

        if (node->left) {
            int is_last_child = (child_index == total_children - 1);
            print_tree(node->left, depth + 1, is_last_child);
            child_index++;
        }
        if (node->right) {
            int is_last_child = (child_index == total_children - 1);
            print_tree(node->right, depth + 1, is_last_child);
            child_index++;
        }
    }
}

/*
 * @func print_tree
 * @desc show connections (parent ←→ children)
 */
void print_node_relations(struct tnode *node) {
    if (node == NULL) return;

    // Информация об узле
    if (node->isleaf) {
        printf("  '%c' (f=%.2f)", node->symbol, node->freq);
    } else {
        printf("  * (f=%.2f)", node->freq);
    }

    // Родитель
    if (node->parent) {
        printf(" <-- ");
        if (node->parent->isleaf)
            printf("'%c'", node->parent->symbol);
        else
            printf("* (f=%.2f)", node->parent->freq);
    } else {
        printf(" <-- [ROOT]");
    }

    // Дети
    if (node->left || node->right) {
        printf(" --> ");
        if (node->left)  printf("L:'%c'", node->left->symbol);
        if (node->left && node->right) printf(" + ");
        if (node->right) printf("R:'%c'", node->right->symbol);
    }
    putchar('\n');

    // Рекурсия для внутренних узлов
    if (!node->isleaf) {
        print_node_relations(node->left);
        print_node_relations(node->right);
    }
}

int main() {
    struct tnode *p = NULL;
    struct tnode *lc, *rc;
    float freq[] = {0.01, 0.04, 0.05, 0.11, 0.19, 0.20, 0.4};
    int NCHAR = 7;
    int i;

    const char *CODE_FILE = "code.txt";
    const char *OUT_FILE = "encoded.txt";
    FILE *fout;

    memset(code, 0, sizeof(code));

    puts("=== [TEST] priority queues ===");
    pq_insert(talloc('a', 0.1));
    pq_insert(talloc('b', 0.2));
    pq_insert(talloc('c', 0.15));
    pq_display(qhead);

    puts("making sure it pops in the right order");
    while ((p = pq_pop())) {
        free(p);
    }

    // --- Main part: building a Huffman tree ---
    qhead = NULL;

    // Adding characters to the queue
    for (i = 0; i < NCHAR; i++) {
        pq_insert(talloc('a' + i, freq[i]));
    }

    // Building a tree: N-1 unions
    for (i = 0; i < NCHAR - 1; i++) {
        lc = pq_pop();  // lowest frequency
        rc = pq_pop();  // second smallest

        // Create a parent
        p = talloc(0, lc->freq + rc->freq);
        p->left = lc;   lc->parent = p;
        p->right = rc;  rc->parent = p;
        p->isleaf = 0;  // internal node

        pq_insert(p);  // put it back into the queue
    }

    // The root of the tree is the last remaining node
    root = pq_pop();

    puts("\n=== GENERATE CODES ===");
    generate_code(root, 0);

    printf("\n\n[TREE] \n\n");
    print_tree(root, 0, "");
    printf("\n\n[REALATIONS] \n\n");
    print_node_relations(root);

    puts("\n=== Table of codes ===");
    fout = fopen(CODE_FILE, "w");
    dump_code(stdout);
    dump_code(fout);
    fclose(fout);

    puts("\n=== String encoding ===");
    printf("original: abba cafe bad\n");
    fout = fopen(OUT_FILE, "w");
    encode("abba cafe bad", stdout);
    putchar('\n');
    encode("abba cafe bad", fout);
    fclose(fout);

    free_tree(root);
    root = NULL;

    puts("\nPress Enter to exit...");
    getchar();

    return 0;
}
