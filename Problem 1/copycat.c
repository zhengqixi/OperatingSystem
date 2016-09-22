#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>

void errorReport(char* action, char* why, char* object){
    printf("Error trying to %s %s %s:\n%s\n", action, object, why, strerror(errno)); 
}

int writeToFile(int readFile, int outputFile, char * buffer, int bufferSize, char * fileName) {
    int amountRead = 1;
    while (amountRead != 0) {
        amountRead = read(readFile, buffer, bufferSize); 
        if (amountRead < 0) {
            errorReport("read", 0, fileName);
            return -1;
        }
        int amountWrote = write(outputFile, buffer, amountRead);
        while(amountWrote < amountRead) {
            if (amountWrote < 0) {
                errorReport("write to", 0, fileName);
                return -1;
            }
            //Accounting for partial writes. This should work........
            //Just keep writing until everything is written
            buffer += amountWrote;
            amountRead -= amountWrote;
            amountWrote = write(outputFile, buffer, amountRead);
        }
    }
}

int main(int argc, char *argv[]){
    int bufferSize = 256;
    int outputFile = STDOUT_FILENO;
    char * outputFileName;

    extern char *optarg;
    extern int optind;
    int c;
    while ( (c = getopt(argc, argv, "b:o:")) != -1) {
        switch(c) {
            case 'b':
                bufferSize = atoi(optarg);
                break;
            case 'o':
                outputFile = open(optarg, O_WRONLY | O_TRUNC | O_CREAT, 00700);
                if (outputFile < 0) {
                    errorReport("open", " for writing", optarg);
                    return -1;
                }
                outputFileName = optarg;
                break;
            case '?':
                printf("usage: copycat [-b ###] [-o outfile] infile [...infile2....] \n");
                return -1;
        }
    }

    char buffer[bufferSize];

    if ( optind  >= argc) 
        return writeToFile(STDIN_FILENO, outputFile, buffer, bufferSize, "Standard Input");

    while ( optind < argc) {
        int readFile;
        char * fileName = "Standard Input";
        if (strcmp(argv[optind], "-") == 0) {
            readFile = STDIN_FILENO;
        } else {
            readFile = open(argv[optind], O_RDONLY); 
            fileName = argv[optind]; 
        }
        if (readFile < 0) {
            errorReport("open", "for reading", fileName);
            return -1;
        } else {
            if (writeToFile(readFile, outputFile, buffer, bufferSize, fileName) == -1)
                return -1;
        }
        if (readFile != STDIN_FILENO) {
            int closed = close(readFile);
            if (closed < 0) {
                errorReport("close", "", fileName);
                return -1;
            }
        }
        ++optind;
    }

    if (outputFile != STDOUT_FILENO) {
        int closed = close(outputFile);
        if (closed < 0) {
            errorReport("close", "", outputFileName); 
        }
    }


    return 0;
}
