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
    printf("Welcome to comp304!\n");
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
    
    while(1) {

        lineSize = getline(&inputStr, &len, stdin);

        if (lineSize > 1) {

            DIR *d;
            struct dirent *dir;
            d = opendir(roomName);
            if (d) {
                while((dir = readdir(d)) != NULL) {
                    strcat(inputStr, dir->d_name);
                    if (strcmp(".",dir->d_name) == 0 || strcmp("..", dir->d_name) == 0 ) {
                        continue;
                    }

                    else if (strcmp(argv[2], dir->d_name) == 0) {
                        // WRITER
                        
                    }
                    else {
                        // READERS
                        char *currUser = malloc(sizeof(char)*128);
                        strcat(currUser, roomName);
                        strcat(currUser, "/");
                        strcat(currUser, dir->d_name);
                        
                        //First write in them
                        

                        pid_t pid;
                        pid = fork();
                        if (pid == 0) {
                            //printf("WRİTER\n");
                            
                            
                            int fd0;                    
                            fd0 = open(currUser, O_WRONLY);
                            write(fd0, inputStr, 80);
                            close(fd0);
                            
                            //printf("WRİTER END\n");
                            exit(0);
                        }
                        else {
             
                            //printf("READER %s\n", currUser);
                            char str[128];
                            
                            int fd1;
                            fd1 = open(currUser, O_RDONLY);
                            wait(0);
                            read(fd1, str, 128);
                            close(fd1);
                            

                            //printf("%s: Bunu okudum %s\n", currUser, str);
                        }
                         
                        //Then read from them

                        free(currUser);
                    }
                }
            }
            closedir(d);

            printf("\033[A\r[%s] %s: %s\n", argv[1], argv[2], inputStr);
        }

        lineSize = getline(&inputStr, &len, stdin);
    }

    /*
    DIR *d;
    struct dirent *dir;
    d = opendir(roomName);
    if (d) {
        while((dir = readdir(d)) != NULL) {
            strcat(inputStr, dir->d_name);
            if (strcmp(".",dir->d_name) == 0 || strcmp("..", dir->d_name) == 0 ) {
                continue;
            }

            else if (strcmp(argv[2], dir->d_name) == 0) {
                // WRITER
                
            }
            else {
                // READERS
                char *currUser = malloc(sizeof(char)*128);
                strcat(currUser, roomName);
                strcat(currUser, "/");
                strcat(currUser, dir->d_name);
                
                //First write in them
                

                pid_t pid;
                pid = fork();
                if (pid == 0) {
                    printf("WRİTER\n");
                    
                    
                    int fd0;                    
                    fd0 = open(currUser, O_WRONLY);
                    write(fd0, inputStr, 80);
                    close(fd0);
                    
                    printf("WRİTER END\n");
                    exit(0);
                }
                else {
     
                    printf("READER %s\n", currUser);
                    char str[128];
                    
                    int fd1;
                    fd1 = open(currUser, O_RDONLY);
                    wait(0);
                    read(fd1, str, 128);
                    close(fd1);
                    
                    printf("Bunu okudum %s\n", str);
                }
                 
                //Then read from them

                free(currUser);
            }
        }
    }
    closedir(d);
    */
    // End
    return 0;
}
