#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#include "io.h"
#include "err.h"

int read_str(char *const str, const int max_len, FILE *stream)
{
    if (!fgets(str, max_len, stream))
        return ERR_READ_STR;

    size_t byte_len = strlen(str);

    if ('\n' == str[byte_len - 1])
        str[byte_len - 1] = '\0';
    else
    {
        clear_stdin();
        return ERR_READ_STR;
    }

    return OK;
}

void clear_stdin(void)
{
    char ch;
    do
    {
        ch = getchar();
    } while (ch != '\n' && ch != EOF);
}

int read_uint(uint *const unint)
{
    long long int llint;

    if (scanf("%lld", &llint) != 1)
        return ERR_NONINT;

    if (llint < 0 || llint > UINT_MAX)
        return ERR_NONUINT;

    *unint = (uint) llint;

    return OK;
}

int read_uint_from_file(FILE *file, uint *const unint)
{
    long long int llint;

    if (fscanf(file, "%lld", &llint) != 1)
        return ERR_NONINT;

    if (llint < 0 || llint > UINT_MAX)
        return ERR_NONUINT;

    *unint = (uint) llint;

    return OK;
}

int read_int(int *const integer)
{
    long long int llint;

    if (fscanf(stdin, "%lld", &llint) != 1)
        return ERR_NONINT;

    if (llint < INT_MIN || llint > INT_MAX)
        return ERR_NONINT;

    *integer = (int) llint;

    return OK;
}

int read_answer(int *const answer)
{
    int exit_code = OK;

    exit_code = read_int(answer);
    clear_stdin();

    if (exit_code)
        return exit_code;

    if (*answer < 0 || *answer > 2)
        return ERR_NONANSWER;

    return OK;
}

int open_file(FILE **file, const char *const filename, bool *const is_open)
{
    *file = fopen(filename, "r");

    if (!*file)
        return ERR_OPEN_FILE;

    *is_open = true;
    return OK;
}

int read_string(char **string, FILE *file)
{
    *string = malloc(sizeof(char));

    if (!string)
        return ERR_MEMORY;

    int len = 0;
    int capacity = 1;

    char cur_ch;
    while ((cur_ch = getc(file)) != '\n' && cur_ch != EOF)
    {
        if (len >= capacity - 1)
        {
            char *tmp = realloc(*string, capacity * 2 * sizeof(char));

            if (!tmp)
            {
                free(*string);
                return ERR_MEMORY;
            }

            *string = tmp;
            capacity *= 2;
        }

        (*string)[len++] = cur_ch;
    }

    (*string)[len] = '\0';

    return OK;
}
