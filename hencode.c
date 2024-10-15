#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <arpa/inet.h>

#include "huffman.h"
#include "hencode.h"
#define HUNK 4096
#define BYTE_SIZE 8
#define MAX_ASCII 256




extern char *optarg;
extern int optind;

int main(int argc, char *argv[]) {
    /* hencode encodes a file using a huffman tree to
    * create huffman codes and then writing the binary
    * versions of those to the output file */ 

    int input = 0;
    int output = STDOUT_FILENO;
    Node *tree = NULL;
    char **codes = NULL;
    int *frequencyTable = NULL;
    int numEntries = 0;
    off_t offset;

    getopt(argc, argv, "");

    if ((input = open(argv[optind], O_RDONLY)) < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    if (optind == argc - 2) { /* if 2 arguments are passed */
        if((output = open(argv[optind + 1], 
            O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
            perror("Error opening file");
            close(input);
            exit(EXIT_FAILURE);
        }
    }
    
    
    frequencyTable = fill_frequency_table(input);   
    tree = create_tree(frequencyTable, &numEntries);
    if(tree) { /* if file isn't empty */
        codes = create_codes(tree);

        write_header(frequencyTable, output, &numEntries);
    
        if((offset = lseek(input, 0, SEEK_SET)) == (off_t)-1) {
            perror("lseek");
            close(input);
            exit(EXIT_FAILURE);
        }
        write_file(input, output, codes);   
    }
    
    close(input);
    close(output);
    free_all(tree);
    free_codes(codes);
    free(frequencyTable);

    return 0;
}

void write_file(int input, int output, char **codes) {
    /* writes the body of the encoded file
     * after the header by iterating through
     * the input file and outputing the binary
     * codes */
    
    char inputBuffer[HUNK] = {0};
    char outputBuffer[HUNK] = {0};
    char* code = NULL;
    int bitCount = 0;
    int bytesRead = 0;
    int i = 0;
    int j = 0;
    int bufferTracker = 0;
    while((bytesRead = read(input, inputBuffer, HUNK)) > 0) {

        for(i = 0; i < bytesRead; i++) {
            code = codes[((unsigned char)inputBuffer[i])];
            j = 0;
            while(code[j]) {
            /* loop through the huffman code */
                if(bitCount == BYTE_SIZE) {
                    bufferTracker++;
                    bitCount = 0;
                }
            
                if(bufferTracker == HUNK) {
                    write(output, outputBuffer, 
                          bufferTracker);
                    bufferTracker = 0;
                    bitCount = 0;
                    memset(outputBuffer, 0, HUNK);
                }

                if(code[j] == '1') {
                    outputBuffer[bufferTracker] <<= 1;
                    outputBuffer[bufferTracker] |= 1;
                }
                else {
                    outputBuffer[bufferTracker] <<= 1;
                }
                bitCount++;
                j++;
            }

                
        }

    }
    if(bytesRead < 0) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }
    if(bitCount != 0) { 
    /*if bits are left over, shift them to the 
     * left side and pad the end */
        outputBuffer[bufferTracker] <<= (BYTE_SIZE - bitCount);
        bufferTracker++;        
    }
    write(output, outputBuffer, bufferTracker);
 

}



void write_header(int *frequencyTable, int fd, int *numEntries) {
    /*write the header of the encoded file by iterating over
     * frequency table */
    
    int i = 0;
    int j = 0;
    char buffer[HUNK] = {0};
    int freq;
    int numBytes = 0;
    
    buffer[j++] = (unsigned char)(*numEntries - 1);
    numBytes++;

    for (i = 0; i < MAX_ASCII; i++) {
        /*below converts the frequency to network byte order
         * and then puts the char - freq pair into buffer */
        freq = htonl(frequencyTable[i]);
        if(freq != 0) {
            buffer[j++] = (unsigned char)i;
            memcpy(buffer + j, &freq, sizeof(freq));
            j += sizeof(freq);
            numBytes += sizeof(freq) + 1;
        }
    }
    write(fd, buffer, numBytes);
}


int *fill_frequency_table(int fd) {
    /* populate the frequency table */
    int bytesRead = 0;
    int i = 0;
    char buffer[HUNK] = {0};
    int *frequencyTable = (int*)calloc(MAX_ASCII, (sizeof(int)));
    while((bytesRead = read(fd, buffer, HUNK)) > 0) {
        for(i = 0; i < bytesRead; i++) {
            frequencyTable[((unsigned char)buffer[i])]++;
        }
    }

    return frequencyTable;

}

void free_codes(char **codes) {
    /* frees all strings in codes table */
    int i = 0;

    if (codes == NULL) {return;}

    for(i = 0; i < MAX_ASCII; i++) {
        free(codes[i]);
    }
    free(codes);
}
