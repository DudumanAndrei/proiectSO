#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
 
#define BUFFSIZE 1024
 
void printWidthAndHeight(int fisierIntrare, int fisierIesire){
	uint32_t width;
	uint32_t height;
	int rd;
	char buffer[BUFFSIZE];
	if((rd = read(fisierIntrare, buffer, BUFFSIZE))!=0){
		width = *(uint32_t *)(buffer + 18);
		sprintf(buffer, "Latimea este: %d\n", width);
		write(fisierIesire, buffer, strlen(buffer));
		height = *(uint32_t *)(buffer + 22);
		sprintf(buffer, "Inaltimea este: %d\n", height);
		write(fisierIesire, buffer, strlen(buffer));
	}
}
 
void printFileInfo(int fisierIntrare,int fisierIesire){
	struct stat fileInfo;
	char buffer[BUFFSIZE];
 
	sprintf(buffer, "Dimensiune: %ld\n", fileInfo.st_size);
	write(fisierIesire, buffer, strlen(buffer));
 
	sprintf(buffer, "Identificatorul utilizatorului: %d\n", fileInfo.st_uid);
	write(fisierIesire, buffer, strlen(buffer));
 
	sprintf(buffer, "Timpul ultimei modificari: %s\n",ctime(&fileInfo.st_mtime)); //why null, in man 2 stat, st_mtime is a struct, and we need another function to convert, intersting 
	write(fisierIesire, buffer, strlen(buffer));
 
	sprintf(buffer, "Numarul de legaturi: %ld\n", fileInfo.st_nlink);// what type is nlink_t?
	write(fisierIesire, buffer, strlen(buffer));
}
 
void printUserPermission(mode_t permisiunea, int fisierIntrare, int fisierIesire){
	char buffer[BUFFSIZE];
 
	sprintf(buffer, "drepturi de acces user: ", NULL);
	write(fisierIesire, buffer, strlen(buffer));
 
	if(permisiunea & S_IRUSR){
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "R",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
 
	if(permisiunea & S_IWUSR){
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "W",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
 
	if(permisiunea & S_IXUSR){
		sprintf(buffer, "-\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "X\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
}
 
void printGroupPermission(mode_t permisiunea, int fisierIntrare, int fisierIesire){
	char buffer[BUFFSIZE];
 
	sprintf(buffer, "drepturi de acces grup: ", NULL);
	write(fisierIesire, buffer, strlen(buffer));
 
	if(permisiunea & S_IRGRP){
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "R",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
 
	if(permisiunea & S_IWGRP){
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "W",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
 
	if(permisiunea & S_IXGRP){
		sprintf(buffer, "-\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "X\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
}
 
void printOtherPermission(mode_t permisiunea, int fisierIntrare, int fisierIesire){
	char buffer[BUFFSIZE];
 
	sprintf(buffer, "drepturi de acces pentru altii: ", NULL);
	write(fisierIesire, buffer, strlen(buffer));
 
	if(permisiunea & S_IROTH){
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "R",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
 
	if(permisiunea & S_IWOTH){
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "W",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
 
	if(permisiunea & S_IXOTH){
		sprintf(buffer, "-\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "X\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
}
 
 
void printAllPermissions(int fisierIntrare, int fisierIesire){
	struct stat filePermission;
 
	printUserPermission((filePermission.st_mode & S_IRWXU), fisierIntrare, fisierIesire);
 
	printGroupPermission((filePermission.st_mode & S_IRWXG), fisierIntrare, fisierIesire);
 
	printOtherPermission(filePermission.st_mode, fisierIntrare, fisierIesire);
}
 
int main(int argc, char *argv[]){
 
    int fisierIntrare;
    int fisierIesire;
    char buffer[BUFFSIZE];
    //struct stat *checkFile;
 
    if(argc != 2){
        perror("Nu sunt sufieciente argumente");
         exit(10);
    }
 
    if((fisierIntrare=open(argv[1], O_RDONLY))<0){
        perror("Nu se poate deschide fisierul de intrare");
         exit(10);
    }
 
    if((fisierIesire=open("statistica.txt", O_WRONLY|O_TRUNC|O_CREAT,S_IRWXU))<0){
        perror("Nu se poate crea fisierul de iesire");
        exit(10);
    }
 
    char aux[100]; // we save a copy of argv[1] in this auxiliar
    strcpy(aux, argv[1]);
    char *trunc;
    trunc=strrchr(aux, '.'); // why strtok doesn't work?
    if(strcmp(trunc,".bmp")!=0){
        perror("Fisierul nostru nu este de tipul indicat!\nIncarcati alt fisier!");
        exit(-1);
    } 
 
    sprintf(buffer, "File name: %s\n", aux);
    write(fisierIesire, buffer, strlen(buffer));
 
    printWidthAndHeight(fisierIntrare, fisierIesire);
 
    printFileInfo(fisierIntrare, fisierIesire);
 
  	printAllPermissions(fisierIntrare, fisierIesire);
 
    close(fisierIntrare);
    close(fisierIesire);
    return 0;
}