#include <stdio.h>
#include <errno.h>
//#include <fcntl.h>
#include <string.h>

//split up into smaller funcitons?
//include accepted/rejected words?
//hashing?

const int MAX_WORD = 30;

char* upper(char* str)
{
    for (int i = 0; i < sizeof(str); i++)
        str[i] = toupper(str[i]);
    return str;
}

int main(int argc, char** argv)
{
//    char*line = "hi";
//    printf("%s\n", upper(argv[1]));
//    
//    printf("%s\n", upper(line));
//    printf("%s\n", argv[1]);
    //open dictionary
    if (argc < 2)
    {
        fprintf(stderr, "Fatal Error: No dictionary file specified\n");
        return -1;
    }
    FILE* pdict;
    if ((pdict = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "Error opening '%s' for reading: %s\n", argv[1], strerror(errno));
        return -1;
    }
    //read dictionary/store in array
    char** dict;                        //malloc?
    int i = 0;
    char* line;                     //malloc??
    size_t n = 0;
    while (getline(&line, &n, pdict) != -1)     //stores the newline --> bad!?
    {
        printf("got line: %s\n", line);
        dict[i] = upper(line);
        i++;
    }
    if (feof(stdin) == 0)           //necessary?
    {
        fprintf(stderr, "Error reading dictionary file: %s\n", strerror(errno));
        return -1;
    }
    //close dictionary
    if (fclose(pdict) != 0)
    {
        fprintf(stderr, "Error closing dictionary file '%s': %s\n", argv[1], strerror(errno));
        return -1;
    }
    //check input for matches
    int matches = 0;
                //shoud i use different getline vars?
    while (getline(&line, &n, stdin) != -1 && feof(stdin) == 0)
    {
        for (int j = 0; j < i; j++)
        {
            if (strcmp(dict[j], upper(line)) == 0)
            {
                fprintf(stdout, "%s", line);
                matches++;
            }
        }
    }
    return 0;
}