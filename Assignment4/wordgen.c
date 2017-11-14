#include <stdio.h>
#include <stdlib.h>

//print same length words
//prints same words every time (need to use seed?)

int main(int argc, char** argv)
{
    int nc = 10;
    int m, wordcount;
    if (argc > 1)
        wordcount = atoi(argv[1]);      //try catch?

    while (argc == 1 || wordcount > 0)
    {
        m = rand()%(nc-2)+3;
        char* word = malloc(m);         //necessary??
        for (int i = 0; i < m; i++)
            word[i] = rand()%26 + 65;
        fprintf(stdout, "%s\n", word);
        free(word);
        wordcount--;
    }
    return 0;
}