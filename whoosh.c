#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

char** path;
int pathLength;

int running = 1;


void printArray(char** arr, int arrLength){
	printf("printing Array\n");
	for (int i = 0; i < arrLength; i++){
		printf("%s\n", arr[i]);
	}
}

//takes in a path (stripped of "path ") and puts each entry into the path variable
void setPath(char* pathCommand){
	char* token = strtok(pathCommand, " ");
	for (int i = 0; token != NULL; i++){
		path[i] = token;
		token = strtok(NULL, " ");
		pathLength = i + 1;
	}
	path[pathLength-1][strlen(path[pathLength-1])-1] = '\0';
}

//parseCommand takes in a command and passes it to the correct handler function.
//returns 0 if the command is valid
//returns 1 if the command is unknown
int parseCommand(char *command){
	if (strcmp (command, "exit\n") == 0){
		running = 0;
		return 0;
	}
<<<<<<< HEAD
	else if (strncmp (command, "path ", 5) == 0 || strncmp (command, "path\n", 5) == 0 ){
		setPath(command+5);
		printArray(path, pathLength);
		return 0;
	}
	return 1;
}



int pwd(char **command) {
=======
>>>>>>> origin/master
	
    else if(strcmp(command[0], "pwd\n") == 0) {
		
		char *cwd;
		char buff[PATH_MAX + 1];
	
		cwd = getcwd(buff, PATH_MAX + 1);
		if(cwd != NULL) {
			printf("%s\n", cwd);
		}
		else {
			printf("Error pwd didn't work"); 
			//error_msg();
		}
		return 0;
	}
	else {
			dir = command[1];
			
			if(chdir(dir) != 0) {
				printf("Error cd didn't work"); 
				//error_msg();
			}
		}
		return 0;
	}

	return 1;
}*/


int main(int argc, char** argv){
	//whoosh is inherently a loop
	
	//How long should this be?
	int const MAX_COMMAND_LEN = 128;
	path = (char**) malloc(sizeof(char) * MAX_COMMAND_LEN);

	while (running){
		printf("whoosh> ");
		char currCommand[MAX_COMMAND_LEN+1];
		//what is the difference between fgets and scanf?
		fgets(currCommand, MAX_COMMAND_LEN, stdin);

		//do we need to do something if they enter a command that is too long?
		int executionCode = parseCommand(currCommand, );
		if ( executionCode == 1){
			printf("whoosh: command not found\n");
		}
	}
	exit(0);
}
