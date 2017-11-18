#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char** argv)
{
    int nc = 5; //max word length
    int m, wordcount;
    if (argc > 1)
        wordcount = atoi(argv[1]);
    srand((unsigned) time(NULL));
    char word[nc + 1];
    while (argc == 1 || wordcount > 0)
    {
        m = rand()%(nc-2)+3;
        for (int i = 0; i < m; i++)
            word[i] = rand()%26 + 65;
        word[m] = '\0';
        fprintf(stdout, "%s\n", word);
        wordcount--;
    }
    return 0;
}