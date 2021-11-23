#include <string.h>
#include <stdio.h>

#define OK 0

#define MAX_LEN 50

int distance(const char *const word1, int len1,
             const char *const word2, int len2)
{
    int matrix[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++)
        matrix[i][0] = i;

    for (int i = 0; i <= len2; i++)
        matrix[0][i] = i;

    for (int i = 1; i <= len1; i++)
    {
        char c1 = word1[i-1];

        for (int j = 1; j <= len2; j++)
        {
            char c2 = word2[j-1];

            if (c1 == c2)
            {
                matrix[i][j] = matrix[i-1][j-1];
            }
            else
            {
                int delete = matrix[i-1][j] + 1;
                int insert = matrix[i][j-1] + 1;
                int substitute = matrix[i-1][j-1] + 1;

                int minimum = delete;

                if (insert < minimum)
                    minimum = insert;

                if (substitute < minimum)
                    minimum = substitute;

                matrix[i][j] = minimum;
            }
        }
    }

    return matrix[len1][len2];
}

int main(int argc, char **argv)
{
    printf("\nРАССТОЯНИЕ ЛЕВЕНШТЕЙНА\n\n");

    char *inFileName = argv[1];
    FILE *inFile = fopen(inFileName, "r");

    char word1[MAX_LEN];
    char word2[MAX_LEN];

    fgets(word1, MAX_LEN - 1, inFile);
    fgets(word2, MAX_LEN - 1, inFile);

    int len1 = strlen(word1) - 1;
    int len2 = strlen(word2) - 1;

    word1[len1] = '\0';
    word2[len2] = '\0';

    int dist = distance(word1, len1, word2, len2);

    printf("Первое слово: %s\n", word1);
    printf("Второе слово: %s\n", word2);
    printf ("Расстояние Левенштейна между данными словами равно %d\n", dist);

    return OK;
}
