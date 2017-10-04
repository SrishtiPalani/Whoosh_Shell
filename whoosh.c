#include "stdio.h"

int main(int argc, char** argv){
	//whoosh is inherently a loop
	int exit = 0;
	//How long should this be?
	int const MAX_COMMAND_LEN = 128;

	while (!exit){
		printf("whoosh> ");
		char currCommand[MAX_COMMAND_LEN+1];
		//what is the difference between fgets and scanf?
		fgets(currCommand, MAX_COMMAND_LEN, stdin);
	}
}