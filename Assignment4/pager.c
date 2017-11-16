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
    char* line = malloc(MAX_WORD_LEN);
    char* cont = malloc(MAX_WORD_LEN);
    while (getline(&line, &MAX_WORD, stdin) != -1) //&& feof(pt) == 0?
    {
        fprintf(stdout, "%s", line);
        lines++;
        if (lines >= 23)
        {
            fprintf(stdout, "---Press RETURN for more---");
            getline(&cont,&MAX_WORD_LEN,pt);
            if (cont[0] == 'q' || cont[0] == 'Q')
            {
                //print "exited with q"??
                return 0;
            }
            lines = 0;
        }
        memset(line,0,MAX_WORD_LEN);
    }
    free(line);
    //do we need to close terminal?
    return 0;
}