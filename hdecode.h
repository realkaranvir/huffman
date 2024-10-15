#ifndef HDECODE
#define HDECODE
#include "huffman.h"
void decode(int input, int output, Node *tree, int numChars);

int *read_header(int fd, int *numChars);

#endif
