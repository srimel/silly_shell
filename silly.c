#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*
			Stuart Rimel, CS333 Homework #3: Silly Shell

	The Purpose of this program is to implement a basic Unix shell program
	that supports execution of simple commands that run in the forground and 
	background processes. This program will also implement the built-in
	command "exit" to terminate the shell session. 

	The strip now works I thinks
*/

#define SCREEN 100 //# of lines to clear screen with
#define BLANK -2   //code value that input was only blank spaces

void clearScreen();
char * strip(char * input);
char ** parse(char *input);
int count_tokens(char * input);

int
main(int argc, char *argv[])
{
	char *command_line = NULL, **new_argv = NULL;
	int cond = 0; 

	clearScreen();
	printf("Welcome to Silly Shell!\n\n\n\n");
	do {
		printf(">>> ");
		if(!scanf("%m[^\n]%*c",&command_line)) //allocates memory 
			perror("Error:");

		char * test = strip(command_line); //testing strip functionality
		if(test)
			printf("stipped input: %s\n",test);

		int size = count_tokens(test);
		new_argv = parse(test);
		if(new_argv) {
			printf("Arguments:\n");
			for(int i = 0; i < size; i++) {
				printf("%s\n",new_argv[i]);
			}
		}

		cond = strcmp("exit",test);  //loop exit condition

		//Garbage collection
		if(new_argv) {
			for(int i = 0; i < size; i++) {
				if(new_argv[i])
					free(new_argv[i]);
				new_argv[i] = NULL;
			}
			free(new_argv);
			new_argv = NULL;
		}
		if(command_line) {
			free(command_line);
			command_line = NULL;
		}
		if(test) {
			free(test);
			test = NULL;
		}

	}while(cond);

	printf("Session terminated...\n");
	return 0;
}

//Parses the input string into separate strings in a 2d array which is returned
//upon success, otherwise will return a null string. 
//TODO: supply back the real number of tokens as well.
//
char ** parse(char *input)
{
	char ** argv = '\0';
	if(!input) return argv;
	int size = count_tokens(input);
	char *token = strtok(input," "); 
	if(!token) return argv;

	if(size) {
		argv = (char**) malloc(sizeof(char*)*size);
		for(int i = 0; i < size; i++) {
			argv[i] = NULL;
		}
		int i = 0;
		while(token && i < size) {
			argv[i] = (char*) malloc(sizeof(char)*(strlen(token)+1));
			strcpy(argv[i], token);
			token = strtok(NULL," ");
			i++;
		}
	}

	return argv;
}

//Under best input, tokens will be equal to space due to first argument being
//a command
//TODO: account for two or more spaces inbetween words
int count_tokens(char * input) 
{
	if(!input) return 0;
	int count = 0;
	for(int i = 0; i < strlen(input); i++) {
		if(input[i] == ' ') 
			count++;
	}
	return count + 1;

}

void clearScreen()
{
	for(int i = 0; i < SCREEN; i++) {
		putchar('\n');
	}
}

//Strips leading and trailing whitespace from the input array.
char * strip(char * input)
{
	if(!input) return input;

	int len = strlen(input);
	char * temp = (char*) malloc(sizeof(char)*(len+1));

	int i = 0;
	while(i < len && input[i] == ' ')  //strips leading whitespace
		i++;

	if(i >= len)
		strcpy(temp,input);
	else
		strcpy(temp,input+i);

	int length = strlen(temp);
	for(int i = length-1; i < length; i--)  //strips trailing whitespace
	{
		if(temp[i] == ' ')
			temp[i] = '\0';
		else
			break;
	}
	return temp;
}
