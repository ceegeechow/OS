//Camille Chow
//ECE357 PSET 5

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

//is closemap necessary???

int openfile(char* name, int size)
{
    FILE* fs;
    int fd;
    //open test file
    if ((fs = fopen(name, "w+")) < 0)
    {
        fprintf(stderr, "Could not open test file for reading and writing: %s\n", strerror(errno));
        exit(255);
    }
    //write to file
    for (int i = 0; i < size; i++)
        fprintf(fs, "A");
    //get file descriptor
    fd = fileno(fs);
    return fd;
}

void closemap(char* map, int size, int fd)
{
    //unmap
    if (munmap(map, size) < 0)
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

void hand1(int sig)
{
    fprintf(stderr, "Signal \"%s\" received\n", strsignal(sig));
    exit(sig);
}

void test1()
{
    fprintf(stderr, "Executing test 1...\n");
    int fd;
    int file_size = 5000; //only works for at least 4097???
    char* map;
    //set signal handlers for signals 1 through 31
    for (int i = 1; i < 32; i++)
        signal(i,hand1);
    //open test file and get stats
    fd = openfile("test1.txt", file_size);
    //call mmap
    if ((map = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    //attempt to write
    fprintf(stderr, "map[3]: %c\n", map[3]);
    fprintf(stderr, "writing to map[3]\n");
    map[3] = 'A';
    closemap(map,file_size,fd); //do i need this?
    exit(0);
}

void test2and3(int testno)
{
    int fd;
    int file_size = 5000;
    char* map;
    char* name = "test2.txt";
    int flag = MAP_SHARED;
    if (testno == 3)
    {
        name = "test3.txt";
        flag = MAP_PRIVATE;
    }
    //open test file and get stats
    fd = openfile(name, file_size);
    //call mmap
    if ((map = mmap(NULL, file_size, PROT_READ|PROT_WRITE, flag, fd, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(-1);
    }
    //write to memory
    fprintf(stderr, "writing 'B' to map[50]\n");
    map[50] = 'B';
    //check memory
    char buf[1];
    lseek(fd, 50, SEEK_SET);
    read(fd,buf,1);
    if (buf[0] == 'B')
    {
        fprintf(stderr, "success!\n");
        closemap(map,file_size,fd);
        exit(0);
    }
    else
    {
        fprintf(stderr, "fail :(\n");
        closemap(map,file_size,fd);
        exit(1);
    }
}

void test4()
{
    int fd;
    
    char* map;
    //open test file and get stats
    //fd = openfile("test4.txt", &sb);
    //change file size (lseek and write)
    
    //mmap
    
    //write just beyond last byte
    
    //stat again???
    
    exit(0);
}

void test5()
{
    int fd;
    
    char* map;
    //open test file and get stats
    //fd = openfile("test5.txt", &sb);
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
    
    char* map;
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
            test2and3(2);
        case 3:
            test2and3(3);
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