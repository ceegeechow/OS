//Camille Chow
//ECE357 PSET 5

#include <stdio.h>
#include <stdlib.h>

void test1()
{
    exit(0);
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
    if (argc == 1)
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
}