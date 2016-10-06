#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<fcntl.h>
#include<pwd.h>

char *beginning;
int use_id;
int use_time;
int stay_in_mount;
int strict_symlink;

dev_t current_device;
uid_t user_id;
time_t time_range;

void errorReport(char * message){
    fprintf(stderr, "Error %s %s: %s", message, beginning, strerror(errno));
    errno = 0;
    return;
}

void printStat(struct stat *data) {

    if (use_id != 0 && user_id != data->st_uid)
        return;

    char type = '-';
    switch (data->st_mode & S_IFMT) {
        case (S_IFDIR):
            type = 'd';
            break;
        case (S_IFLNK):
            type = 'l';
            break;
        case (S_IFBLK):
            type = 'b';
            break;
        case (S_IFSOCK):
            type = 's';
            break;
        case (S_IFCHR):
            type = 'c';
            break;
    }

    char * link_path;
    if (type == 'l'){
        link_path = (char*)malloc(data->st_size * sizeof(char) + 1);
        int success = readlink(beginning, link_path, data->st_size); 
        if (success == -1) {
            errorReport("reading symbolic link");
            if (strict_symlink == 1) { 
                free(link_path);
                return;
            }
            link_path = realloc(link_path, 50);
            if (link_path == NULL){
                errorReport("at");
                return;
            }
            strcpy(link_path, "Cannot resolve symlink");
        } else {
            link_path[data->st_size] = '\0';
            if (strict_symlink == 1 && (success = open(link_path, O_RDONLY)) < 0){
                free(link_path);
                return;
            }
            if ( (success = close(success)) == -1){
                errorReport("at");
                free(link_path);
                return;
            }
        }
    }

    char permissions[] = { type, 
        (data->st_mode & S_IRUSR) ? 'r' : '-',
        (data->st_mode & S_IWUSR) ? 'w' : '-',
        (data->st_mode & S_IXUSR) ? 'x' : '-',
        (data->st_mode & S_IRGRP) ? 'r' : '-',
        (data->st_mode & S_IWGRP) ? 'w' : '-',
        (data->st_mode & S_IXGRP) ? 'x' : '-',
        (data->st_mode & S_IROTH) ? 'r' : '-',
        (data->st_mode & S_IWOTH) ? 'w' : '-',
        (data->st_mode & S_IXOTH) ? 'x' : '-', 
        '\0'
    };
    printf("%lu/%lu %s %lu %s", data->st_dev, data->st_ino, permissions, data->st_nlink, beginning);
    if (type = 'l'){
        printf("->%s", link_path);
        free(link_path);
    } else {
        printf("\n");
    }
    return;


}

void recursiveTraverse(char *extend) {
    DIR *directory = opendir(beginning);
    if(directory == NULL) {
        if(errno == EACCES) {
            errorReport("opening");
            return;
        }
        errorReport("opening");
        exit(-1);
    }
    struct dirent *files;
    errno = 0;
    while((files = readdir(directory)) != NULL) {
        if(strcmp(".", files->d_name) == 0 || strcmp("..", files->d_name) == 0) {
            continue;
        }
        strcpy(extend + 1, files->d_name);
        (*extend) = '/';

        struct stat buf;
        int open = stat(beginning, &buf);
        if(open != 0) {
            errorReport("reading");
            continue;
        }

        if(files->d_type == DT_DIR) {
            if(stay_in_mount == 1 && (current_device != buf.st_dev)) {
                fprintf(stderr, "note: not crossing mount point at: %s\n", beginning);
                continue;
            }
            int length = strlen(files->d_name);
            recursiveTraverse(extend + length + 1);
        } else {
            printStat(&buf);
        }

    }
    if(errno) {
        errorReport("attempting to read file in directory");
    }
    int closed = closedir(directory);
    if(closed != 0) {
        errorReport("closing");
        exit(-1);
    }
    (*extend) = 0;

}

int main(int argc, char *argv[]) {
    use_id = use_time = stay_in_mount = strict_symlink = 0;
    extern char *optarg;
    extern int optind;
    int c;
    while((c = getopt(argc, argv, "u:m:xl:")) != -1) {
        switch (c) {
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
    if(optind >= argc) {
        fprintf(stderr, "Usage: traverse [-u] [-m] [-x] [-l] directory_path\n");
        return 0;
    }
    if(stay_in_mount) {
        struct stat buf;
        int open = stat(argv[optind], &buf);
        if(open != 0) {
            fprintf(stderr, "Error attemping to open directory %s: %s\n", argv[optind], strerror(errno));
            return -1;
        }
        current_device = buf.st_dev;
    }
    beginning = (char *) malloc(sizeof(char) * 64 * 1024);
    strcpy(beginning, argv[optind]);
    int length = strlen(beginning);
    recursiveTraverse(beginning + length);
    free(beginning);
    return 0;
}
