#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//do i need to print how many words were generated?

int main(int argc, char** argv)
{
    int nc = 5; //max word length
    int m, wordcount;
    if (argc > 1)
        wordcount = atoi(argv[1]);      //try catch?
    srand((unsigned) time(NULL));
    char* word;
    while (argc == 1 || wordcount > 0)
    {
        m = rand()%(nc-2)+3;
        word = malloc(m);         //necessary??
        for (int i = 0; i < m; i++)
            word[i] = rand()%26 + 65;
        fprintf(stdout, "%s\n", word);
        memset(word,0,m);               //better way?
        free(word);
        wordcount--;
    }
    return 0;
}