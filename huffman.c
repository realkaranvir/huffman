#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

#define MAX_ASCII 256

Node *create_tree(int *frequencyTable, int *num_entries) {
    /* creates the huffman tree based on the frequency
     * table. Also passes in num entries into create_list */    
    
    Node **head = NULL;
    Node *one = NULL;
    Node *two = NULL;
    Node *three = NULL;

    if((head = (Node**)malloc(sizeof(Node*))) == NULL) {
        perror("Failed to allocated memory");
        exit(EXIT_FAILURE);
    }

    *head = create_list(frequencyTable, num_entries);
    if (*head == NULL) {return NULL;}
    
    while((*head) && (*head)->next) {
        one = pop(head);
        two = pop(head);
        three = create_node(one->freq + two->freq, -1);
        three->left = one;
        three->right = two;
        *head = list_add(*head, three);
    }
    one = *head;
    free(head);
    return (one);
}

char **create_codes(Node* node) {
    /* creates the huffman codes based on a huffman
     * tree */
    char **codes = NULL;
    char* code = NULL;
    
    if((code = malloc(1)) == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    
    if((codes = calloc(MAX_ASCII, sizeof(char*))) == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }
    
    code[0] = '\0';
    create_codes_helper(node, code, codes);
    free(code);
    return codes;
}

void create_codes_helper(Node* node, char *code, char **codes) {
    /* recursive helper function for create_codes */
    
    if (node->left == NULL && node->right == NULL) {
        codes[node->character] = strdup(code);
        return;
    }
    if (node->left != NULL) {
        char *temp = malloc(strlen(code) + 2);
        if(temp == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        strcpy(temp, code);
        strcat(temp, "0");
        create_codes_helper(node->left, temp, codes);
        free(temp);
    }
    if (node->right != NULL) {
        char* temp = malloc(strlen(code) + 2);
        if(temp == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        strcpy(temp, code);
        strcat(temp, "1");
        create_codes_helper(node->right, temp, codes);
        free(temp);
    }
}

    
Node *pop(Node **head) {
    /* removes the front of the linked list
     * and then sets the new head as the
     * next element */
    Node* temp = NULL;
    temp = (*head);
    (*head) = (*head)->next;
    temp->next = NULL;
    return temp;
}

void free_all(Node* node) {
    /* frees every node in a tree */
    Node *currentNode = node;
    if (node) {
        node = node->left;
        free_all(node);
        node = currentNode->right;
        free_all(node);
    }
    free(currentNode);
}


void print_all(Node* node) {
    /* prints every leaf node in a tree.
     * useful for debugging */
    Node *temp = NULL;
    if (node) {
        if (node->left == NULL && node->right == NULL) {
            printf("Node1 freq: %d, char: %d\n",
            node->freq, node->character);
            return;
        }
        temp = node;
        node = node->left;
        print_all(node);
        node = temp->right;
        print_all(node);
    }
}

int node_compare(Node *nodeOne, Node *nodeTwo) {
    /* compares two nodes based on freq and
     * then character */
    if (nodeOne->freq < nodeTwo->freq || 
       (nodeOne->freq == nodeTwo->freq &&
       (nodeOne->character == -1 ||
        nodeOne->character < nodeTwo->character))) {
        return 1;
    }
    return 0;
}



Node *list_add(Node *node, Node* newNode) {
    /* adds a new node to the ordered linked list */
    Node *prevNode = NULL;
    Node *head = node;
    int compVal = 0;
    if (head == NULL) {
        return newNode;
    }
    compVal = node_compare(newNode, node);
    if (compVal) { 
        /*if freq < first node or combo node*/
        head = newNode;
        head->next = node;
        return head;
    }
    
    while (node) { /*find node that has greater than or equal freq*/
        compVal = node_compare(newNode, node);
        if (compVal) {
            prevNode->next = newNode;
            newNode->next = node;
            return head;
        }
            
                
        prevNode = node;
        node = node->next;
    }
    /*if node has higher freq than all others*/
    prevNode->next = newNode;
    return head;
}

Node *create_list(int h[], int *numEntries) {
    /* creates an ordered linked list based on
     * the frequency table */
    int i = 0;
    Node *head = NULL;
    Node *temp = NULL;
    for (i = 0; i < MAX_ASCII; i++) {
        if (h[i] != 0) {
            (*numEntries)++;
            temp = create_node(h[i], i);
            head = list_add(head, temp);
    
        }
    }
    return head;
}

Node *create_node(int freq, int character) {
    /* creates a node */
    Node *node = (Node*)malloc(sizeof(Node));
    
    if(node == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    node->freq = freq;
    node->character = character;
    node->next = NULL;
    node->left = NULL;
    node->right = NULL;
    return node;
}   





