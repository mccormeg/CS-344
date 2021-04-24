/******************************************************************************
** Name: Megan McCormick
** Program Name: keygen.c 
** Description: This program randomly generates a key for the enconding and 
** deciphering process.
** Date: 12/2/2016
******************************************************************************/

/******************************************************************************
***************************** Included files **********************************
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void checkarg(int argc,char*argv[]);
void error(const char *msg);

int main(int argc, char *argv[]){
	checkarg(argc,argv);
	long length = strtol(argv[1], NULL, 10);
	int i;
	char choice[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	//Seeding rand
	srand(time(NULL));
	
	for(i = 0;i<length;i++){
		printf("%c",choice[rand()%27]);
	}
	printf("\n");
	
}

void checkarg(int argc,char*argv[]){
	if (argc != 2 ){
		error("USAGE: keygen keylength\n");
	}
}
void error(const char *msg) { 
	perror(msg); 
	exit(1); 
} 