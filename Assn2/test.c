/**************************************************************************
** Name: Megan McCormick
** Date: 10/27/2016
** Program Name: mccormeg.adventure.cab
** Description: Text based adventure game run by file IO
**************************************************************************/
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


struct room{
	char name[330];
	int num_con;
	char *connections[6];
	char type[330];
	
};
const char* SME[3]={
	"START_ROOM",
	"MID_ROOM",
	"END_ROOM"
};

const char* rooms[10]={
	"London",
	"Hogwarts",
	"Diagon Ally",
	"Hogsmeed",
	"Ministry of Magic",
	"Forbidden Forest",
	"Quiddich Pitch",
	"Knockturn Ally",
	"Black Lake",
	"Gringotts"
};

struct room used_rooms[7];

int r_index[7];

void create_directory();
void create_files(char *dir_Name);
int repeat_r(int r_index[7],int r);
void print_room(int room);
void create_rooms();
void connect(int r1, int r2);
int connected(int r1,int r2);
struct room* create_map();
char *directory();
struct room read_room(char* f_name);

const char *pick_right_name(char *in) {
	int i;
        for ( i = 0; i < 6; i++) {
                if (strcmp(in, used_rooms[i].name) == 0) {
                        return used_rooms[i].name;
                }
        }
        return NULL;
}

// Return the room with the provided name. Again, keeps us from mallocing
// things we don't need.
char *pick_right_room(char *in) {
	int i;
        for (i = 0; i < 7; i++) {
                if (strcmp(in, used_rooms[i].name) == 0) {
                        return used_rooms[i].name;
                }
        }
        return NULL;
}

int main(){
	srand(time(NULL));
	create_rooms();
	create_directory();
	struct room *map=create_map();
	int i =0;
	//for (i = 0;i<7;i++){
		//printf("Name: %s\nNum_con %d\nType: %s\n",map[i].name,map[i].num_con,map[i].type);
       // for (i = 0; i < map[i].num_con; i++) {
             //   printf("connection: %s\n", map[i].connections[i]);
      //  }
	//}
	
	return 0;
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
	//Else it prints out an error message
	else{
		printf("Directory already exists\n");
		exit(1);
	}
	//chdir("..");
	free(dir_Name);
}

/**************************************************************************
** Function Name: create_files
** Description: Randomly creates 7 files
** Parameters: char*
** Type: void
***************************************************************************/
void create_files(char *dir_Name){
	FILE *fp;
	int i=0;
	int j=0;
	char file_path[1000];
	int r;
	for (i =0; i<7;i++){
		sprintf(file_path,"%s/%s.txt",dir_Name,used_rooms[i].name);
		fp=fopen(file_path,"w+");
		fprintf(fp,"ROOM NAME: %s\n",used_rooms[i].name);
		for(j=0;j<used_rooms[i].num_con;j++){
			fprintf(fp,"CONNECTION %d: %s\n",i+1,used_rooms[i].connections[j]);
		}
		fprintf(fp,"ROOM TYPE: %s\n",used_rooms[i].type);
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
	
	for (i =0; i<7;i++){
		char room[1000];
		do{
			r = rand()%10;
		}while(repeat_r(r_index,r));
		r_index[i]=r;
		strcpy(used_rooms[i].name,rooms[r]);
		used_rooms[i].num_con = 0;
		if(i==0){
			strcpy(used_rooms[i].type,SME[0]);
		}
		else if(i==6){
			strcpy(used_rooms[i].type,SME[2]);
		}
		else
			strcpy(used_rooms[i].type,SME[1]);
		
	}
	//print_room(1);
	for(i=0;i<7;i++){
		
			for(j=0;j<6;j++){
				room_num = rand() % 7;			
				connect(i,room_num);	
			}			
	}
	strcpy(used_rooms[0].type,"START_ROOM");
	strcpy(used_rooms[6].type,"END_ROOM");
	for(i=0;i<7;i++){
		
		for(j=0;j<6;j++){
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
	
	struct room *room1 = &used_rooms[r1];
	struct room *room2 = &used_rooms[r2];
	
	if(!(room1->num_con==6||room2->num_con==6||connected(r1,r2))){
		room1->connections[room1->num_con]=room2->name;
		room2->connections[room2->num_con]=room1->name;
		room1->num_con++;
		room2->num_con++;
	}



	
}

/**************************************************************************
** Function Name: connected
** Description: Checks if the files are already connected
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
** Description: Prints the struct values for a room for easy error checking
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
        struct room *rooms = malloc(7 * sizeof(struct room));
        unsigned int room_count = 0;
        char *dir_name = directory();
        // FIXME: Make sure that directory exists with stat
        chdir(dir_name);
        DIR *dp;
        struct dirent *dir;
        dp = opendir (".");
        assert(dp != NULL);

        while ((dir = readdir (dp))) {
                rooms[room_count] = read_room(dir->d_name);
        }

        closedir (dp);

        free(dir_name);
        chdir("..");
        return rooms;
}

/**************************************************************************
** Function Name: read_room
** Description: Reads in data from files into a room container to give back to 
** create map.
** Parameters: char*
** Type: struct room
***************************************************************************/
struct room read_room(char *name){
        struct room r;
        FILE *file = fopen(name, "r");

        // I control the names so I know how long to make this buffer
        // should be as long as the longest name or START_ROOM + 1 more
        // character for \0.
        char received_name[50];
        fscanf(file, "ROOM NAME: %s\n", name);
        strcpy(r.name,pick_right_name(name));

        int read;
        int conn_number;
        while ((read =
                fscanf(file, "CONNECTION %d: %s\b", &conn_number, received_name)) != 0
                && read != EOF) {
                strcpy(r.connections[conn_number-1], pick_right_room(received_name));
        }
        r.num_con = conn_number - 1;
        fscanf(file, "ROOM TYPE: %s\n", received_name);
      
        fclose(file);
        return r;
}