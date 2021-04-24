/**************************************************************************
** Name: Megan McCormick
** Date: 10/27/2016
** Program Name: mccormeg.adventure.cab
** Description: Text based adventure game run by file IO. Program starts by 
** creating rooms with randoms connections and exporting those rooms 
** to text files within a generated directory. THe program then reads in the 
** data from those files to be used in the game. I tried to impliment
** but the code no longer ran correctly so I took it out. SO\rry
**************************************************************************/

/***********************************************************
** Include statements
*************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <dirent.h>
#include <assert.h>
#include <pthread.h> 

/*******************************************************
** Declaration of struct to hold room values for writing
** and reading to/from file aswell as easy access
*********************************************************/
struct room{
	char name[330];
	int num_con;
	char *connections[6];
	char type[330];
	
};
/*************************************************************
** Declaration of constant static global variables that hold 
** the list of names and the type of room. Also declairs a static
** container for rooms used in the game and an index for r1
**************************************************************/
const char* SME[3]={
	"START_ROOM",
	"MID_ROOM",
	"END_ROOM"
};

const char* rooms[10]={
	"London",
	"Hogwarts",
	"Diagon_Alley",
	"Hogsmeed",
	"Ministry_of_Magic",
	"Forbidden_Forest",
	"Quiddich_Pitch",
	"Knockturn_Alley",
	"Black_Lake",
	"Gringotts"
};

struct room used_rooms[7];

int r_index[7];

/*******************************************************
** Prototyes for all functions
******************************************************/

void create_directory();
void create_files(char *dir_Name);
int repeat_r(int r_index[7],int r);
void print_room(int room);
void create_rooms();
void connect(int r1, int r2);
int connected(int r1,int r2);
struct room* create_map();
char *directory();
void kill_map(struct room *map);
void play(struct room *map);
int find_index(char *name);


int main(){
	srand(time(NULL)); //Seeding random function for use throughout the code.
	create_rooms(); // Creates each room and placeseach room in the used room container
	create_directory(); // Uses the used room container and creates a directory and places txt files for roms in that directory.
	struct room *map=create_map();//Reads data from the files into a dynamic array called map
	play(map);//Runs through the game
	kill_map(map);//Frees the memory allocated for the map (Helps with multiple runs);
	
	return 0;
}
/**************************************************************************
** Function Name: find_index
** Description: Finds the index of the room for printing reasons. Compares
** strings to know where in the array the room is
** Parameters: char*
** Type: int
***************************************************************************/
int find_index(char *name){
	int i;
	for(i=0;i<7;i++){
		if(!(strcmp(name,used_rooms[i].name))){
			return i;
		}
	}
}
/**************************************************************************
** Function Name: correct_path
** Description: Checks that the path (connection) exists
** Parameters: struct room, char*
** Type: int
***************************************************************************/
int correct_path(struct room r, char *name){
	int i;
	for (i=0;i<r.num_con;i++){
		if(!(strcmp(r.connections[i],name))){
			return 0;
		}
	}
	printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
	return 1;
}
/**************************************************************************
** Function Name: play
** Description: Plays the full game
** Parameters: struct room *
** Type: void
***************************************************************************/
void play(struct room *map){
	//Initialization of variables. Pointer to current gets the address of the starting room
	struct room *current = &used_rooms[0];
	int *visited = malloc(sizeof(int)*7);
	int index = 0;
	char input[123];
	int cap=7;
	int visit=0;
	int i;
	int j;
	int k;
	int steps=0;
	//Do while will run untill the end room is found
	do{
		//Formatting for printing: Prints current room and all connections with commas seperating and a period at the end
		printf("\nCURRENT LOCATION: %s\n", current->name);
		printf("POSSIBLE CONNECTIONS: ");
        for(i=0;i<current->num_con-1;i++){
			printf("%s, ", current->connections[i]);
		}
		if (current->num_con > 0) {
                printf("%s.\n", current->connections[current->num_con-1]);
        }
		//Will resize array for places visited incase there is no more space for new value
		if(visit>=cap){
			cap = cap +7;
			visited = realloc(visited, cap*(sizeof(int)));
		}
		//Will prompt untill user enters correct path name
		do{
			printf("WHERE TO? >");
			//Will read from command line
			char *ret = fgets(input, 123, stdin);
			input[strlen(input)-1]='\0';
		}while(correct_path(*current,input));
		//Loops through to find all connections
		for(j =0;j<current->num_con;j++){
			if(strcmp(input,current->connections[j])==0){
				//Updates current position, visited array, visited index and steps once
				//match is found
				current = &used_rooms[find_index(input)];
				visited[visit]=find_index(input);
			
				visit++;
				steps++;
				//breaks because match was found
				break;
			}	
		}
	//Prints out wining message, stats, and path to user
	}while(strcmp(current->type,"END_ROOM"));
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
    for (k = 0; k < visit; k++) {
         printf("%s\n", used_rooms[visited[k]].name);
    }
}
/**************************************************************************
** Function Name: directory
** Description: Creates the name for directory
** Parameters: None
** Type: char*
***************************************************************************/
char *directory(){
	int pid = getpid(); //Gets process ID to make directory
	char prefix[] = "mccormeg.adventure."; //Defines the prefix for the directory
	char *dir_Name= malloc(100*sizeof(char)); //Container for full directory name
	
	sprintf(dir_Name, "%s%d", prefix, pid); //Places prefix and PID in directory name container
	return dir_Name;
}

/**************************************************************************
** Function Name: create_directory
** Description: Creates the game directory
** Parameters: None
** Type: void
***************************************************************************/
void create_directory(){
	char *dir_Name= directory();
	
	struct stat st = {0}; //Used to check if the directory all ready exists
	//Checks if the directory exists, if not it creates a directory
	if (stat(dir_Name, &st) == -1) {
		mkdir(dir_Name,0777);
		create_files(dir_Name);
	}
	//Else it prints out an error message and exits
	else{
		printf("Directory already exists\n");
		exit(1);
	}
	free(dir_Name);
}

/**************************************************************************
** Function Name: create_files
** Description: Randomly creates 7 files
** Parameters: char*
** Type: void
***************************************************************************/
void create_files(char *dir_Name){
	//Declare file pointer to write files
	FILE *fp;
	int i=0;
	int j=0;
	char file_path[1000];
	int r;
	for (i =0; i<7;i++){
		//Creates the file path to create files
		sprintf(file_path,"%s/%s.txt",dir_Name,used_rooms[i].name);
		//Opens file with w+ option
		fp=fopen(file_path,"w+");
		//Prints data into file
		fprintf(fp,"ROOM NAME: %s\n",used_rooms[i].name);
		for(j=0;j<used_rooms[i].num_con;j++){
			fprintf(fp,"CONNECTION %d: %s\n",j+1,used_rooms[i].connections[j]);
		}
		fprintf(fp,"ROOM TYPE: %s\n",used_rooms[i].type);
		//Closes file, if left open file gets distroyed
		fclose(fp);
	}	
		
	
}

/**************************************************************************
** Function Name: create_rooms
** Description: Randomly creates 7 rooms
** Parameters: None
** Type: void
***************************************************************************/
void create_rooms(){
	int i;
	int r;
	int j;
	int room_num;
	int random;
	//Will run 7 ties to create 7 files for game play
	for (i =0; i<7;i++){
		char room[1000];
		//will randomly choose an index for room names untill one it chosen that hasn't
		//been used before
		do{
			r = rand()%10;
		}while(repeat_r(r_index,r));
		r_index[i]=r;
		//Uses index t get name and copy it into static used rooms container for room 0-7
		strcpy(used_rooms[i].name,rooms[r]);
		//Initializes how many connection the room has
		used_rooms[i].num_con = 0;
		//Copies file tye into used room container
		if(i==0){
			strcpy(used_rooms[i].type,SME[0]);
		}
		else if(i==6){
			strcpy(used_rooms[i].type,SME[2]);
		}
		else
			strcpy(used_rooms[i].type,SME[1]);
		
	}
	//Makes connectios for each room, can make up to 6 if all connection can be made
	for(i=0;i<7;i++){
		random = (rand()%4)+3;
			for(j=0;j<random;j++){
				room_num = rand() % 7;			
				connect(i,room_num);	
			}			
	}
}

/**************************************************************************
** Function Name: repeat_r
** Description: Checks if the random number has already been used
** Parameters: int*, int
** Type: int
***************************************************************************/
int repeat_r(int r_index[7],int r){
	int i;
	for(i=0;i<7;i++){
		if(r_index[i]==r)
			return 1;
	}
	return 0;	
}

/**************************************************************************
** Function Name: connect
** Description: Will connect files unless they are already conneced, the same file or
** the max connection have already been made
** Parameters: int,int
** Type: void
***************************************************************************/
void connect(int r1, int r2){
	// uses a pointer to access each room struct's contents
	struct room *room1 = &used_rooms[r1];
	struct room *room2 = &used_rooms[r2];
	//Checks if rooms can be connected
	if(!(room1->num_con==6||room2->num_con==6||connected(r1,r2))){
		//Adds room to conntions for r1 and r2 and then incriments their
		//connection counter
		room1->connections[room1->num_con]=room2->name;
		room2->connections[room2->num_con]=room1->name;
		room1->num_con++;
		room2->num_con++;
	}



	
}

/**************************************************************************
** Function Name: connected
** Description: Checks if the files are already connected or is itself
** Parameters: int,int
** Type: int
***************************************************************************/
int connected(int r1,int r2){
	int i;
	if(r1==r2){
		return 1;
	}
	for (i=0;i<used_rooms[r1].num_con;i++){
		if(!strcmp(used_rooms[r1].connections[i],used_rooms[r2].name))
			return 1;
	}
	return 0;
}

/**************************************************************************
** Function Name: print_room
** Description: Prints the struct values. Mainly used for error checking
** Parameters: int
** Type: void
***************************************************************************/
void print_room(int room) {
	int i;
        printf("Name: %s\nNum_con %d\nType: %s\n",used_rooms[room].name,used_rooms[room].num_con,used_rooms[room].type);
        for (i = 0; i < used_rooms[room].num_con; i++) {
                printf("connection: %s\n", used_rooms[room].connections[i]);
        }
}

/**************************************************************************
** Function Name: map
** Description: Reads in data from files and recreates the map
** Parameters: None
** Type: struct room*
***************************************************************************/
struct room* create_map(){
	struct room *map = malloc(7*sizeof(struct room));
	int i;
	int j;
	int k;
	int r_count=0;
	struct stat st = {0};
	//Gets directory name
	char *dir_Name = directory();
	char filepath[123];
	char r_name[330];
	char t_name[330];
	//Move to directory of files to be read
	chdir(dir_Name);
	//Allocaes space for connections for each room
	for(j=0;j<7;j++){
		for(k=0;k<6;k++){
			map[j].connections[k]=malloc(sizeof(char));
		}
	}
	//For loop will fill in struct data to array for all 7 rooms
	for(i = 0;i<7;i++){
		//Creates file path for opening files for reading
		sprintf(filepath,"%s.txt",used_rooms[i].name);
		//File is opened in read only mode
		FILE *fp = fopen(filepath,"r");
		//Will read in only the string in the %s place to t_name
		fscanf(fp,"ROOM NAME: %s\n",t_name);
		//Copies that string into the struct array
		strcpy(map[i].name,r_name);
		int read;
		int c;
		//Will read untill "CONNECTION %d: %s\n" can't be found or end of file
		while ((read = fscanf(fp,"CONNECTION %d: %s\n",&c,r_name))!=0 &&read != EOF){
			//Copies connection data into struct array
			strcpy(map[i].connections[c-1] , r_name);
		}
		//Copies num_con data into struct array
		map[i].num_con = c-1;
		//Scans for Type
		fscanf(fp,"ROOM TYPE: %s\n",t_name);
		//Copies type data into struct array
		strcpy(map[i].type, t_name);
		//Closes file to keep it from getten broke
		fclose(fp);
	}
	//Returns adujusted map for use in game
	return map;
}

/**************************************************************************
** Function Name: read_room
** Description: Reads in data from files into a room container to give back to 
** create map.
** Parameters: char*
** Type: struct room
***************************************************************************/
void kill_map(struct room *map){
	int i;
	int j;
	//Deallocates all memory for connections in each room
	for(i = 0;i<7;i++){
		for(j=0;j<6;j++){
			free(map[i].connections[j]);
		}
	}
	//Frees map pointer
	free(map);
}
