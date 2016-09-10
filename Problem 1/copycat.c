#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

void errorReport(char* action, char* why, char* object){
    printf("Error trying to %s %s %s:\n%s", action, object, why, strerror(errno)); 
}

int main(int argc, char *argv[]){
    int bufferSize = 256;
    int outputFile = STDOUT_FILENO;
    int i;
    for (i = 1; i < argc ; ++i) {
        if (strcmp(argv[i], "-b") == 0) {
            ++i;
            bufferSize = atoi(argv[i]);
        } else if (strcmp(argv[i], "-o") == 0){
            ++i;
            outputFile = open(argv[i], O_WRONLY | O_TRUNC | O_CREAT, 00700);
            if (outputFile < 0) {
                errorReport("open", " for writing", argv[i]);
                return -1;
            }
        } else {
            break;
        }
    }

    char buffer[bufferSize];
    while ( i < argc) {
        int readFile;
        readFile = open(argv[i], O_RDONLY); 
        if (readFile < 0) {
            errorReport("open", "for reading", argv[i]);
            return -1;
        } else {
            int amountRead = 1;
            while (amountRead != 0) {
                amountRead = read(readFile, buffer, bufferSize); 
                if (amountRead < 0) {
                    errorReport("read", 0, argv[i]);
                    return -1;
                }
                int amountWrote = write(outputFile, buffer, amountRead);
                if (amountWrote < 0) {
                    errorReport("write to", 0, argv[i]);
                    return -1;
                }
            }
        }
        ++i;
    }


    return 0;
}
