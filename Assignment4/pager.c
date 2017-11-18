#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//prints the rest if ^D hit during "press RETURN for more"

const int MAX_WORD_LEN = 30;

int main()
{
    int lines = 0;
    //open terminal
    FILE* pt;
    if ((pt = fopen("/dev/tty", "r+")) == NULL)
    {
        fprintf(stderr, "Error opening terminal for reading and writing: %s\n", strerror(errno));
        return -1;
    }
    size_t n = 0;
    char c;
    char* line = NULL;
    while (getline(&line, &n, stdin) != -1) //&& feof(pt) == 0?
    {
        fprintf(stdout, "%s", line);
        lines++;
        if (lines >= 23)
        {
            fprintf(stdout, "---Press RETURN for more---");
            c = getc(pt);
            if (c == 'q' || c == 'Q') //or == 3?
            {
                return 0;
            }
            lines = 0;
        }
    }
    return 0;
}