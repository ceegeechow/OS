//Camille Chow
//ECE357 PSET 5

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

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
    for (int i = 0; i < size; i++)      //check for partial writes???
        fprintf(fs, "A");
    if ((fd = fileno(fs)) < 0)
    {
        fprintf(stderr, "Error getting file descriptor: %s\n", strerror(errno));
        exit(255);
    }
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
        exit(255);
    }
    //write to memory
    fprintf(stderr, "writing 'B' to map[50]\n");
    map[50] = 'B';
    //check memory
    char buf[1];
    if (lseek(fd, 50, SEEK_SET) < 0)
    {
        fprintf(stderr, "Error lseek-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    if (read(fd,buf,1) < 0)
    {
        fprintf(stderr, "Error reading test file: %s\n", strerror(errno));
        exit(255);
    }
    closemap(map,file_size,fd);
    if (buf[0] == 'B')
    {
        fprintf(stderr, "success!\n");
        exit(0);
    }
    fprintf(stderr, "fail :(\n");
    exit(1);
}

void test4()
{
    int fd;
    int file_size = 5000;
    char* map;
    struct stat buf;
    //open test file and get stats
    fd = openfile("test4.txt", file_size);
    //call mmap
    if ((map = mmap(NULL, file_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    //find initial size
    if (fstat(fd,&buf) < 0)
    {
        fprintf(stderr, "error stat-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    off_t old = buf.st_size;
    fprintf(stderr, "file size: %lld\n", old);
    //write beyond last byte
    fprintf(stderr, "writing 'B' beyond last byte\n");
    map[file_size+1] = 'B';
    //find new size
    if (fstat(fd,&buf) < 0)
    {
        fprintf(stderr, "error stat-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    off_t new = buf.st_size;
    fprintf(stderr, "new file size: %lld\n", new);
    closemap(map,file_size,fd);
    if (old == new)
    {
        fprintf(stderr, "fail :(\n");
        exit(1);
    }
    fprintf(stderr, "success!\n");
    exit(0);
}

void test5()
{
    int fd;
    int file_size = 5000;
    char* map;
    //open test file and get stats
    fd = openfile("test5.txt", file_size);
    //call mmap
    if ((map = mmap(NULL, file_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    //write beyond last byte
    fprintf(stderr, "writing 'B' beyond last byte\n");
    map[file_size+1] = 'X';
    //write 16 bytes past eof
    if (lseek(fd,16,SEEK_END) < 0)
    {
        fprintf(stderr, "Error lseek-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    char* buf = "B";
    if (write(fd,buf,1) < 0)
    {
        fprintf(stderr, "Error writing to test file: %s\n", strerror(errno));
        exit(255);
    }
    //check for byte 'X'
    if (lseek(fd, file_size + 1, SEEK_SET) < 0)
    {
        fprintf(stderr, "Error lseek-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    if (read(fd,buf,1) < 0)
    {
        fprintf(stderr, "Error reading test file: %s\n", strerror(errno));
        exit(255);
    }
    closemap(map,file_size,fd);
    if (buf[0] == 'X')
    {
        fprintf(stderr, "success!\n");
        exit(0);
    }
    fprintf(stderr, "fail :(\n");
    exit(1);
}

void hand6(int sig)
{
    fprintf(stderr, "Signal \"%s\" received\n", strsignal(sig));
    exit(sig);
}

void test6()
{
    int fd;
    int file_size = 1000;
    char* map;
    //set signal handlers for signals 1 through 31
    for (int i = 1; i < 32; i++)
        signal(i,hand6);
    //open test file and get stats
    fd = openfile("test6.txt", file_size);
    //call mmap
    if ((map = mmap(NULL, 8192, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) < 0)
    {
        fprintf(stderr, "Error mmap-ing test file: %s\n", strerror(errno));
        exit(255);
    }
    //part a)
    fprintf(stderr, "reading memory beyond eof in first page\n");
    fprintf(stderr, "map[2000]: %d\nsuccess!\n", map[2000]);
    //part b)
    fprintf(stderr, "reading memory beyond eof in second page\n");
    fprintf(stderr, "map[5000]: %d\nsuccess!\n", map[5000]);
    closemap(map,8192,fd);
    exit(0);
}

int main(int argc, char**argv)
{
//    FILE* fs = fopen("test.txt", "w+");
//    for (int i = 0; i < 5000; i++)      //check for partial writes???
//        fprintf(fs, "A");
//    struct stat buf;
//    stat("test.txt", &buf);
//    fprintf(stderr, "size: %lld\n", buf.st_size);
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