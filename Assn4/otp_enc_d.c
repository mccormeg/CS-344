/******************************************************************************
** Name: Megan McCormick
** Program Name: otp_enc_d.c 
** Description: This will run in the background and will encrypt the message 
** using a key and a text given to it through a socket. Once encrypted the
** the new text will be sent back through the same socket.
** Date: 12/2/2016
******************************************************************************/

/******************************************************************************
***************************** Included files **********************************
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUG 0

/******************************************************************************
*************************** Function Prototypes *******************************
******************************************************************************/
void error(const char *msg);
void runprocess(int nsockfp, int pid);
int conint(char a);
int conint(char a);


int main(int argc, char*argv[]){
	//Variable Declaration
	int listsockfd; //Listening socket
	int nsockfd; //Connect to socket
	int port; //Port number given in command line
	int pid;
	int yes;
	
	socklen_t clilen;
	struct sockaddr_in serverAddress, clientAddress;
	
	//Must have 2 arguments to run
	if (argc<2){
		fprintf(stderr,"USAGE: %s port\n", argv[0]);
		exit(1);
	}
	
	if(BUG){
		fflush(stdout);
		printf("Setting up address for server...\n");
	}
	if(BUG){
		fflush(stdout);
		printf("Setting up socket...\n");
	}
	// Set up the socket
	listsockfd = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listsockfd < 0){
		error("ERROR IN SERVER: Cannot open socket\n");
	}
	
	yes = 1;
	setsockopt(listsockfd, SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
		
	if(BUG){
		fflush(stdout);
		printf("Socket setup completed\n");
	}
	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	port = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(port); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process
	if(BUG){
		fflush(stdout);
		printf("Server address completed\n");
	}

	if(BUG){
		fflush(stdout);
		printf("Binding socket...\n");
	}
	
		// Enable the socket to begin listening
	if (bind(listsockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){// Connect socket to port
		error("ERROR IN ENCRYPTION SERVER: Cannot bind\n");
	}
	if(BUG){
		fflush(stdout);
		printf("Socket has been bound\n");
	}
	
	if(BUG){
		fflush(stdout);
		printf("Listening for connections...\n");
	}
	if(listen(listsockfd, 5)==-1){// Flip the socket on - it can now receive up to 5 connections
		error("Cannot listen to port\n");
	}
	
	clilen = sizeof(clientAddress);
	
	while(1){
		//Set clilen to the size of the address
		
		
		//Accept connections
		if(BUG){
			fflush(stdout);
			printf("Connection time...\n");
		}
		//Acepting new socket
		nsockfd = accept(listsockfd, (struct sockaddr *)&clientAddress, &clilen); 
		if (nsockfd < 0){
			printf("ERROR IN SERVER: Cannot make connection\n");
			exit(1);
		}
	
		//Forking time
		if(BUG){
			fflush(stdout);
			printf("Forking...\n");
		}
		pid = fork();
		if(pid < 0){
			error("Cannot fork\n");
		}
	
		//Child Process happening
		if(pid==0){
			if(BUG){
				fflush(stdout);
				printf("In child process\n");
			}
		
			runprocess(nsockfd,pid);
			close(listsockfd);
			close(nsockfd);
			exit(0);
		
		}
		else{
			close(listsockfd);
			close(nsockfd);
		}
	}
	
}

void error(const char *msg) { 
	perror(msg); 
	exit(1); 
} 

int conint(char a){
	if(a == ' '){
		return 26;
	}
	else{
		return (a-'A');
	}
}

char conchar(int a){
	if(a == 26){
		return ' ';
	}
	else{
		return (a+'A');
	}
}

void runprocess(int nsockfd, int pid){
	char buffer[12000];
	char key[12000];
	int nread; 
	int kread;
	int i = 0;
	int nchar;
	int sent;
	
	if(BUG){
			fflush(stdout);
			printf("In runprocess\n");
	}
	
	//Clean buffer
	memset(buffer, 0 , 12000);
	
	//Recieve Message from client
	nread = recv(nsockfd, buffer, 12000, 0);
	//printf("Buffer: %s\n",buffer);
	if (nread < 0){
		printf("SERVER: I received this from the client: \"%s\"\n", buffer);
		error("ERROR reading from socket");
	}
	//Got message sending one back
	sent = write(nsockfd, "I am the server, and I got your message", 39);
	if (nread < 0){
		error("ERROR: Cannot write to socket\n");
	}
	printf("nread: %d\n",nread);
	//Key time
	memset(key, 0 , 12000);
	
	//Recieve key
	kread = read(nsockfd, key, 12000);
	//printf("Key: %s\n",key);
	if(kread < 0){
		error("ERROR: Cannot read key");
	}
	printf("kread: %d\n",kread);
	
	//Check Cipher
	for (i = 0; i < nread-1;i++){
		if((((int) buffer[i] > 90)&&(int) buffer[i] != 32) || (((int) buffer[i] < 65) && (int) buffer[i] != 32)){
			//printf("Bad char is: %c ascii number: %d\n",buffer[i],(int)buffer[i]);
			error("ERROR: Bad Characters in cipher text\n");
		}
	}
	
	//Check key
	for (i = 0; i < kread-1;i++){
		if(((int) key[i] > 90&& (int) key[i] != 32) || ((int) key[i] < 65 && (int) key[i] != 32)){
			printf("Bad char is: %c ascii number: %d\n",key[i],(int)key[i]);
			error("ERROR: Bad Characters in key\n");
		}
	}
	
	//Check that key is as big or bigger than cipher
	if(kread < nread){
		error("ERROR: Key isn't big enough\n");
		
	}
	
	//Encrypting process
	for(i=0;i<nread;i++){
		nchar = (conint(buffer[i])+conint(key[i]))%27;
		//printf("nchar: %d\n",nchar);
		buffer[i] = conchar(nchar);
		//printf("buffer at %d: %c\n",i,buffer[i]);
	}
	buffer[i] = '\n';
	//printf("Buffer in server: %s", buffer);
	
	//Send it back
	sent = send(nsockfd,buffer,12000,0);
	if (sent<0){
		error("ERROR: Cannot send message\n");
	}
	
}