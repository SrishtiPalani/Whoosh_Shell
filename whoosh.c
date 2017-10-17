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

void reportError() {
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
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
	return 1;
}



int main(int argc, char** argv){
	//whoosh is inherently a loop
	
	//How long should this be?
	int const MAX_COMMAND_LEN = 128;

	while (running){
		printf("whoosh> ");
		char currCommand[MAX_COMMAND_LEN+1];
		//what is the difference between fgets and scanf?
		fgets(currCommand, MAX_COMMAND_LEN, stdin);

		//do we need to do something if they enter a command that is too long?
		int executionCode = parseCommand(currCommand);
		if ( executionCode == 1){
			printf("whoosh: command not found\n");
		}
	}
	exit(0);
}
