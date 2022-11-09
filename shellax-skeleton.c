#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h> // termios, TCSANOW, ECHO, ICANON
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h> /// ME ADDED
#include <sys/stat.h>
const char * sysname = "shellax";
int module_installed = 0;

enum return_codes {
	SUCCESS = 0,
	EXIT = 1,
	UNKNOWN = 2,
    SUCCESS_BACKGROUND = 3,
};

struct command_t {
	char *name;
	bool background;
	bool auto_complete;
	int arg_count;
	char **args;
	char *redirects[3]; // in/out redirection
	struct command_t *next; // for piping
	char *commandStr;
};

/**
 * Prints a command struct
 * @param struct command_t *
 */
void print_command(struct command_t * command)
{
	int i=0;
	printf("Command: <%s>\n", command->name);
	printf("\tIs Background: %s\n", command->background?"yes":"no");
	printf("\tNeeds Auto-complete: %s\n", command->auto_complete?"yes":"no");
	printf("\tRedirects:\n");
	for (i=0;i<3;i++)
		printf("\t\t%d: %s\n", i, command->redirects[i]?command->redirects[i]:"N/A");
        
	printf("\tArguments (%d):\n", command->arg_count);
	for (i=0;i<command->arg_count;++i)
		printf("\t\tArg %d: %s\n", i, command->args[i]);
	if (command->next)
	{
		printf("\tPiped to:\n");
		print_command(command->next);
	}


}
/**
 * Release allocated memory of a command
 * @param  command [description]
 * @return         [description]
 */
int free_command(struct command_t *command)
{
	if (command->arg_count)
	{
		for (int i=0; i<command->arg_count; ++i)
			free(command->args[i]);
		free(command->args);
	}
	for (int i=0;i<3;++i)
		if (command->redirects[i])
			free(command->redirects[i]);
	if (command->next)
	{
		free_command(command->next);
		command->next=NULL;
	}
	free(command->name);
	free(command);
	return 0;
}
/**
 * Show the command prompt
 * @return [description]
 */
int show_prompt()
{
	char cwd[1024], hostname[1024];
    gethostname(hostname, sizeof(hostname));
	getcwd(cwd, sizeof(cwd));
	printf("%s@%s:%s %s$ ", getenv("USER"), hostname, cwd, sysname);
	return 0;
}
/**
 * Parse a command string into a command struct
 * @param  buf     [description]
 * @param  command [description]
 * @return         0
 */
int parse_command(char *buf, struct command_t *command)
{

/////////// MINE START
    char *pointerToPipe = strchr(buf, '|');
    if (pointerToPipe != NULL) {
	char *currComm = malloc(sizeof(char)*(pointerToPipe-buf + 1));
	strncpy(currComm, buf, (pointerToPipe - buf));
	strcat(currComm, "\0");
	int len = strlen(currComm);
	    while (len>0 && strchr(" \t", currComm[len-1])!=NULL)
		    currComm[--len]=0; // trim right whitespace
	command->commandStr = currComm;
    }
    else {
	char*currComm = strdup(buf);
	int len = strlen(currComm);
	    while (len>0 && strchr(" \t", currComm[len-1])!=NULL)
		    currComm[--len]=0; // trim right whitespace
	command->commandStr = currComm;
    }
    ////////// MINE END

	const char *splitters=" \t"; // split at whitespace
	int index, len;
	len=strlen(buf);
	while (len>0 && strchr(splitters, buf[0])!=NULL) // trim left whitespace
	{
		buf++;
		len--;
	}
	while (len>0 && strchr(splitters, buf[len-1])!=NULL)
		buf[--len]=0; // trim right whitespace

	if (len>0 && buf[len-1]=='?') // auto-complete
		command->auto_complete=true;
	if (len>0 && buf[len-1]=='&') // background
		command->background=true;

	char *pch = strtok(buf, splitters);
	command->name=(char *)malloc(strlen(pch)+1);
	if (pch==NULL)
		command->name[0]=0;
	else
		strcpy(command->name, pch);

	command->args=(char **)malloc(sizeof(char *));

	int redirect_index;
	int arg_index=0;
	char temp_buf[1024], *arg;
	while (1)
	{
		// tokenize input on splitters
		pch = strtok(NULL, splitters);
		if (!pch) break;
		arg=temp_buf;
		strcpy(arg, pch);
		len=strlen(arg);

		if (len==0) continue; // empty arg, go for next
		while (len>0 && strchr(splitters, arg[0])!=NULL) // trim left whitespace
		{
			arg++;
			len--;
		}
		while (len>0 && strchr(splitters, arg[len-1])!=NULL) arg[--len]=0; // trim right whitespace
		if (len==0) continue; // empty arg, go for next

		// piping to another command
		if (strcmp(arg, "|")==0)
		{
			struct command_t *c=malloc(sizeof(struct command_t));
			int l=strlen(pch);
			pch[l]=splitters[0]; // restore strtok termination
			index=1;
			while (pch[index]==' ' || pch[index]=='\t') index++; // skip whitespaces

			parse_command(pch+index, c);
			pch[l]=0; // put back strtok termination
			command->next=c;
			continue;
		}

		// background process
		if (strcmp(arg, "&")==0)
			continue; // handled before

		// handle input redirection
		redirect_index=-1;
		if (arg[0]=='<')
			redirect_index=0;
		if (arg[0]=='>')
		{
			if (len>1 && arg[1]=='>')
			{
				redirect_index=2;
				arg++;
				len--;
			}
			else redirect_index=1;
		}
		if (redirect_index != -1)
		{
			command->redirects[redirect_index]=malloc(len);
			strcpy(command->redirects[redirect_index], arg+1);
			continue;
		}

		// normal arguments
		if (len>2 && ((arg[0]=='"' && arg[len-1]=='"')
			|| (arg[0]=='\'' && arg[len-1]=='\''))) // quote wrapped arg
		{
			arg[--len]=0;
			arg++;
		}
		command->args=(char **)realloc(command->args, sizeof(char *)*(arg_index+1));
		command->args[arg_index]=(char *)malloc(len+1);
		strcpy(command->args[arg_index++], arg);
	}
	command->arg_count=arg_index;
	return 0;
}

void prompt_backspace()
{
	putchar(8); // go back 1
	putchar(' '); // write empty over
	putchar(8); // go back 1 again
}
/**
 * Prompt a command from the user
 * @param  buf      [description]
 * @param  buf_size [description]
 * @return          [description]
 */
int prompt(struct command_t *command)
{
	int index=0;
	char c;
	char buf[4096];
	static char oldbuf[4096];

    // tcgetattr gets the parameters of the current terminal
    // STDIN_FILENO will tell tcgetattr that it should write the settings
    // of stdin to oldt
    static struct termios backup_termios, new_termios;
    tcgetattr(STDIN_FILENO, &backup_termios);
    new_termios = backup_termios;
    // ICANON normally takes care that one line at a time will be processed
    // that means it will return if it sees a "\n" or an EOF or an EOL
    new_termios.c_lflag &= ~(ICANON | ECHO); // Also disable automatic echo. We manually echo each char.
    // Those new settings will be set to STDIN
    // TCSANOW tells tcsetattr to change attributes immediately.
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);


    //FIXME: backspace is applied before printing chars
	show_prompt();
	int multicode_state=0;
	buf[0]=0;
  	while (1)
  	{
		c=getchar();
		// printf("Keycode: %u\n", c); // DEBUG: uncomment for debugging

		if (c==9) // handle tab
		{
			buf[index++]='?'; // autocomplete
			break;
		}

		if (c==127) // handle backspace
		{
			if (index>0)
			{
				prompt_backspace();
				index--;
			}
			continue;
		}
		if (c==27 && multicode_state==0) // handle multi-code keys
		{
			multicode_state=1;
			continue;
		}
		if (c==91 && multicode_state==1)
		{
			multicode_state=2;
			continue;
		}
		if (c==65 && multicode_state==2) // up arrow
		{
			int i;
			while (index>0)
			{
				prompt_backspace();
				index--;
			}
			for (i=0;oldbuf[i];++i)
			{
				putchar(oldbuf[i]);
				buf[i]=oldbuf[i];
			}
			index=i;
			continue;
		}
		else
			multicode_state=0;

		putchar(c); // echo the character
		buf[index++]=c;
		if (index>=sizeof(buf)-1) break;
		if (c=='\n') // enter key
			break;
		if (c==4) // Ctrl+D
			return EXIT;
  	}
  	if (index>0 && buf[index-1]=='\n') // trim newline from the end
  		index--;
  	buf[index++]=0; // null terminate string

  	strcpy(oldbuf, buf);

  	parse_command(buf, command);

  	// print_command(command); // DEBUG: uncomment for debugging

    // restore the old settings
    tcsetattr(STDIN_FILENO, TCSANOW, &backup_termios);
  	return SUCCESS;
}
int process_command(struct command_t *command);
// ME
int handle_pipe_and_IO(struct command_t *command);
void pipe_handler(struct command_t *command);
void io_handler(struct command_t *command);
void module_handler(struct command_t *command);
void module_output_adjuster(struct command_t *command);
int main()
{
	while (1)
	{
		struct command_t *command=malloc(sizeof(struct command_t));
		memset(command, 0, sizeof(struct command_t)); // set all bytes to 0
        
        /* ////OLD CODE START
		int code;
		code = prompt(command);
		if (code==EXIT) break;

		code = process_command(command);
		if (code==EXIT) break;
        */ // OLD CODE END
        
        /////////// MINE START
        int code;
        code = prompt(command);
        if(code==EXIT) break;
        code = handle_pipe_and_IO(command);
        if (code == EXIT) break;
        /////////// MINE END

		free_command(command);
	}

	printf("\n");
	return 0;
}

int process_command(struct command_t *command)
{
	int r;
	if (strcmp(command->name, "")==0) return SUCCESS;

	if (strcmp(command->name, "exit")==0) {
        if (module_installed == 1) {
            system("sudo rmmod mymodule2");
            printf("Remove was succesfull\n");
        }
		return EXIT;
    }

	if (strcmp(command->name, "cd")==0)
	{
		if (command->arg_count > 0)
		{
			r=chdir(command->args[0]);
			if (r==-1)
				printf("-%s: %s: %s\n", sysname, command->name, strerror(errno));
			return SUCCESS;
		}
	}
    
    if (strcmp(command->name, "psvis") == 0) {
        if (fork() ==0) {
            module_handler(command);
            exit(0);
        }
        else {
            wait(0);
            module_output_adjuster(command);
            module_installed=1;
        }
        return SUCCESS;
    }

	pid_t pid=fork();
	if (pid==0) // child
	{
		/// This shows how to do exec with environ (but is not available on MacOs)
	    // extern char** environ; // environment variables
		// execvpe(command->name, command->args, environ); // exec+args+path+environ

		/// This shows how to do exec with auto-path resolve
		// add a NULL argument to the end of args, and the name to the beginning
		// as required by exec
        
        /* ////////////////// OLD CODE START
		// increase args size by 2
		command->args=(char **)realloc(
			command->args, sizeof(char *)*(command->arg_count+=2));

		// shift everything forward by 1
		for (int i=command->arg_count-2;i>0;--i)
			command->args[i]=command->args[i-1];

		// set args[0] as a copy of name
		command->args[0]=strdup(command->name);
		// set args[arg_count-1] (last) to NULL
		command->args[command->arg_count-1]=NULL;

		execvp(command->name, command->args); // exec+args+path
		exit(0);
		*/ ////////////////// OLD CODE END

        /// TODO: do your own exec with path resolving using execv()
            
            
        //////////////////////////////////////////
        if (!command->background) {

            command->args=(char **)realloc(
                command->args, sizeof(char *)*(command->arg_count+=2));

            // shift everything forward by 1
            for (int i=command->arg_count-2;i>0;--i)
                command->args[i]=command->args[i-1];
           
            // set args[0] as a copy of name
            command->args[0]=strdup(command->name);
            // set args[arg_count-1] (last) to NULL
            command->args[command->arg_count-1]=NULL;

            //printf("Hello guys\n");
            //for (int i = 0; i < command->arg_count -1; i++) {
            //    printf("%s\n", command->args[i]);
            //}
            //printf("Listed element : %d\n", command->arg_count); 
            
            char* general_command_path = (char*) malloc(sizeof(char)*(6 + strlen(command->name)) );
            char* local_command_path = (char*) malloc(sizeof(500));
            char *path = realpath(command->name, NULL);
            
            if (path == NULL) {
                // PATH : i.e. ls
                //printf("Cannot find the file with name %s\n", command->name);

                
                char *Path = getenv("PATH");
                char *token = strtok(Path, ":"); 
                
                while (token != NULL) {
                    
                    char *path_to_command = malloc(sizeof(char)*150);
                    strcat(path_to_command, token);
                    strcat(path_to_command, "/");
                    strcat(path_to_command, command->name);
                    
                    if (access(path_to_command, X_OK)) {
                        //printf("NO\n");
                        //continue;
                    }
                    else {
                        //printf("YES\n");
                        free(path);
                        free(general_command_path);
                        free(local_command_path);
                        execv(path_to_command, command->args++);
                        exit(0); 
                    }
                    
                    free(path_to_command);
                    token = strtok(NULL, ":");
                }
                
                /* 
                strcat(general_command_path, "/bin/");
                strcat(general_command_path, command->name);
                //printf("Command Name : %s\n", command->name);
                execv(general_command_path, command->args++);
                free(general_command_path);
                exit(0);
                */
            }
            else {
                // local commands : i.e. ./dummy.out
                //printf("ELSE E GİRDİM \n");
                strcat(local_command_path, path);
                //printf("loc_name2 %s\n", loc_name2);
                execv(local_command_path, command->args++);
                free(path);
                free(local_command_path);
                free(general_command_path);
                exit(0);
            }
        }
        else {
            pid_t pid2 = fork();
            if (pid2 == 0) {
                command->args=(char **)realloc(
                    command->args, sizeof(char *)*(command->arg_count+=2));

                // shift everything forward by 1
                for (int i=command->arg_count-2;i>0;--i)
                    command->args[i]=command->args[i-1];
               
                // set args[0] as a copy of name
                command->args[0]=strdup(command->name);
                // set args[arg_count-1] (last) to NULL
                command->args[command->arg_count-1]=NULL;

                //printf("Hello guys\n");
                //for (int i = 0; i < command->arg_count -1; i++) {
                //    printf("%s\n", command->args[i]);
                //}
                //printf("Listed element : %d\n", command->arg_count); 
                
                char* general_command_path = (char*) malloc(sizeof(char)*(6 + strlen(command->name)) );
                char* local_command_path = (char*) malloc(sizeof(500));
                char *path = realpath(command->name, NULL);
                
                if (path == NULL) {
                    // PATH : i.e. ls
                    //printf("Cannot find the file with name %s\n", command->name);

                    
                    char *Path = getenv("PATH");
                    char *token = strtok(Path, ":"); 
                    
                    while (token != NULL) {
                        
                        char *path_to_command = malloc(sizeof(char)*150);
                        strcat(path_to_command, token);
                        strcat(path_to_command, "/");
                        strcat(path_to_command, command->name);
                        
                        if (access(path_to_command, X_OK)) {
                            //printf("NO\n");
                            //continue;
                        }
                        else {
                            //printf("YES\n");
                            free(path);
                            free(general_command_path);
                            free(local_command_path);
                            execv(path_to_command, command->args++);
                            exit(0); 
                        }
                        
                        free(path_to_command);
                        token = strtok(NULL, ":");
                    }
                    
                    /* 
                    strcat(general_command_path, "/bin/");
                    strcat(general_command_path, command->name);
                    //printf("Command Name : %s\n", command->name);
                    execv(general_command_path, command->args++);
                    free(general_command_path);
                    exit(0);
                    */
                }
                else {
                    // local commands : i.e. ./dummy.out
                    //printf("ELSE E GİRDİM \n");
                    strcat(local_command_path, path);
                    //printf("loc_name2 %s\n", loc_name2);
                    execv(local_command_path, command->args++);
                    free(path);
                    free(local_command_path);
                    free(general_command_path);
                    exit(0);
                }
            }
            else {
                // Parent
                exit(0);
            }
        }
        /////////////////////////////////////////
        
	}
	else
	{
    // TODO: implement background processes here
        /*  ////////////////////////////////// OLD CODE  START
        wait(0); // wait for child process to finish
		return SUCCESS;
        */  ////////////////////////////////// OLD CODE END
        
        //print_command(command);

        wait(0);
        return SUCCESS;

        /*
        printf("My last arg is %s\n", command->args[command->arg_count -1]);        
        return SUCCESS;
	    */
    }

	// TODO: your implementation here

	printf("-%s: %s: command not found\n", sysname, command->name);
	return UNKNOWN;
}


int handle_pipe_and_IO(struct command_t *command){
    if (command->next == NULL && command->redirects[0] == NULL &&  command->redirects[1] == NULL &&  command->redirects[2] == NULL ) {
        printf("OKAY\n");
        return process_command(command);
    }
    else if (command->next != NULL) {
        // IT CONTAINS PIPE
        printf("IT CONTAINS PIPE\n");
        
        /* 
        struct command_t *currCommand = command;
        while (currCommand->next != NULL) {
            print_command(currCommand);
            currCommand =currCommand->next;
        }*/

        pipe_handler(command);
        return SUCCESS; // ? 
    }
    else if (command->redirects[0] != NULL ||  command->redirects[1] != NULL ||  command->redirects[2] != NULL ) {
        printf("IT CONTAINS IO\n");
        io_handler(command);
        return SUCCESS; // ? 
    }
    return SUCCESS;
}

void pipe_handler(struct command_t *command) {
    int pipefd[2];
    int infd;    
    int processCounter = 0;

    struct command_t *currCommand = command;

    while (currCommand != NULL) {
        //printf("It is working %d\n", processCounter);
        
        if (pipe(pipefd) == -1) {
            perror("pipe");
        }
        

        pid_t pid;
        pid = fork();

        if (pid == -1) {
            perror("fork");
            return;
        }
        else if (pid == 0) {
            
            if (processCounter != 0) {
                // stdin
                dup2(infd, 0);
            }
            
            if (currCommand->next != NULL) {
                // stdout
                dup2(pipefd[1], 1);
            }
            
            process_command(currCommand);
            exit(0);
        }
        else {
            wait(0);
            infd = pipefd[0];
            close(pipefd[1]);
        }
        
        currCommand = currCommand->next;
        processCounter++;
    }
    
}

void io_handler(struct command_t *command) { // Cannot handle input commands, command needs to be at the beginnig to work
    // Guideline:
    // get the min index of IO redirection [before which args]
    // then using  dup and dup2 (dup is encourgaed) 
    if (command->redirects[0] == NULL &&  command->redirects[1] == NULL &&  command->redirects[2] == NULL ) {
        printf("OKAY2\n");
    }
    else {
        // Find the beginning of IO command

        print_command(command);

        char* fileStart;
        if (strchr(command->commandStr, '>') == NULL) fileStart = strchr(command->commandStr, '<'); 
        else if (strchr(command->commandStr, '<') == NULL) fileStart = strchr(command->commandStr, '>'); 
        else if (strchr(command->commandStr, '>') < strchr(command->commandStr, '<')) fileStart = strchr(command->commandStr, '>');        
        else fileStart = strchr(command->commandStr, '<');
        
        // Copy the string till IO command start
        char *currComm = malloc(sizeof(char)*(fileStart - command->commandStr + 1));
        strncpy(currComm, command->commandStr, (fileStart - command->commandStr));
        strcat(currComm, "\0");
        
        // Create a sub command
        // i.e = cat naber.txt > a.txt ------> subCommand : cat naber.txt
		struct command_t *subCommand = (struct command_t*) malloc(sizeof(struct command_t));
		memset(subCommand, 0, sizeof(struct command_t)); // set all bytes to 0        
        parse_command(currComm, subCommand);
        
        //copy command->commandStr into a string and parse it for IO, and calculate IO redirection
        /*
        char* commandStrCpy = strdup(command->commandStr);
        
        char *ioPointer;
        int ioCounter = 0;
        if (strchr(commandStrCpy, '>') == NULL) ioPointer = strchr(commandStrCpy, '<'); 
        else if (strchr(commandStrCpy, '<') == NULL) ioPointer = strchr(commandStrCpy, '>'); 
        else if (strchr(commandStrCpy, '>') < strchr(commandStrCpy, '<')) ioPointer = strchr(commandStrCpy, '>');        
        else ioPointer = strchr(commandStrCpy, '<');
        while(ioPointer != NULL) {
            if(ioPointer[0] == '>' && ioPointer[1] == '>') ioPointer++;
            //if(ioPointer[0] == '>' && ioPointer[1] == '<') perror("><"); MAYBE NEED TO HANDLE THESE
            //if(ioPointer[0] == '<' && ioPointer[1] == '>') perror("<>");

            ioPointer++;
            if (strchr(ioPointer, '>') == NULL) ioPointer = strchr(ioPointer, '<'); 
            else if (strchr(ioPointer, '<') == NULL) ioPointer = strchr(ioPointer, '>'); 
            else if (strchr(ioPointer, '>') < strchr(ioPointer, '<')) ioPointer = strchr(ioPointer, '>');        
            else ioPointer = strchr(ioPointer, '<');
            
            //sleep(1);
            //printf("IO POINTER %s\n", ioPointer);
            ioCounter++;
        }
        */ 

        //Begin further parse and IO direction
        char* commandString = strdup(command->commandStr);

        char *ioPoint;
        int ioCount=0; 
        int whichIO = -1; // 0 in 1 out 2 append
        
        int infd;
        int pipefd[2];


        if (strchr(commandString, '>') == NULL) ioPoint = strchr(commandString, '<'); 
        else if (strchr(commandString, '<') == NULL) ioPoint = strchr(commandString, '>'); 
        else if (strchr(commandString, '>') < strchr(commandString, '<')) ioPoint = strchr(commandString, '>');        
        else ioPoint = strchr(commandString, '<');
        while (ioPoint != NULL) {
            //pipe
            pipe(pipefd);

            if (ioPoint[0] == '>' && ioPoint[1] == '>') {
                whichIO = 2;
                ioPoint++;
            }
            else if(ioPoint[0] == '>') {
                whichIO = 1;
            }
            else if (ioPoint[0] == '<') {
                whichIO = 0;
            }

            ioPoint++;
            
            char* nextIO;
            if(strchr(ioPoint, '>') == NULL) nextIO = strchr(ioPoint, '<');
            else if(strchr(ioPoint, '<') == NULL) nextIO = strchr(ioPoint, '>');
            
            char* fileName;
            if (nextIO != NULL) {
                fileName = malloc(sizeof(char)* (nextIO - ioPoint + 1));
                strncpy(fileName, ioPoint , nextIO - ioPoint);
                strcat(fileName, "\0");                
            }
            else {
                fileName = strdup(ioPoint);
            }
            int len = strlen(fileName);
            while (len>0 && strchr(" \t", fileName[0])!=NULL) // trim left whitespace
            {
                fileName++;
                len--;
            }
	        while (len>0 && strchr(" \t", fileName[len-1])!=NULL)
		        fileName[--len]=0; // trim right whitespace
            //printf("Filename %s\n", fileName);            

            if (strchr(ioPoint, '>') == NULL) ioPoint = strchr(ioPoint, '<'); 
            else if (strchr(ioPoint, '<') == NULL) ioPoint = strchr(ioPoint, '>'); 
            else if (strchr(ioPoint, '>') < strchr(ioPoint, '<')) ioPoint = strchr(ioPoint, '>');        
            else ioPoint = strchr(ioPoint, '<');
           
            

            if (whichIO == 0 && ioCount == 0) {
            }
            else if (whichIO == 1) {
                int file_fd = open(fileName, O_RDWR | O_CREAT, 0666);
                
                int pid = fork();
                if (pid == 0) {
                    dup2(file_fd, 1);
                    
                    process_command(subCommand);
                    close(file_fd);
                    
                    exit(0);
                }
                else {
                    wait(0);
                    infd = pipefd[0];
                    close(pipefd[1]);
                }

            }
            else if (whichIO == 2 && ioCount == 0) {
                int file_fd = open(fileName, O_RDWR | O_CREAT | O_APPEND, 0666);
                
                int pid = fork();
                if (pid == 0) {
                    dup2(file_fd, 1);
                    
                    process_command(subCommand);
                    close(file_fd);
                    
                    exit(0);
                }
                else {
                    wait(0);
                    infd = pipefd[0];
                    close(pipefd[1]);
                }


            }

            ioCount++;
        }
        
    }
}


void module_handler(struct command_t *command) {
    
    if (module_installed == 0) {
        
        char input[100] = "sudo insmod mymodule2.ko processID=";
        strcat(input, command->args[0]);

        system(input);
        printf("Succesfully installed module\n");
        module_installed = 1;
    }
    else {
        printf("Module was intalled before\n");
    }
    
    int fd = open("trick.txt", O_WRONLY | O_APPEND | O_CREAT);
    dup2(fd, 1);
    system("dmesg");
    close(fd);
    

    //printf("Dmesg Success\n");
    chmod("trick.txt", S_IRUSR | S_IWUSR);
   
}


void module_output_adjuster(struct command_t *command) {
    // FİLE düzenle
    system("cat trick.txt | grep \"Loading Module\" -n | tail -1  > trick2.txt" );
    FILE *secondFile = fopen("trick2.txt", "r");
    char line[1000];
    
    int lineCounter = 0;
    long lineInt = -1;
    char buffer[10];
    char *ptr;
    while(fgets(line, 1000, secondFile)) {
        printf("%s", line);
        char *toPtr = strchr(line, ':');
        strncat(buffer, line, toPtr-line);
        lineInt = strtol(buffer, &ptr, 10);
    }
    printf("%s and %ld\n", buffer, lineInt);
    fclose(secondFile);
    // Now read the trick and adjust the output
    
    FILE *trickFile = fopen("trick.txt", "r"); 
    FILE *outputFile = fopen("trick2.txt", "w");
    lineCounter = 0;
    fputs("digraph {\n", outputFile);
    while (fgets(line, 1000, trickFile)) {
        if (lineCounter < lineInt) {
            
        }
        else {
            
            fputs(&line[16], outputFile);
        }
        lineCounter++;
    }
    fputs("}", outputFile);
    fclose(trickFile);
    fclose(outputFile);

    char input[100] = "cat trick2.txt | dot -Tpng >";
    strcat(input, command->args[1]);
    system(input);
    remove("trick.txt");
    //remove("trick2.txt");

}








