#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/*
				Stuart Rimel, CS333 Homework #3: Silly Shell

	The Purpose of this program is to implement a basic Unix shell program
	that supports execution of simple commands that run in the forground and 
	background processes. This program will also implement the built-in
	command "exit" to terminate the shell session. 
*/

#define SCREEN 100 //# of lines to clear screen with

void clearScreen();

int
main(int argc, char *argv[])
{
	char *command_line;
	int cl_size = 0, cond = 0; //cond is for the loop condition

	clearScreen();
	printf("Welcome to Silly Shell!\n\n\n\n");
	do {
		printf(">>> ");
		if(!scanf("%m[^\n]%*c",&command_line)) //allocates memory
			perror("Error:");
		cl_size = strlen(command_line);
		cond = strcmp("exit",command_line);
		free(command_line);
	}while(cond);

	printf("Session terminated...\n");
	return 0;
}

void clearScreen()
{
	for(int i = 0; i < SCREEN; i++) {
		putchar('\n');
	}
}
