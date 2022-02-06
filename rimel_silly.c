#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
			Stuart Rimel, CS333 Homework #3: Silly Shell

	The Purpose of this program is to implement a basic Unix shell program
	that supports execution of simple commands that run in the forground and 
	background processes. This program will also implement the built-in
	command "exit" to terminate the shell session. 
*/

#define SCREEN 100 //# of lines to clear screen with
#define BLANK -2   //code value that input was only blank spaces

void clearScreen();
char * strip(char * input, int * bg);
char ** parse(char *input);
int count_tokens(char * input);
void welcome();
void insertPrompt();
void printArgs(char ** new_argv, int size);
void destroyArgs(char **new_argv, int size, char *stripped, char *command_line);
void termination_handler(int signum);
void foreground(char ** new_argv);
void background(char ** new_argv);

int
main(int argc, char *argv[])
{
	char **new_argv = NULL;
	int cond = 0, size = 0;
	struct sigaction new_action, old_action;

	//Resets how SIGCHLD will be handled via termination_handler()
	new_action.sa_handler = termination_handler;
	sigemptyset (&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction (SIGCHLD, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN) {
		sigaction (SIGCHLD, &new_action, NULL);
	}

	welcome();

	do 
	{
		int bg = 0;
		char *command_line = NULL;
		insertPrompt();
		if(!scanf("%m[^\n]%*c",&command_line)) { //allocates memory
			cond = 1;
			while(getchar() != '\n');
			continue; //allows '\n' to be entered and handled by shell
		}
		char * stripped = strip(command_line,&bg); 
		size = count_tokens(stripped);
		new_argv = parse(stripped);

		if(!bg)
			foreground(new_argv);
		else
			background(new_argv);

		//Exits main loop if and only if "exit" is typed.
		if(stripped)
			cond = strcmp("exit",stripped);

		destroyArgs(new_argv, size, stripped, command_line);
	}while(cond);

	printf("Session terminated...\n");
	return 0;
}

//Forks a child process to run in the foreground. Parent process waits for 
//child to terminate and reaps child with waitpid(). 
//
void foreground(char ** new_argv)
{
	int status;
	pid_t cpid, w;
	cpid = fork();
	if(cpid == -1) 
	{
		perror("Fork Error:");
		exit(EXIT_FAILURE);
	}
	if(cpid == 0) //Child process
	{
		execvp(new_argv[0],new_argv);
		exit(EXIT_FAILURE);
	} 
	else          //Parent process
	{
		w = waitpid(cpid,&status,0);
		if(w == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
		if (WIFEXITED(status)) {
			if(WEXITSTATUS(status)) {
				if(strcmp(new_argv[0], "exit"))
					printf("'%s' command not found...\n",new_argv[0]);
			}
		} 
	}
}

//Forks a child process to run in the background. Child process is reaped
//via termination handler in the background.
//
void background(char ** new_argv)
{
	pid_t cpid; 
	cpid = fork();
	if(cpid == -1) 
	{
		perror("Fork Error:");
		exit(EXIT_FAILURE);
	}
	if(cpid == 0)   //Child process
	{
		execvp(new_argv[0],new_argv);
		exit(EXIT_FAILURE);
	} 
	//Parent process returns from this function.
}

//Parses the input string into separate strings in a 2d array which is returned
//upon success, otherwise will return a null string. 
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

//Counts the number of tokens present in the raw command line string input.
//Tokens are delimited by ' '.
//
int count_tokens(char * input) 
{
	if(!input) return 0;
	int count = 0;
	int i = 0;
	while(i < strlen(input)) {
		if(input[i] == ' ') {
			count++;
			while(input[i] == ' ')
				i++;
		}
		else
			i++;
	}
	return count + 1;

}

//Strips leading and trailing whitespace from the input array.
//Also parses the '&' symbol and will set bg = 1 if found.
//
char * strip(char * input, int * bg)
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
		char curr = temp[i];
		if(curr == '&') *bg = 1;
		if(curr == ' ' || curr == '&')
			temp[i] = '\0';
		else
			break;
	}
	return temp;
}

void destroyArgs(char **new_argv, int size, char *stripped, char *command_line)
{
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
	if(stripped) {
		free(stripped);
		stripped = NULL;
	}
}

//Reaps all children processes. Used by sigaction() to handle SIGCHLD signal
//
void termination_handler(int signum) 
{
	int status;
	pid_t w;
	while((w = waitpid(-1,&status,WNOHANG)) > 0) {
		if(w == -1) {
			perror("termination handler error");
			exit(EXIT_FAILURE);
		}
	}
}

void printArgs(char ** new_argv, int size)
{
	if(new_argv) {
		printf("Arguments:\n");
		for(int i = 0; i < size; i++) {
			printf("%s\n",new_argv[i]);
		}
	}
	else
		printf("No arguments within argv\n");
}

void insertPrompt()
{
	char *usr = getenv("USER");
	printf("silly-%s # ",usr);
}

void welcome()
{
	clearScreen();
	printf("Welcome to Silly Shell!\n\n\n\n");
}

void clearScreen()
{
	for(int i = 0; i < SCREEN; i++) {
		putchar('\n');
	}
}
