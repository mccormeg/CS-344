/***********************************************************************
** Name: Megan McCormick
** Program: smallsh
** Description: A simple shell that has set built-in commands and sends
** all other commands back to original shell.
** Date: 11/17/16
***********************************************************************/

/**********************************************************************
** Block of include statements for program
***********************************************************************/
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**********************************************************************
** Global variables used in running the code
**********************************************************************/
int pid;
int ppid;
int status=0;
int background = 0;
int exit_sh = 0;

/**********************************************************************
** Function prototypes
***********************************************************************/
void run();
char *read_in();
char **sep_in(char*input);
int built_in(char**args);
int execute(char**args);
int contains(char**args,char*search);
char* file_name(char**args);
int fork_it(char** args);
void restdin(char**args);
void restdout(char**args);

int main(){
	//Signal Handling
	struct sigaction act;
	act.sa_handler = SIG_IGN; //set to ignore
	sigaction(SIGINT, &act, NULL);
	//Running loop
	run();
	return 0;
}
/************************************************************************
** Name: run
** Description: loop for constant read-in. Parses data by passing to 
** functions and executes based on data or lack there of.
** Parameters: None
** Type: void
************************************************************************/
void run(){
	
	char *input;
	char **args;

	//Loop for constant 
	do{
		//Resets background value to 0
		background = 0;
		//Prints prompting colon
		printf(": ");
		//Flushes stdout
		fflush(stdout);  
		//Read will take in full line from stdin
		input = read_in();
		//Will splits arguments and places them in array
		args = sep_in(input);
		//Execetes arguments
		execute(args);
		//Frees all dynamic memory
		free(input);
	}while(!exit_sh);
	
}

/************************************************************************
** Name: read_in
** Description:Reads in data from command line to be used for parsing and
** execution.
** Parameters: None
** Type: char*
************************************************************************/
char* read_in(){
	int pos = 0;
	int buffsize = 1024;
	int a;
	char* buffer = (char*) malloc( sizeof(char)*buffsize);
	//Checks that things get allocated.
	if(!buffer){
		fprintf(stderr,"Allocation Error\n");
		exit(1);
	}
	while(1){
		//Gets one character at a time.
		a = getchar();
		//Stops when you reach the end of the file or a new line
		if(a==EOF || a=='\n'){
			buffer[pos]='\0';
			return buffer;
		}
		//Places character in array
		else{
			buffer[pos]=a;
		}
		//Incirments counter
		pos++;
		//Resizes array if  not enough space for all characters
		if(pos >=buffsize){
			buffsize+= buffsize;
			buffer = (char*)realloc(buffer,buffsize);
			if(!buffer){
				fprintf(stderr, "Allocation Error\n");
				exit(1);
			}	
		}
	}	
}
/************************************************************************
** Name: sep_in
** Description:Parses input for useablility
** Parameters: input in the form of a char*
** Type: char**
************************************************************************/
char **sep_in(char*input){
	int buffsize = 64, pos = 1;
	char **args = (char**)malloc(buffsize*sizeof(char*));
	//Splits by spaces
	const char *end = " ";
	//Gets first command
	char *com = strtok(input,end);
	//Makes first command first arguments
	args[0] = com;
	com = strtok(NULL,end);
	//Will get all commands 
	while(com !=NULL){
		args[pos] = com;
		pos++;
		//Resizes if there aren't enough spaces
		if(pos>=buffsize){
			buffsize+=buffsize;
			args = (char**)realloc(args,buffsize*sizeof(char*));
			if(!args){
				perror("Can not allocate\n");
				exit(1);
			}
		}
		//Gets next argument
		com = strtok(NULL,end);
	}
	//Makes last argument NULL
	args[pos]=NULL;
	return args;
}
/************************************************************************
** Name: built_in
** Description:executes built in functions(cd,exit,status)
** Parameters: char**
** Type: void
************************************************************************/
int built_in(char**args){
	char* com = args[0];
	//Exits for command exit
	if(strcmp(com,"exit")==0){
		free(args);
		exit_sh =1;
		exit(0);
	}
	//Change directory
	else if(strcmp(com,"cd")==0){
		char *place = args[1];
		if(place ==NULL){
			//If no directory go to home directory
			place = getenv("HOME");
		}
		//Make sure you can change dirtory if so changes directory otherwise error message
		if(chdir(place) == -1){
			perror("chdir isn't a directory.\n");
			free(args);
		}
		free(args);
		return 1;
	}
	//Status command 
	else if( strcmp(com,"status")==0){
		//Prints out exit status
		if(!WIFEXITED(status)){
			printf("Exit: %d\n", WEXITSTATUS(status));
		}
		//Prints signal if there is one
		if(!WIFSIGNALED(status)){
			printf("Signal: %d\n", WSTOPSIG(status));
		}
		//Prints terminating signal
		if(!WTERMSIG(status)){
			printf("Termination signal: %d\n",WTERMSIG(status));
		}
		free(args);
		return 1;
	}
	else{
		return 0;
	}
}
/************************************************************************
** Name: execute
** Description:executes all other functions
** Parameters: char**
** Type: int
************************************************************************/
int execute(char**args){
	//If there are no arguments or a comment do nothing
	if(args[0] == NULL || !strncmp(args[0],"#",1)){
		
	}
	//Runs built in arguments
	else if (built_in(args)==1){
		
	}
	//All other commands
	else{
		//If background change background values
		if(contains(args,"&")==1){
			background =1;
		}
		//Start that forking
		fork_it(args);
		
	}
	return 1;
}
/************************************************************************
** Name: Fork_it
** Description:Makes a child directory
** Parameters: char**
** Type: int
************************************************************************/
int fork_it(char** args){

	struct sigaction act;
		pid =fork();
		if(pid ==0){
			if(!background) {
				//Must handling signals again because of child process
				act.sa_handler = SIG_DFL;
				act.sa_flags = 0;
				sigaction(SIGINT, &act, NULL);
			}
			//If stdin redirections
			if(contains(args,"<")==1){
				//Redirects stdin and then gets rid of all arguments but 
				//first one
				restdin(args);
				args[1]=NULL;
				args[2]=NULL;
			}
			//If background process
			else if(background == 1){
				//Redirect stdin 
				int fd = open("/dev/null",O_RDONLY);
				//Catches if can we duplicated
				if(dup2(fd, 0)==-1){
					perror("File Error: Can't redirect\n");
				}
				//Kills other arguments
				args[2]=NULL;
				//Don't forget to close that file
				close(fd);
			}
			//If redirecting stdout
			else if(contains(args,">")==1){
				//Redirect stdout and kills other arguments but first
				restdout(args);
				args[1]=NULL;
				args[2]=NULL;
			}
			//Executes first argument if it can't prints an error message
			if(execvp(args[0],args)==-1){
				printf("Command Error\n");
				fprintf(stderr,"Command Error\n");
				fflush(stdout);
				exit(1);
			}
		}
		//Prints error if forking fails
		else if(pid<0){
			fprintf(stderr,"Forking error\n");
			fflush(stdout);
			status = 1;
		}
		//Parent process execution
		else{
			//Wait for forground proces to finish if not a background process
			if(!background){
				do{
					ppid = waitpid(pid,&status,0);
				}while(!WIFEXITED(status) && !WIFSIGNALED(status));
			}
			//Otherwise print process id of background proces
			else{
				printf("Background pid: %d\n", pid);
				fflush(stdout);
			}
		}
		//KILL ALL CHILDREN or let them die on their own
		pid = waitpid(-1, &status,WNOHANG);
		while(pid>0){
			printf("Background pid complete: %d\n", pid);
			if(WIFEXITED(status)){
				printf("Exit status: %d\n", WEXITSTATUS(status));
				fflush(stdout);
			}
			else{
				printf("Termination signal: %d\n",status);
			}	
			pid = waitpid(-1,&status,WNOHANG);
		}
		return 1;
}
/************************************************************************
** Name: contains
** Description:Checks if a string is in the array
** Parameters: char**
** Type: int
************************************************************************/
int contains(char**args,char*search){
	int i=0;
	//Looks for charaters by looping through array
	while(args[i]!=NULL){
		if(!strcmp(args[i],search)){
			return 1;
		}
		i++;
	}
	return 0;
}
/************************************************************************
** Name: file_name
** Description:Gets the file name for redirecting input/output
** Parameters: char**
** Type: char*
************************************************************************/
char* file_name(char**args){
	int count = 0;
	//Gets file name by finding < or >
	while(args[count]!=NULL){
		if((!strcmp(args[count],"<"))||(!strcmp(args[count],">"))){
			return args[count+1];
		}
		count++;
	}
}
/************************************************************************
** Name: restdin
** Description:Redirects standard in
** Parameters: char**
** Type: void
************************************************************************/
void restdin(char**args){
	int fd;
	//Gets fie name for redirection
	char *file_in = file_name(args);
	printf("file_in: %s\n",file_in);
	//Redirects to dev/null if no file is given
	if(args[2]==NULL){
		//Open in read only
		fd = open("/dev/null",O_RDONLY);
		//Kills < character
		args[1]=NULL;
	}
	else{
		//Otherwise opens file
		fd = open(file_in,O_RDONLY);
		//If file doesn't exist or can't be opened it outputs an error message
		if(fd==-1){
			printf("File Error: Can't open file %s\n",file_in);
			fprintf(stderr,"File Error: Can't open file\n");
			fflush(stdout);
			exit(1);
		}
	}
	//Duplicates for redirection and prints an error message if it fails
	if(dup2(fd, 0)==-1){
				perror("File Error: Can't redirect\n");
	}
	//Closes file	
	close(fd);
	
}
/************************************************************************
** Name: restdout
** Description:Redirects stdout
** Parameters: char**
** Type: void
************************************************************************/
void restdout(char**args){
	int fd;
	//Gets file name
	char* file_out = file_name(args);
	printf("file_out: %s\n",file_out);
	//Attempts to open file
	fd = open(file_out,O_WRONLY|O_CREAT|O_TRUNC, 0644);
	//If file can't be opened outputs and error message
	if(fd ==-1){
		printf("Can not open file: %s\n",file_out);
		//Always flush
		fflush(stdout);
		perror("No such file\n");
		exit(1);
	}
	//Redirects stdout 
	if(dup2(fd,1)==-1){
		//If redirection fails errormessage and quit
		fprintf(stderr,"Cannot redirect\n");
		printf("Cannot redirect: %s\n",file_out);
		fflush(stdout);
		exit(1);
	}
	//Close file
	close(fd);
	
}