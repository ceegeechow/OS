//  Camille Chow
//  minicat.c
//  ECE357: Problem 1
//  9/24/17.

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

int buff_size, fdout, fdin, no_input, input_count;
char* out_filename;
char* in_filename;

int main(int argc, char** argv) {
    
    fdin = 0;
    fdout = 1;
    buff_size = 16384;
    out_filename = "stdout";
    in_filename = "stdin";
    no_input = 0;
    
    //process command line arguments
    int arg;
    
    while ((arg = getopt(argc, argv, "b:o:")) != -1) {
        //search for flags
        switch (arg) {
            //set buffer size
            case 'b':
                buff_size = atoi(optarg);
                break;
            //open output file
            case 'o':
                out_filename = optarg;
                
                if ((fdout = open(out_filename, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
                    fprintf(stderr, "Could not open file '%s' for writing: %s\n", out_filename, strerror(errno));
                    return -1;
                }
                break;
        }
    }
    
    //determine number of input files
    if (optind >= argc) {
        input_count = 1; //if no input specified, the input "file" is stdin
        no_input = 1;
    }
    else {
        input_count = argc - optind;
    }
    
    //begin concatenating
    char buffer[buff_size*8];
    int n, m, p;
    n = -1;
    m = -1;
    p = -1;
    
    //reading and writing main loop
    for (int i = 0; i < input_count; i++) {
        
        if (no_input == 1) {
            in_filename = "-";
        }
        else {
            in_filename = argv[optind+i];
        }
        
        if ((strcmp(in_filename, "-") == 0)) {
            fdin = 0;
            in_filename = "stdin";
        }
        //open input file
        else if ((fdin = open(in_filename, O_RDONLY)) < 0) {
            fprintf(stderr, "Error opening file '%s' for reading: %s\n", in_filename, strerror(errno));
            return -1;
        }
        
        //read to buffer
        while ((n = read(fdin, buffer, buff_size)) != 0) {
            if (n < 0) {
                fprintf(stderr, "Error reading from input file '%s': %s\n", in_filename, strerror(errno));
                return -1;
            }
            //write from buffer
            else {
                while (m < n) { //in case of partial writes
                    if ((m = write(fdout, buffer, n)) < 0) {
                        fprintf(stderr, "Error writing to output file '%s': %s\n", out_filename, strerror(errno));
                        return -1;
                    }
                    n -= m;
                    m = 0;
                }
            }
        }
        
        //close input file (if it is a file)
        if ((fdin != 0) && (p = close(fdin)) != 0) {
            fprintf(stderr, "Error closing input file '%s': %s\n", in_filename, strerror(errno));
            return -1;
        }
    }
    
    //close output file (if it is a file)
    if ((fdout != 1) && (p = close(fdout)) != 0) {
        fprintf(stderr, "Error closing output file '%s': %s\n", out_filename, strerror(errno));
        return -1;
    }
    
    return 0;
}