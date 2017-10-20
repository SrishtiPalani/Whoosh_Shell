//Briar and Srishti 
//10/19/2017
//WHOOOSH is basically an interactive loop: 
//it repeatedly prints a prompt "whoosh> ",
// parses the input, executes the command specified on that line of input, 
//and waits for the command to finish. This is re- peated until the user types "exit".

//include statements
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

//Global Variables
//the running state of the 
int running = 1;
//The maximum length of a line of input to the shell is 128 bytes.
int const MAX_COMMAND_LEN = 128;
//the path 
char** path;
//the length of the path 
int pathLength;

//this one and only error message that prints whenever it encounters an error of any type
void reportError() {
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
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

//Looks through each location in the path and 
//checks to see if the command exists there 
//if so returns its locations 
//else returns an empty string
char* findExternal(char* commandKey){
	char* location = (char*) malloc(sizeof(char) * MAX_COMMAND_LEN);;
	struct stat fileStat;
	//printf("commandKey: %s\n", commandKey);

	for (int i = 0; i < pathLength; i++){
		char testPath[200];
		strcpy(testPath, path[i]);
		strcat(testPath, "/");
		strcat(testPath, commandKey);

		//testPath should be bin/ls
		if(stat(testPath, &fileStat) == 0){
			//printf("the stat is: %d\n", stat(testPath, &fileStat)); 
			// strcpy(location, "/");
			strcpy(location, testPath);
			//strcat(location, testPath);
			break;
		}
	}
	return location;
}

//Tokenizes the command into an array 
//calls findExternal to check if the command exists there
//if so, executes it 
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
	//printf("location is: %s\n", location); 

	if (location[0] != '/'){
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
  	free(argv);
  	//free(location);
  	return 0;
}

//this function calculates the number arguments in the command
int count_args(char *command) {
	int num_args = 0;
	int length = strlen(command);
	int state = 0;
	int i;
	
	for(i=0; i<length; i++) {
		if (command[i] == ' ' || command[i] == '\n') {
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
//returns 1 if the command is unknown - and reports error
//Built-in commands
int parseCommand(char *command, int num_args){
	//exit command
	if (strcmp(command, "exit\n") == 0){
		running = 0;
		return 0;
	}
	//pwd command
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
	//path command
   else if (strncmp (command, "path ", 5) == 0 || strncmp (command, "path\n", 5) == 0 ){
		setPath(command+5);
		return 0;
   }
   //cd command
   else if(strncmp(command, "cd\n", 3) == 0 || strncmp(command, "cd ", 3) == 0) {
		char *dir;
		//if only cd is passed in
		if(num_args == 1) {
			dir = getenv("HOME");
			if(chdir(dir) != 0){
				reportError();
				//printf("chdir(dir) is %d\n", chdir(dir));
			}			
		}
		else{
			//if cd and another directory is passed in
			dir = command + 3;
			//printf("dir is %s\n", dir);
			int lineLen = strlen(command);
			if(lineLen > 0){
				if (command[lineLen - 1] == '\n')
	        		command[lineLen - 1] = '\0';
			}
			//else reports error
			if(chdir(dir) != 0) {
				//printf("is more than 1 chdir(dir) %d\n", chdir(dir));
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

	//How long should the commmand be?
	int const MAX_COMMAND_LEN = 128;
	//allocate enough memory for the command
	path = (char**) malloc(sizeof(char) * MAX_COMMAND_LEN);
	///the path is /bin intialized
	path[0] = "/bin";
	//the pathlenght initially has 1 item in it
	pathLength = 1;

	//Checks that only ./whoosh is entered to invoke shell
	if(argc != 1) {
		//printf("Error argc !=1. argc is: %d\n", argc);
		reportError();
		exit(1);
	}

	//while the user has not said exit
	//when the user types exit we set it to false
	while (running){
		//prompt prints this
		printf("whoosh> ");
		//the command is stored in an array
		char currCommand[MAX_COMMAND_LEN+1];
		
		//what is the difference between fgets and scanf?
		//gets the command from the command line and stores it in the array
		fgets(currCommand, MAX_COMMAND_LEN, stdin);
		//printf("The currCommand is before:%s \n", currCommand);
		int lineLen = strlen(currCommand);
				if(lineLen > MAX_COMMAND_LEN) {
			reportError();
			continue;
		}
		
		//do we need to do something if they enter a command that is too long?
		//executionCode is set to 0 when there is a command that is found in our parseCommand function
		int executionCode = parseCommand(currCommand, count_args(currCommand));

		//if the built-in command is not found in our parseCommand function it returns 1
		if (executionCode != 0){
			//printf("whoosh: command not found\n");
			reportError();
			//printf("executionCode is: %d\n", executionCode); 
		}
	}
	//frees path memory
	free(path);
	exit(0);
}
