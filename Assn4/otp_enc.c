/******************************************************************************
** Name: Megan McCormick
** Program Name: otp_enc.c 
** Description: This program is the client for the server otp_enc_d.c
** otp_enc_d will encrypt the message and send it back to otp_enc and 
** otp_enc will send it to stdout.
** Date: 12/2/2016
******************************************************************************/

/******************************************************************************
***************************** Included files **********************************
******************************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUG 0

/******************************************************************************
*************************** Function Prototypes *******************************
******************************************************************************/
void error(const char *msg);
void goodfile(char* file,int size);

int main(int argc, char *argv[]){
	int i;
	int sockfd;
	int port;
	int nwrite;
	int nread;
	int kread;
	int fp;
	int got;
	int yes;
	char buffer[12000];
	char key[12000];
	char msg[12000];
	char enc[12000];
	
	struct sockaddr_in serverAddress;
	struct hostent* server;
	//Check args
	if(argc < 4){
		fprintf(stderr,"USAGE: %s plaintxt key port\n", argv[0]);
		exit(0);
	}
	
	//Set port
	port = atoi(argv[3]);
	
		//Creating socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	
	if (sockfd < 0){
		error("CLIENT: ERROR opening socket");
	}
	
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	server = gethostbyname("localhost");
	if (server == NULL) {
		error("CLIENT: ERROR, no such host\n");  
	}
	memcpy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, server->h_length);
	
	yes = 1;
	setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
	// Connect socket to address
	if (connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
		error("CLIENT: ERROR connecting");
	}
	
	
	//Opean file
	fp = open (argv[1],O_RDONLY);
	
	//Can't read
	if (fp < 0){
		error("ERROR IN CLIENT: Cannot read file for to encrypt\n");
	}
	
	//Read in text
	nread = read(fp,buffer,12000);
	
	//Check file
	goodfile(buffer,nread);
	
	//Close file
	close(fp);
	
	//Open key
	fp =  open(argv[2], O_RDONLY);
	
	//Can't read
	if (fp < 0){
		error("ERROR IN CLIENT: Cannot read file for key\n");
	}
	
	//Read key
	kread = read(fp,key,12000);
	
	//Check file
	goodfile(key,kread);
	
	//Close file
	close(fp);
	
	if(kread<nread){
		fprintf(stderr,"ERROR: Key file isn't big enough\n");
		exit(1);
	}
	//Send file to otp_enc_d
	if((write(sockfd,buffer,nread-1)<nread-1)){
		error("Couldn't send text file to server\n");
	}
	
	//Make sure server got file
	memset(msg,0,12000);
	if((recv(sockfd,msg,12000-1,0))<0){
		error("Couldn't get message\n");
	}
	//printf("Message from server: %s",msg);
	//Send key
	if((write(sockfd,key,kread-1))<kread-1){
		error("Couldn't send key file to server\n");
	}
	
	memset(enc,0,12000);
	
		//Get ciphertext from server
		got =recv(sockfd,enc,nread-1,0);
	
	//printf("enc: %s\n",enc);
	if(got<0){
		error("ERROR: Cannot get cipher text\n");
	}
	
	if(enc!=NULL){
		//Print ciphertext to stdout
		for(i=0; i < nread-1;i++){
			printf("%c",enc[i]);
		}
		//Newline for cipher
		printf("\n");
	}
	//Kill socket;
	close(sockfd);
	
	return 0;
}
void goodfile(char* file,int size){
	int i;
	for (i = 0; i< size-1; i++){
		if((int) file[i] > 90 || ((int) file[i] < 65 && (int) file[i] != 32)){
			error("ERROR IN CLIENT: Bad Characters\n");

		}
	}
}

void error(const char *msg) { 
	perror(msg);
	exit(1); 
} 