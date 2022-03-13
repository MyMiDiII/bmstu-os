#include <stdio.h>
#include <string.h>

#include "myftw.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        puts("Использование: ./MYFTW <начальный_каталог>");
        return -1;
    }

    return myftw(argv[1]);
}
