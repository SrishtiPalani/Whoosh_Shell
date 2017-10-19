#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>


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
	char* location = NULL;
	struct stat fileStat;

	for (int i = 0; i < pathLength; i++){
		char* testPath = path[i] + 1;
		testPath = strcat(testPath, strcat("/", commandKey));
		if(stat(testPath, &fileStat) < 0){
			location = strcat("/", testPath);
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

	printf("Location is %s.\n",location );
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

//parseCommand takes in a command and passes it to the correct handler function.
//returns 0 if the command is valid
//returns 1 if the command is unknown
int parseCommand(char *command){
	if (strcmp(command, "exit\n") == 0){
		running = 0;
		return 0;
	}
   else if(strcmp(command,"pwd\n") == 0) {
		char *cwd;
		char buff[PATH_MAX + 1];
	
		cwd = getcwd(buff, PATH_MAX + 1);

		if(cwd != NULL) {
			printf("%s\n", cwd);
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

   else{
   		return executeExternal(command);
   }

	return 1;
}



int main(int argc, char** argv){
	path = (char**) malloc(sizeof(char) * MAX_COMMAND_LEN);
	path[0] = "/bin";
	pathLength = 1;

	while (running){
		printf("whoosh> ");
		char currCommand[MAX_COMMAND_LEN+1];
		//what is the difference between fgets and scanf?
		fgets(currCommand, MAX_COMMAND_LEN, stdin);

		//do we need to do something if they enter a command that is too long?
		int executionCode = parseCommand(currCommand);
		if ( executionCode != 0){
			reportError();
		}
	}
	exit(0);
}
