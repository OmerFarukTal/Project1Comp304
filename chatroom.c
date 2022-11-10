#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    printf("Welcome to %s!\n\n", argv[1]);
    char *fileName = malloc(sizeof(char)*(strlen(argv[1] + strlen(argv[2]) + 256)));

    strcat(fileName, "./tmp/");
    strcat(fileName, argv[1]);
    char *roomName = strdup(fileName);
    //printf("%s\n", fileName);
   
    // create room if not exist
    struct stat st = {0};
    if (stat(roomName, &st) == -1) {
        //printf("%s\n", fileName);
        mkdir(roomName, 0700);
    }

    strcat(fileName, "/");
    strcat(fileName, argv[2]);
    char *userName = strdup(fileName);
    
    // create user if not exist
    if (access(userName, F_OK) != 0) {
        //printf("%s\n", fileName); 
        mkfifo(userName, 0666);
    }
    
    // Write message and read message
    
    char* inputStr = NULL;
    size_t len = 0;
    ssize_t lineSize = 0;
   
            
        

    pid_t pid;
    pid = fork();
    if (pid == 0) {
        while(1) {

            lineSize = getline(&inputStr, &len, stdin);
        
            if (lineSize > 1) {

                DIR *d;
                struct dirent *dir;
                d = opendir(roomName);
                if (d) {
                    while((dir = readdir(d)) != NULL) {
                        //strcat(inputStr, dir->d_name);
                        if (strcmp(".",dir->d_name) == 0 || strcmp("..", dir->d_name) == 0 ) {
                            continue;
                        }

                        else if (strcmp(argv[2], dir->d_name) == 0) {
                            // THE USER
                            
                        }
                        else {
                            // WRITE TO OTHERS
                            char *currUser = malloc(sizeof(char)*128);
                            strcat(currUser, roomName);
                            strcat(currUser, "/");
                            strcat(currUser, dir->d_name);
                            
                            //First write in them
                            
                            char *inputMsg = malloc(sizeof(char)*512);
                            strcat(inputMsg, "\033[A\r[");
                            strcat(inputMsg, argv[1]);
                            strcat(inputMsg, "] ");
                            strcat(inputMsg, argv[2]);
                            strcat(inputMsg, ": ");
                            strcat(inputMsg, inputStr);
                            strcat(inputMsg, "\n");


                            //printf("WRİTER\n");
                                
                                
                            int fd0;                    
                            fd0 = open(currUser, O_WRONLY);
                            write(fd0, inputMsg, 80);
                            close(fd0);
                            
                            //printf("WRİTER END\n");
                            printf("\033[A\r[%s] %s: %s", argv[1], argv[2], inputStr);
                            fflush(stdout);
                         
                            //Then read from them

                            free(currUser);
                        }
                    }
                }
                closedir(d);

            }
 
        }
    }
    else {
        while (1) {
            //Parent
            char str[128];
            //printf("HELLO");
            int fd1;
            
            fd1 = open(userName, O_RDONLY);
            read(fd1, str, 128);
            close(fd1);
            printf("%s", str);
            fflush(stdout);


        }
    }
    return 0;
}
