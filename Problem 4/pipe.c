#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>

int main(){
    int fd[2];
    int success = pipe(fd);
    if (success < 0){
        printf("%s\n", strerror(errno));
        exit(-1);
    }
    success = fcntl(fd[1], F_SETFL, O_NONBLOCK);
    if (success < 0){
        printf("%s\n", strerror(errno));
        exit(-1);
    }
    int total_written = 0;
    while(1){
        int written = write(fd[1], "a", 1); 
        if (written < 1 && errno == EAGAIN){
            printf("Written: %d\n", total_written);
            return 0;
        } else {
            total_written += written;
        }
    }
    return 0;
}
