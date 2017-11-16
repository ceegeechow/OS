#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//split up into smaller funcitons?
//include accepted/rejected words?
//hashing?

const int MAX_WORD_LEN = 30;
const int DICT_LEN = 30000;

char* upper(char* str)
{
    int i = 0;                              //???
    for (; i < strlen(str); i++)
        str[i] = toupper(str[i]);
    return str;
}

int main(int argc, char** argv)
{
    //open dictionary
    if (argc < 2)
    {
        fprintf(stderr, "No dictionary file specified\n");
        return -1;
    }
    FILE* pdict;
    if ((pdict = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "Error opening '%s' for reading: %s\n", argv[1], strerror(errno));
        return -1;
    }
    //read dictionary/store in array
    char** dict = malloc(DICT_LEN);
    char* line = malloc(MAX_WORD_LEN);
    int i = 0;
    while (getline(&line, &MAX_WORD_LEN, pdict) != -1)     //doesnt store last newline
    {
        dict[i] = upper(line);
        i++;
    }
//    if (!feof(stdin))           //necessary?
//    {
//        fprintf(stderr, "Error reading dictionary file: %s\n", strerror(errno));
//        return -1;
//    }
    //close dictionary
    printf("hello, %d", dict[100][0]);    //by this point dictionary stores null terminators only
    if (fclose(pdict) != 0)
    {
        fprintf(stderr, "Error closing dictionary file '%s': %s\n", argv[1], strerror(errno));
        return -1;
    }
    //check input for matches
    int matches = 0;
    char* buf = malloc(MAX_WORD_LEN);
    while (getline(&buf, &MAX_WORD_LEN, stdin) != -1) //              && feof(stdin) == 0?
    {       //fails here
        
        for (int j = 0; j < i; j++)
        {
            
//            if (strcmp(dict[j], upper(buf)) == 0)
//            {
//                fprintf(stdout, "%s", buf);
//                matches++;
//            }
        }
    }
    
    free(dict);
    free(line);
    free(buf);
    return 0;
}