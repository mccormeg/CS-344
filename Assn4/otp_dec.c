/******************************************************************************
** Name: Megan McCormick
** Program Name: otp_dec.c 
** Description: This program is the client for the server otp_dec_d.c
** otp_dec_d will encrypt the message and send it back to otp_dec and 
** otp_dec will send it to stdout.
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
	char buffer[12000];
	char key[12000];
	char msg[12000];
	char enc[12000];
	
	struct sockaddr_in serverAddress;
	struct hostent* server;

	if(argc < 4){
		fprintf(stderr,"USAGE: %s plaintxt key port\n", argv[0]);
		exit(1);
	}
	
	//Set port
	port = atoi(argv[3]);
	
	
	//Opean file
	fp = open (argv[1],O_RDONLY);
	
	//Can't read
	if (fp < 0){
		printf("ERROR IN CLIENT: Cannot open ciphertext file: %s\n",argv[1]);
		exit(1);

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
		printf("ERROR IN CLIENT: Cannot open file %s\n",argv[2]);
		exit(1);

	}
	
	//Read key
	kread = read(fp,key,12000);
	
	//Check file
	goodfile(key,kread);
	
	//Close file
	close(fp);
	
	//Check that key is bigger or same
	if(kread<nread){
		printf("ERROR: Key file isn't big enough\n");
	}
	
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	server = gethostbyname("localhost");
	if (server == NULL) {
		fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
		exit(2); 
	}
	memcpy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, server->h_length);
	
	//Creating socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	
	if (sockfd < 0){
		printf("ERROR IN CLIENT: Cannot create socket\n");
		exit(2);
	}
	
	// Connect socket to address
	if (connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
		printf("ERROR IN CLIENT: Cant connect to server\n");
		exit(2);

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
//	printf("Message from server: %s",msg);
	
	//Send key
	if((write(sockfd,key,nread-1))<kread-1){
		error("Couldn't send key file to server\n");
	}
	
	memset(enc,0,12000);
	

		//Get ciphertext from server
		got =recv(sockfd,enc,nread-1,0);
	
	
	if(got<0){
		error("ERROR: Cannot get cipher text\n");
	}
	if(enc !=NULL){
		//Print decoded text to stdout
		for(i=0; i < nread-1;i++){
			printf("%c",enc[i]);
		}
		//Newline
		printf("\n");
	}
	//Kill socket;
	close(sockfd);
	
}
void goodfile(char* file,int size){
	int i;
	for (i = 0; i< size-1; i++){
		if((int) file[i] > 90 || ((int) file[i] < 65 && (int) file[i] != 32)){
			printf("ERROR: Text includes bad characters\n");
			exit(1);
		}
	}
}

void error(const char *msg) { 
	perror(msg); 
	exit(1); 
}