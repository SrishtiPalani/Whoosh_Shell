#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>


int running = 1;
int const MAX_COMMAND_LEN = 128;
char** path;
int pathLength;

void reportError() {
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
}
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
	printArray(path, pathLength);
}

char* findExternal(char* commandKey){
	char* location = (char*) malloc(sizeof(char) * MAX_COMMAND_LEN);;
	struct stat fileStat;
	printf("commandKey: %s\n", commandKey);

	for (int i = 0; i < pathLength; i++){
		char testPath[200];
		strcpy(testPath, path[i] + 1);
		strcat(testPath, "/");
		strcat(testPath, commandKey);

		//testPath should be bin/ls
		if(stat(testPath, &fileStat) < 0){

			strcpy(location, "/");
			strcat(location, testPath);
			break;
		}
	}
	return location;
}

int executeExternal(char* command){
	char** argv = (char**) malloc(sizeof(char) * MAX_COMMAND_LEN);
		
	int numArgs = 0;
	char* token = strtok(command, " ");
	for (int i = 0; token != NULL; i++){
		argv[i] = token;
		token = strtok(NULL, " ");
		numArgs++;
	}

	argv[numArgs-1][strlen(argv[numArgs-1])-1] = '\0';

	char* location = findExternal(argv[0]);

	if (location == NULL){
		return 1;
	}
	
	pid_t pid = fork();

	if (pid < 0) {
		return 2;
	} else if (pid > 0) { // I am the parent
		int status;
		wait(&status);
	} else { // I am the child
		execve (location, argv, 0);
  	}

  	return 0;
}
int count_args(char *command) {
	int num_args = 0;
	int length = strlen(command);
	char *str = command;
	int state = 0;
	int i;
	
	for(i=0; i<length; i++) {
		if (str[i] == ' ' || str[i] == '\n') {
			state = 0;
		}
		else if (state == 0) {
			state = 1;
			num_args++;
		}
	}
	//printf("The num_args is:%d \n", num_args);
	return num_args;
}
//parseCommand takes in a command and passes it to the correct handler function.
//it's for the 1 parameter commands such as exit, path and pwd
//returns 0 if the command is valid
//returns 1 if the command is unknown
int parseCommand(char *command, int num_args){
	if (strcmp(command, "exit\n") == 0){
		running = 0;
		return 0;
	}
   else if(strcmp(command,"pwd\n") == 0) {
		char *cwd;
		char buff[PATH_MAX + 1];
		cwd = getcwd(buff, PATH_MAX + 1);
		if(cwd != NULL) {
			printf("You're here: %s\n", cwd);
		}
		else {
			reportError();
		}
		return 0;
	}
   else if (strncmp (command, "path ", 5) == 0 || strncmp (command, "path\n", 5) == 0 ){
		setPath(command+5);
		return 0;
   }
   else if(strncmp(command, "cd\n", 3) == 0 || strncmp(command, "cd ", 3) == 0) {
		char *dir;
		if(num_args == 1) {
			dir = getenv("HOME");
			if(chdir(dir) != 0){
				reportError();
				printf("chdir(dir) is %d\n", chdir(dir));
			}			
		}
		else{
			
			dir = command + 3;
			//printf("dir is %s\n", dir);
			int lineLen = strlen(command);
			if(lineLen > 0){
				if (command[lineLen - 1] == '\n')
	        		command[lineLen - 1] = '\0';
			}
			if(chdir(dir) != 0) {
				printf("is more than 1 chdir(dir) %d\n", chdir(dir));
				reportError();
			}
		}
		return 0;
	}
	
	else{
   		return executeExternal(command);
   }
}



int main(int argc, char** argv){
	//whoosh is inherently a loop
	//How long should this be?
	int const MAX_COMMAND_LEN = 128;
	path = (char**) malloc(sizeof(char) * MAX_COMMAND_LEN);
	path[0] = "/bin";
	pathLength = 1;

	//Checks that only ./whoosh is entered to invoke shell
	if(argc != 1) {
		printf("Error argc !=1. argc is: %d\n", argc);
		reportError();
		exit(1);
	}

	while (running){
		printf("whoosh> ");
		char currCommand[MAX_COMMAND_LEN+1];
		
		//what is the difference between fgets and scanf?
		fgets(currCommand, MAX_COMMAND_LEN, stdin);
		// printf("The currCommand is before:%s \n", currCommand);
		// int lineLen = strlen(currCommand);
		// 	if(lineLen > 0){
		// 		if (currCommand[lineLen - 1] == '\n')
	 //        		currCommand[lineLen - 1] = '\0';
		// 	}

		// if(lineLen > MAX_COMMAND_LEN) {
		// 	reportError();
		// 	continue;
		// }
		// else{
		// 	//printf("The currCommand is after:%s \n", currCommand);
		// }
		
		//do we need to do something if they enter a command that is too long?
		int executionCode = parseCommand(currCommand, count_args(currCommand));
		
	
		if (executionCode != 0){
			printf("whoosh: command not found\n");
			reportError();
			//printf("executionCode is: %d\n", executionCode); 
		}
	}
	exit(0);
}
