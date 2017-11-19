#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main()
{
    int lines = 0;
    FILE* pt;
    if ((pt = fopen("/dev/tty", "r+")) == NULL)
    {
        fprintf(stderr, "Error opening terminal for reading and writing: %s\n", strerror(errno));
        return -1;
    }
    size_t n = 0;
    char c;
    char* line = NULL;
    while (getline(&line, &n, stdin) != -1)
    {
        fprintf(stdout, "%s", line);
        lines++;
        if (lines >= 23)
        {
            fprintf(stdout, "---Press RETURN for more---");
            if (getline(&line, &n, pt) == -1 || line[0] == 'q' || line[0] == 'Q')
            {
                return 0;
            }
            lines = 0;
        }
    }
    return 0;
}