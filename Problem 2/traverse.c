#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>

void recursiveTraverse(char * beginning, char * extend) {
    printf("Current Directory: %s\n", beginning);
    DIR * directory = opendir(beginning); 
    if (directory == NULL) {
        printf("DEAD: %s\n", strerror(errno));
        exit(-1);
    }
    struct dirent * files;
    while ( (files = readdir(directory)) != NULL ) {
        if (strcmp(".", files->d_name) == 0 || strcmp("..", files->d_name) == 0) {
            continue;
        }
        if ( files->d_type == DT_DIR ) {
            //some fancy directory manipulation and recursive call
            int length = strlen(files->d_name);
            strcpy(extend+1, files->d_name);
            (*extend) = '/';
            recursiveTraverse(beginning, extend + length + 1);
        } else {
            printf("%s\n", files->d_name);
        }

    }
    (*extend) =  0;

}

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
        strcpy(directory, argv[optind]);
        int length = strlen(directory);
        recursiveTraverse(directory, directory+length);
    }
    free(directory);
}

    
