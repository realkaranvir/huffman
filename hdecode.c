#include <unistd.h>
#include "hdecode.h"
#include "huffman.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define HUNK 4096
#define BYTE_SIZE 8
#define MAX_ASCII 256
#define TEST_MASK 128
#define HEADER_SECTION 5

extern int optind;
extern char *optarg;


int main(int argc, char *argv[]) {
    /* hdecode recreates the huffman tree using
    * the header file and then traverses
    * the input file to decode it and output it
    * to a specified output file or stdout */ 

    int input = STDIN_FILENO;
    int output = STDOUT_FILENO;
    Node *tree = NULL;
    int *frequencyTable = NULL;
    int numEntries = 0;
    int numChars = 0;

    getopt(argc, argv, "");

    if(optind < argc && strcmp(argv[optind], "-") != 0) {
        /* if input argument is passed */
        if((input = open(argv[optind], O_RDONLY)) < 0) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
        }
    }

    if(optind == argc - 2) {
        /* if 2 arguments passed */
            if((output = open(argv[optind + 1], 
        O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
                perror("Error opening file");
                if(input != STDIN_FILENO) {
                    close(input);
                }
                exit(EXIT_FAILURE);
            }
    }

    frequencyTable = read_header(input, &numChars);
    if(frequencyTable) {
        /* if not an empty file */
        tree = create_tree(frequencyTable, &numEntries);
        decode(input, output, tree, numChars);
    }
    close(input);
    close(output);
    free_all(tree);

    return 0;
}


void decode(int input, int output, Node *tree, int numChars) {
    /* decodes the body of the encoded file by iterating
     * through input's binary data and using it to traverse
     * the huffman tree. Then adding a char to the buffer
     * when a leaf node is reached. */
    
    int inBufTracker = 0;
    int bitCount = 0;
    unsigned char currentBit = 0;
    unsigned char testMask = TEST_MASK;
    int bytesRead = 0;
    char inputBuffer[HUNK] = {0};
    char outputBuffer[HUNK] = {0};
    Node *currentNode = tree;
    int outBufTracker = 0;
    int charsWritten = 0;
    int i = 0;

    while((bytesRead = read(input, inputBuffer, HUNK)) > 0) {
        bitCount = 0;
        inBufTracker = 0;
        testMask = TEST_MASK;
        while(inBufTracker < bytesRead) {
                    
            if(outBufTracker == HUNK) {
                write(output, outputBuffer, HUNK);
                memset(outputBuffer, 0, HUNK);
                outBufTracker = 0;
            }
            currentBit = (inputBuffer[inBufTracker]) & testMask;
            if (currentBit) {
                currentNode = currentNode->right;
            } else {
                currentNode = currentNode->left;
            }

            testMask >>= 1;
            currentBit = 0;
            bitCount++;
            
            if(bitCount == BYTE_SIZE) {
                testMask = TEST_MASK;
                inBufTracker++;
                bitCount = 0;
            }

            if(currentNode->left == NULL && 
               currentNode->right == NULL) {
                /* if a leaf node is found */
                outputBuffer[outBufTracker] = 
                (char)(currentNode->character);
            
                outBufTracker++;
                currentNode = tree;
                charsWritten++;
            }
            

            if(charsWritten == numChars) {
                write(output, outputBuffer, outBufTracker);
                return;
            }




        

        }

    }
    for(i = 0; i < tree->freq; i++) {
        /* if only one character in file loop and print
         * using header data */
        outputBuffer[outBufTracker] = (char)(tree->character);
        outBufTracker++;
        if(outBufTracker == HUNK || i == tree->freq - 1){
            write(output, outputBuffer, outBufTracker);
            memset(outputBuffer, 0, HUNK);
            outBufTracker = 0;
        }
    }

}












int *read_header(int fd, int *numChars) {
    /* reads the header and uses that to reconstruct
     * the frequency table */
    
    int bytesRead = 0;
    int i = 0;
    int index = 0;
    unsigned char buffer[HUNK] = {0};
    int count = 0;
    int freqCount = 0;

    int *frequencyTable = (int*)calloc(MAX_ASCII, (sizeof(int)));
    if((bytesRead = read(fd, buffer, 1)) == 0) {
        return NULL;
    }
    freqCount = buffer[0];
    while(i < freqCount + 1) {
        /* uses first byte for character and following
         * 4 bytes for the count. loops through the number
         * of unique chars which is found in byte 1 of header */
        bytesRead = read(fd, buffer, HEADER_SECTION);
        index = buffer[0];
        memcpy(&count, &(buffer[1]), 4);
        count = ntohl(count);
        frequencyTable[(unsigned char)index] = count;
        *numChars += count;
        i++;
    }


    return frequencyTable;
}
