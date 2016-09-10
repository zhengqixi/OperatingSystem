#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>

void errorReport(char* action, char* why, char* object){
    printf("Error trying to %s %s for %s:\n%s", action, object, why, strerror(errno)); 
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
                errorReport("open", "writing", argv[i]);
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
            printf("Error trying to open %s for reading:\n %s \n", argv[i], strerror(errno));
            return -1;
        } else {
            int amountRead = 1;
            while (amountRead != 0) {
                amountRead = read(readFile, buffer, bufferSize); 
                if (amountRead < 0) {
                    printf("Error trying to read %s:\n%s", argv[i], strerror(errno));
                    return -1;
                }
                int amountWrote = write(outputFile, buffer, amountRead);
            }
        }
        ++i;
    }


    return 0;
}
