#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const int DICT_LEN = 300000;

char* upper(char* str)
{
    for (int i = 0; i < strlen(str); i++)
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
    int i = 0;
    size_t n = 0;
    while (getline(&dict[i], &n, pdict) != -1)
    {
        upper(dict[i]);
        i++;
    }
    //close dictionary
    if (fclose(pdict) != 0)
    {
        fprintf(stderr, "Error closing dictionary file '%s': %s\n", argv[1], strerror(errno));
        return -1;
    }
    //check input for matches
    int matches = 0;
    n = 0;
    char* buf = NULL;
    while (getline(&buf, &n, stdin) != -1)
    {
        for (int j = 0; j < i; j++)
        {
            if (strcmp(dict[j], upper(buf)) == 0)
            {
                fprintf(stdout, "%s", buf);
                matches++;
            }
        }
    }
    for (int k = 0; k < i; k++)
        free(dict[k]);
    free(buf);
    return 0;
}