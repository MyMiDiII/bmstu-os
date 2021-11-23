#ifndef __IO_H__

#define __IO_H__

#include <stdio.h>
#include <stdbool.h>

typedef unsigned int uint;

int read_str(char *const str, const int max_len, FILE *stream);

void clear_stdin(void);

int read_uint(uint *const uinteger);

int read_uint_from_file(FILE *file, uint *const unint);

int read_answer(int *const answer);

int open_file(FILE **file, const char *const filename, bool *const is_open);

int read_string(char **string, FILE *file);

#endif
