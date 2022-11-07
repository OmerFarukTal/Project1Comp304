#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

//void executeCommand(char *command, char** args);

int main(int argc, char **argv) {
    
    // insert into kernel modules
    
    pid_t pid = fork();
    
    if (pid == 0) {
        char *sudoCommand = "sudo";
        char *insArgs[5] = {"sudo", "insmod", "mymodule2.ko", "processID=1", NULL};

        printf("Ins Succes Before\n");
        //executeCommand(sudoCommand, insArgs);
        execvp(sudoCommand, insArgs);
        exit(0);
    }
    else {
        wait(0);
        printf("Ins Succes\n");
    }
    
     // EX
    pid = fork();

    if (pid) {
        int fd = open("trick.txt", O_WRONLY | O_APPEND | O_CREAT);
        dup2(fd,1);
        char *dmesgCommand = "dmesg";
        char *dmesgArgs[2] = {"dmesg", NULL};
        execvp(dmesgCommand, dmesgArgs);
        close(fd);

        exit(0);

        /*
        char *command = "cat";
        char *args[3] = {"cat", "naber.txt", NULL};
        
        int fd = open("tricky.txt", O_RDWR | O_APPEND | O_CREAT);
        dup2(fd, 1);
        //executeCommand(command, args);
        execvp(command, args);
        close(fd);
        */
    }
    else {
        wait(0);
        printf("Dmesg Success\n");
        
        chmod("trick.txt", S_IRUSR | S_IWUSR);

        //int fd = open("trick.txt", O_RDONLY);
        
    }
    

    // read the output of dmegs
    /*    
    int fd = open("trick.txt", O_WRONLY | O_APPEND | O_CREAT);
    dup2(fd,1);
    char *dmesgCommand = "dmesg";
    char *dmesgArr[2] = {"dmesg", NULL};
    executeCommand(dmesgCommand, dmesgArr);
    close(fd);
    printf("Dmesg Succes\n");
    */

    
    // remove it from the kernel modules
    pid = fork();
    if (pid == 0) {
        char *sudoCommand = "sudo";
        char *rmArgs[4] = {"sudo", "rmmod", "mymodule2", NULL};
        execvp(sudoCommand, rmArgs);
        exit(0);
        //executeCommand(sudoCommand, rmArgs);
        //remove("trick.txt");
    }
    else {
        wait(0);
        printf("Rmmod Succes\n");

    }
    return 0;
}


    
/*
void executeCommand(char *command, char** args) {
 
    char *Path = getenv("PATH");
    char *token = strtok(Path, ":");


    while (token != NULL) {

        char *path_to_command = malloc(sizeof(char)*150);
        strcat(path_to_command, token);
        strcat(path_to_command, "/");
        strcat(path_to_command, command);

         if (access(path_to_command, X_OK)) {
            //PASS    
         }
         else {
             execv(path_to_command, args);
             printf("Hello I am executed\n");
             break;
         }

        free(path_to_command);
        token = strtok(NULL, ":");
    }
       

}

*/
