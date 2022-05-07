#include <stdio.h>
#include <sys/stat.h>

#define CLEAR "\033[0m"

void fileInfo(FILE *fs)
{
	struct stat statbuf;
	stat("result.txt", &statbuf);
	printf("\033[38;05;214minode: %ld\n", statbuf.st_ino);
	printf("Общий размер в байтах: %ld\n", statbuf.st_size);
	printf("Текущая позиция: %ld\n\n" CLEAR, ftell(fs));
}

int main(void)
{
    FILE *fs1 = fopen("result.txt", "w");
    fileInfo(fs1);

    FILE *fs2 = fopen("result.txt", "w");
    fileInfo(fs2);

    for (char ch = 'a'; ch <= 'z'; ++ch)
        fprintf(ch % 2 ? fs1 : fs2, "%c", ch);

    fileInfo(fs1);
    fclose(fs1);
    fileInfo(fs1);

    fileInfo(fs2);
    fclose(fs2);
    fileInfo(fs2);

    return 0;
}
