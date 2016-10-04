#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>

#define PATHNAME_LENGTH 

int main(int argc, char *argv[]) {
    extern char *optarg;
    extern int optind;
    int c;
    while ( (c = getopt(argc, argv, "u:m:x:l")) != -1) {
        switch(c) {
            case 'u':
            break;
            case 'm':
            break;
            case 'x':
            break;
            case 'l':
            break;
        }
    }
    char * directory = (char *)malloc(sizeof(char)*8*1024);
    if (optind >= argc) {
    } else {
    }
    free(directory);
}

    
