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

int openstat(char* name, struct stat* sb)
{
    int fd;
    //open test file
    if ((fd = open(name, O_RDWR|O_CREAT|O_TRUNC, 0666)) < 0)  //are these permissions ok?
    {
        fprintf(stderr, "Could not open test file for reading and writing: %s\n", strerror(errno));
        exit(255);
    }
    //get stats
    if (fstat(fd, &sb) < 0)
    {
        fprintf(stderr, "Error stat-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    return fd;
}

void closemap(void* map, struct stat* sb, int fd)
{
    //unmap
    if (munmap(map, sb.st_size) < 0)
    {
        fprintf(stderr, "Error unmapping mmap: %s\n", strerror(errno));
        exit(255);
    }
    //close file descriptor
    if (close(fd) < 0)
    {
        fprintf(stderr, "Error closing test file: %s\n", strerror(errno));
        exit(255);
    }
}

void test1()
{
    fprintf(stderr, "Executing test 1:\n");
    int fd;
    struct stat sb;
    void* map;
    //set signal handlers for signals 1 through 31
    for (int i = 1; i < 32; i++)
        signal(i,hand1);
    //open test file and get stats
    fd = openstat("test1.txt", &sb);
    //call mmap
    if ((map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) < 0)
        //are these the right arguments??? shared/private, len, offset??
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    //attempt to write
    //??????????????????????????????
    //write sys call or just map[0] = blah?

    closemap(map,sb,fd);
    exit(0);
}

void hand1(int sig)
{
    fprintf(stderr, "Signal %s received\n", strsignal(sig));
    exit(sig);
}

void test2()
{
    int fd;
    struct stat sb;
    void* map;
    //open test file and get stats
    fd = openstat("test2.txt", &sb);
    //call mmap
    if ((map = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, fd, 0)) < 0)
        //are these the right arguments??? shared/private, len, offset??
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(-1);
    }
    //write to memory
    
    //check memory
    
    closemap(map,sb,fd);
    exit(0);
}

void test3()
{
    int fd;
    struct stat sb;
    void* map;
    //open test file and get stats
    fd = openstat("test3.txt", &sb);
    //call mmap
    if ((map = mmap(NULL, sb.st_size, PROT_WRITE, MAP_PRIVATE, fd, 0)) < 0)
        //are these the right arguments??? shared/private, len, offset??
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(-1);
    }
    //write to memory
    
    //check memory
    

    closemap(map,sb,fd);
    exit(0);
}

void test4()
{
    int fd;
    struct stat sb;
    void* map;
    //open test file and get stats
    fd = openstat("test4.txt", &sb);
    //change file size (lseek and write)
    
    //mmap
    
    //write just beyond last byte
    
    //stat again???
    
    exit(0);
}

void test5()
{
    int fd;
    struct stat sb;
    void* map;
    //open test file and get stats
    fd = openstat("test5.txt", &sb);
    //change file size (lseek?)
    
    //mmap
    
    //write just beyond last byte (X)
    
    //change file size by 16 bytes (lseek)
    
    //check for byte X
    
    exit(0);
}

void test6()
{
    int fd;
    struct stat sb;
    void* map;
    //open test file and get stats
    fd = openstat("test6.txt", &sb);
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