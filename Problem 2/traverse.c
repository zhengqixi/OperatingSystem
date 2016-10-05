#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>

char * beginning;
int use_id;
int use_time;
int stay_in_mount;
int strict_symlink;

dev_t current_device;
uid_t user_id;
time_t time_range;


void printStat(struct stat * data) {
    printf("%lu/%lu %lu %s\n", data->st_dev, data->st_ino, data->st_nlink, beginning);
    return;


}

void recursiveTraverse(char * extend) {
    errno = 0;
    DIR * directory = opendir(beginning); 
    if (directory == NULL) {
        if (errno == EACCES) {
            fprintf(stderr, "Error Opening %s: %s \n", beginning, strerror(errno));
            return;
        }
        fprintf(stderr, "Error Opening %s: %s\n", beginning,strerror(errno));
        exit(-1);
    }
    struct dirent * files;
    while ( (files = readdir(directory)) != NULL ) {
        if (strcmp(".", files->d_name) == 0 || strcmp("..", files->d_name) == 0) {
            continue;
        }
        strcpy(extend+1, files->d_name);
        (*extend) = '/';

        struct stat buf;
        int open = stat(beginning, &buf);
        if (open != 0){
            fprintf(stderr, "Error reading %s:%s\n", beginning, strerror(errno));
            continue;
        }

        if ( files->d_type == DT_DIR) {
            if (stay_in_mount == 1 && (current_device != buf.st_dev)){
                fprintf(stderr, "note: not crossing mount point at: %s\n", beginning);
                continue;
            }
            int length = strlen(files->d_name);
            recursiveTraverse(extend + length + 1);
        } else {
            printStat(&buf);   
        }

    }
    if (errno){
        fprintf(stderr, "Error attempting to read file in directory %s: %s\n", beginning, strerror(errno));
    }
    int closed = closedir(directory);
    if (closed != 0) {
        fprintf(stderr, "Error Closing %s: %s \n", beginning, strerror(errno));
        exit(-1);
    }
    (*extend) =  0;

}

int main(int argc, char *argv[]) {
    use_id = use_time = stay_in_mount = strict_symlink = 0; 
    extern char *optarg;
    extern int optind;
    int c;
    while ( (c = getopt(argc, argv, "u:m:xl:")) != -1) {
        switch(c) {
            case 'u':
                use_id = 1;
            break;
            case 'm':
                use_time = 1;
            break;
            case 'x':
                stay_in_mount = 1;
            break;
            case 'l':
                strict_symlink = 1;
            break;
        }
    }
    if (optind >= argc) {
        fprintf(stderr, "Usage: traverse [-u] [-m] [-x] [-l] directory_path\n");
        return 0;
    } 
    if (stay_in_mount) {
        struct stat buf;
        int open = stat(argv[optind], &buf);
        if (open != 0){
            fprintf(stderr, "Error attemping to open directory %s: %s\n", argv[optind], strerror(errno));
            return -1;
        }
        current_device = buf.st_dev;
    }
    beginning = (char *)malloc(sizeof(char)*64*1024);
    strcpy(beginning, argv[optind]);
    int length = strlen(beginning);
    recursiveTraverse(beginning+length);
    free(beginning);
    return 0;
}

    
