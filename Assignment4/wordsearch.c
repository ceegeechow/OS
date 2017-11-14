#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string>

//split up into smaller funcitons?
//include accepted/rejected words?
//hashing?

int main(int argc, char** argv)
{
    //open dictionary
    int fdict;
    if (argc < 2)
    {
        fprintf(stderr, "Fatal Error: No dictionary file specified\n");
        return -1;
    }
    if ((fdict = open(argv[1], O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error opening '%s' for reading: %s\n", argv[1], strerror(errno));
        return -1;
    }
    //read dictionary/store in array
    char** dict;                        //malloc?
    int i = 0;
    char* line;                     //malloc??
    size_t n = 0;
    while (getline() != -1)
    {
        dict[i] = toupper(line);
        i++;
    }
    if (feof(stdin) == 0)           //necessary?
    {
        fprintf(stderr, "Error reading dictionary file: %s\n", strerror(errno));
        return -1;
    }
    //close dictionary
    if (close(fdin) < 0)
    {
        fprintf(stderr, "Error closing input file '%s': %s\n", argv[1], strerror(errno));
        return -1;
    }
    //check input for matches
    int matches = 0;
                //shoud i use different getline vars?
    while (getline(&line, &n, stdin) != -1 && feof(stdin) == 0)
    {
        for (int j = 0; j < i; j++)
        {
            if (strcmp(dict[j], line) == 0)
            {
                fprintf(stdout, "%s\n", line);
                matches++;
            }
        }
    }
    fprintf(stdout, )
    return 0;
}