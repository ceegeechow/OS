//Camille Chow
//ECE357 PSET 5

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <unistd.h>

void test1()
{
    int fd;
    struct stat sb;
    char* map;
    char* name = "test1.txt";
    //set signal handlers for signals 1 through 31
    for (int i = 1; i < 32; i++)
        signal(i,hand1);
    //open test file
    if ((fd = open(name, O_RDONLY)) < 0)    //read only????
    {
        fprintf(stderr, "Could not open test file for reading: %s\n", strerror(errno));
        exit(255);
    }
    //get size
    if (fstat(fd, &sb) < 0)
    {
        fprintf(stderr, "Error stat-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    //call mmap
    if ((map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) < 0)
        //are these the right arguments??? shared/private, len, offset??
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    //attempt to write
    //??????????????????????????????
    //unmap and close
    if (munmap(map, sb.st_size) < 0)
    {
        fprintf(stderr, "Error unmapping mmap: %s\n", strerror(errno));
        exit(255);
    }
    if (close(fd) < 0)
    {
        fprintf(stderr, "Error closing test file: %s\n", strerror(errno));
        exit(255);
    }
    exit(0);
}

void hand1(int sig)
{
    fprintf(stderr, "Signal %s received\n", strsignal(sig));
    exit(sig);
}

void test2()
{
    exit(0);
}

void test3()
{
    exit(0);
}

void test4()
{
    exit(0);
}

void test5()
{
    exit(0);
}

void test6()
{
    exit(0);
}

int main(int argc, char**argv)
{
    if (argv[1] == NULL)
    {
        fprintf(stderr, "Please specify test number\n");
        return 0;
    }
    switch (atoi(argv[1])) {
        case 1:
            test1();
        case 2:
            test2();
        case 3:
            test3();
        case 4:
            test4();
        case 5:
            test5();
        case 6:
            test6();
            
        default:
            fprintf(stderr, "Test number must be between 1 and 6\n");
            return 0;
    }
    return 0;
}