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
#include<time.h>

char *beginning;

int use_id;
uid_t user_id;

int use_time;
double time_range;
time_t current_time;

int strict_symlink;
dev_t target_device;
ino_t target_inode;

int stay_in_mount;
dev_t current_device;

void errorReport(char * message){
    fprintf(stderr, "Error %s %s: %s\n", message, beginning, strerror(errno));
    errno = 0;
    return;
}

char* symResolve(struct stat * data, char * extend) {
    char * link = (char*)malloc(data->st_size * sizeof(char) + 1);
    int success = readlink(beginning, link, data->st_size); 
    if (success == -1) {
        errorReport("reading symbolic link");
        if (strict_symlink) { 
            free(link);
            return NULL;
        }
        link = realloc(link, 50);
        if (link == NULL){
            errorReport("allocating memory at");
            exit(-1);
        }
        strcpy(link, "Cannot resolve symlink");
    } else {
        link[data->st_size] = '\0';
        if (strict_symlink){
            struct stat buf;
            (*extend) = '/';
            strcpy(extend + 1, link);
            int open = lstat(beginning, &buf);
            if (open != 0) {
                (*extend) = '\0';
                free(link);
                return NULL;
            }
            (*extend) = '\0';
            return (target_device == buf.st_dev & target_inode == buf.st_ino)? link : NULL;
        }
    }
    return link;
}

void printStat(struct stat *data, char * extend) {

    if (use_id != 0 && user_id != data->st_uid)
        return;

    if (use_time){
        double time_difference = difftime(current_time, data->st_mtim.tv_sec);
        if (time_range > 0 && time_difference < time_range)
            return;
        if (time_range < 0 && time_difference > (-1 * time_range))
            return;
    }

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
        if ( (link_path = symResolve(data, extend)) == NULL){
            return;
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
    char date[256];
    strftime(date, 256, "%D %T", localtime(&data->st_mtim.tv_sec));
    printf("%lu/%lu %s %lu %lu %s %s", data->st_dev, data->st_ino, permissions, data->st_nlink, data->st_size, date, beginning);
    if (type == 'l'){
        printf(" -> %s\n", link_path);
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
            errorReport("opening directory");
            return;
        }
        errorReport("opening directory");
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
        int open = lstat(beginning, &buf);
        if(open != 0) {
            errorReport("reading file metadata");
            continue;
        }
        printStat(&buf, extend);
        if(files->d_type == DT_DIR) {
            if(stay_in_mount == 1 && (current_device != buf.st_dev)) {
                fprintf(stderr, "note: not crossing mount point at: %s\n", beginning);
                continue;
            }
            int length = strlen(files->d_name);
            recursiveTraverse(extend + length + 1);
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
    (*extend) = '\0';
    return;
}

int main(int argc, char *argv[]) {
    use_id = use_time = stay_in_mount = strict_symlink = 0;
    char *target_path, *time_string, *user_name;
    extern char *optarg;
    extern int optind;
    int c;
    while((c = getopt(argc, argv, "u:m:xl:")) != -1) {
        switch (c) {
            case 'u':
                use_id = 1;
                user_name = optarg;
                break;
            case 'm':
                use_time = 1;
                time_string = optarg;
                break;
            case 'x':
                stay_in_mount = 1;
                break;
            case 'l':
                strict_symlink = 1;
                target_path = optarg;
                break;
            case '?':
                fprintf(stderr, "Usage: traverse [-u] [-m] [-x] [-l] directory_path\n");
                return 0;
        }
    }

    if(optind >= argc) {
        fprintf(stderr, "Usage: traverse [-u] [-m] [-x] [-l] directory_path\n");
        return 0;
    }

    if (use_time){
        current_time = time(NULL); 
        time_range = strtod(time_string, NULL);
        
    }

    if(stay_in_mount) {
        struct stat buf;
        int open = lstat(argv[optind], &buf);
        if(open != 0) {
            fprintf(stderr, "Error attemping to open directory %s: %s\n", argv[optind], strerror(errno));
            return -1;
        }
        current_device = buf.st_dev;
    }

    if (strict_symlink){
        struct stat buf;
        int open = lstat(target_path, &buf);
        if(open != 0) {
            fprintf(stderr, "Error attemping to file %s: %s\n", argv[optind], strerror(errno));
            return -1;
        }
        target_device = buf.st_dev;
        target_inode = buf.st_ino;
    }

    beginning = (char *) malloc(sizeof(char) * 64 * 1024);
    strcpy(beginning, argv[optind]);
    int length = strlen(beginning);
    recursiveTraverse(beginning + length);
    free(beginning);
    return 0;
}
