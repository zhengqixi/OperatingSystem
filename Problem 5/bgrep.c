#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<signal.h>
#include<setjmp.h>

int return_value;
jmp_buf jumper;

void signal_handler(int signal){
    return_value = -1;
    longjmp(jumper, 1);
}

int main(int argc, char ** argv){
    signal(SIGBUS, signal_handler);
    return_value = 1;
    extern char *optarg;
    extern int optind;
    int c;
    int context = 0;
    char* pattern = NULL;
    while ((c = getopt(argc, argv, "p:c:")) != -1) {
        switch(c) {
            case 'p':
                pattern = optarg;
                break;
            case 'c':
                context = atoi(optarg); 
                break;
            case '?':
                fprintf(stderr, "Invalid argument...\n");
                exit(-1);
                break;
        }
    }
    int pattern_length = 0;
    if (pattern){
        int fd = open(pattern, O_RDONLY);                
        if (fd < 0){
            perror("Failed to open pattern file");
            exit(-1);
        }
        struct stat buf;
        if (stat(pattern, &buf) == -1){
            perror("Failed to access file information");
            exit(-1);
        }
        pattern_length = buf.st_size; 
        pattern = (char*)mmap(NULL,pattern_length, PROT_READ, MAP_PRIVATE, fd, 0); 
    } else {
        if (optind == argc){
            fprintf(stderr, "Insufficient arguments");
            exit(-1);
        }
        pattern = argv[optind];
        pattern_length = strlen(pattern);
    }

    while (optind < argc){
        if (setjmp(jumper) == 1){
            ++optind;
            continue;
        }
        int fd = open(argv[optind], O_RDONLY);
        if (fd < 0) {
            perror("Failed to open file for matching");
            exit(-1);
        }
        struct stat buf;
        if (stat(argv[optind], &buf) == -1){
            perror("Failed to access file information");
            exit(-1);
        }
        int file_length = buf.st_size;
        char* file_compare = (char*)mmap(NULL, file_length, PROT_READ, MAP_PRIVATE, fd, 0); 
        char* start = file_compare;
        int length_remain = file_length - pattern_length;
        while(length_remain > 0) {
            if (memcmp(pattern, file_compare, pattern_length) == 0){
                return_value = 0;                
                printf("%s:%d \n", argv[optind], file_length - length_remain);
            }
            ++file_compare;
            --length_remain;
        }
        ++optind;
    }
    return return_value;
}
