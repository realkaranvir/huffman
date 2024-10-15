#ifndef HUFFMAN_NODE
#define HUFFMAN_NODE

#include <stdio.h>

typedef struct Node {
	int freq;
	int character;
	struct Node *left;
	struct Node *right;
	struct Node *next;
} Node;

Node *create_tree(int *frequencyTable, int *numEntries);

Node *pop(Node **head);

void free_all(Node *node);

void print_all(Node *node);

char **create_codes(Node* node);

void create_codes_helper(Node *node, char *code, char **codes);

Node *list_add(Node *node, Node *newNode);

Node *create_list(int h[], int *numEntries);

Node *create_node(int freq, int character);

#endif
