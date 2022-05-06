#include <stdio.h>

#define GREEN "\033[01;38;05;46m"
#define BLUE  "\033[01;38;05;33m"
#define CLEAR "\033[0m"

int main(void)
{
    FILE *fs1 = fopen("result.txt", "w");
    FILE *fs2 = fopen("result.txt", "w");

    for (char ch = 'a'; ch <= 'z'; ++ch)
        fprintf(ch % 2 ? fs1 : fs2, "%s%c" CLEAR, ch % 2 ? GREEN : BLUE, ch);

    fclose(fs1);
    fclose(fs2);

    return 0;
}
