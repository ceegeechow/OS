#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

const int capacity = 300000;
int matches = 0;
//char* dict[capacity];
//int taken[capacity];

//int hash(char* key)
//{
//    int hashVal = 0;
//    for (int i = 0; i < sizeof(key); i++)
//        hashVal = 37*hashVal + key[i];
//    hashVal %= capacity;
//    
//    if (hashVal < 0)
//        hashVal += capacity;
//    
//    return hashVal;
//}
//
//void insert(char* key)
//{
//    int p = hash(key);
//    while (taken[p] == 1)
//    {
//        p++;
//        p %= capacity;
//    }
//    dict[p] = key;
//    taken[p] = 1;
//}
//
//int findPos(char* key)
//{
//    int i = hash(key);
//    while (taken[i] == 1)
//    {
//        if (dict[i] == key)
//            return i;
//        i++;
//        i %= capacity;
//    }
//    return -1;
//}

char* upper(char* str)
{
    for (int i = 0; i < strlen(str); i++)
        str[i] = toupper(str[i]);
    return str;
}

void pipe_handle(int sig)
{
    fprintf(stderr, "Matched %d words\n", matches);
    signal(SIGPIPE, SIG_DFL);
    raise(SIGPIPE);
}

int main(int argc, char** argv)
{
    signal(SIGPIPE, pipe_handle);
    
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
    char** dict = malloc(capacity);
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
//        if (findPos(buf) != -1)
//        {
//            fprintf(stdout, "%s", buf);
//            matches++;
//        }
    }
    fprintf(stderr, "Matched %d words\n", matches);
    for (int k = 0; k < i; k++)
        free(dict[k]);
    free(buf);
    return 0;
}