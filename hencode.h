#ifndef HENCODE
#define HENCODE

int *fill_frequency_table(int fd);

void write_file(int input, int output, char **codes);

void write_header(int *frequency_table, int fd, int *numEntries);

void free_codes(char **codes);

#endif
