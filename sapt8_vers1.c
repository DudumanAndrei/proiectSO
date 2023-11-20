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
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>

#define BUFFSIZE 1024

void printWidthAndHeight(const char *path, int fisierIesire){
	uint32_t width;
	uint32_t height;
	int rd;
	int fisierIntrare;
	char buffer[BUFFSIZE];
	
	if((fisierIntrare = open(path, O_RDONLY))<0){
		perror("Nu putem deschide fisierul bmp");
		exit(-1);
	}
	
	if((rd = read(fisierIntrare, buffer, BUFFSIZE))!=0){
		width = *(uint32_t *)(buffer + 18);
		sprintf(buffer, "\nLatimea este: %d\n", width);
		write(fisierIesire, buffer, strlen(buffer));
		height = *(uint32_t *)(buffer + 22);
		sprintf(buffer, "Inaltimea este: %d", height);
		write(fisierIesire, buffer, strlen(buffer));
	}
}

void printFileInfo(struct stat fileInfo, int fisierIesire){
	char buffer[BUFFSIZE];
	
	sprintf(buffer, "\nDimensiune: %ld\n", fileInfo.st_size);
	write(fisierIesire, buffer, strlen(buffer));
	
	sprintf(buffer, "Identificatorul utilizatorului: %d\n", fileInfo.st_uid);
	write(fisierIesire, buffer, strlen(buffer));
	
	sprintf(buffer, "Timpul ultimei modificari: %s",ctime(&fileInfo.st_mtime)); //why null, in man 2 stat, st_mtime is a struct, and we need another function to convert, intersting 
	write(fisierIesire, buffer, strlen(buffer));
	
	sprintf(buffer, "Numarul de legaturi: %ld\n", fileInfo.st_nlink);// what type is nlink_t?
	write(fisierIesire, buffer, strlen(buffer));
}

void printUserPermission(mode_t permisiunea, int fisierIesire){
	char buffer[BUFFSIZE];
	
	sprintf(buffer, "drepturi de acces user: ", NULL);
	write(fisierIesire, buffer, strlen(buffer));
	
	if(permisiunea & S_IRUSR){
		sprintf(buffer, "R",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
	
	if(permisiunea & S_IWUSR){
		sprintf(buffer, "W",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
	
	if(permisiunea & S_IXUSR){
		sprintf(buffer, "X\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
}

void printGroupPermission(mode_t permisiunea, int fisierIesire){
	char buffer[BUFFSIZE];
	
	sprintf(buffer, "drepturi de acces grup: ", NULL);
	write(fisierIesire, buffer, strlen(buffer));
	
	if(permisiunea & S_IRGRP){
		sprintf(buffer, "R",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
	
	if(permisiunea & S_IWGRP){
		sprintf(buffer, "W",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
	
	if(permisiunea & S_IXGRP){
		sprintf(buffer, "X\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
}

void printOtherPermission(mode_t permisiunea, int fisierIesire){
	char buffer[BUFFSIZE];
	
	sprintf(buffer, "drepturi de acces pentru altii: ", NULL);
	write(fisierIesire, buffer, strlen(buffer));
	
	if(permisiunea & S_IROTH){
		sprintf(buffer, "R",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
	
	if(permisiunea & S_IWOTH){
		sprintf(buffer, "W",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
	
	if(permisiunea & S_IXOTH){
		sprintf(buffer, "X\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}else{
		sprintf(buffer, "-\n",NULL);
		write(fisierIesire,buffer,strlen(buffer));
	}
}


void printAllPermissions(int fisierIesire,struct stat filePermission){
	
	printUserPermission(filePermission.st_mode, fisierIesire);
	
	printGroupPermission(filePermission.st_mode, fisierIesire);
	
	printOtherPermission(filePermission.st_mode, fisierIesire);
}

void printDirectorInfo(const char *director_principal, int fisierIesire){
	struct stat checkFile;
	char buffer[BUFFSIZE];
	DIR *director = opendir(director_principal);
	if(director == NULL){
		perror("Nu s-a transmis corect calea catre director!\n");
		exit(-1);
	}
	
	sprintf(buffer, "\nNumele directorului: %s\n", director_principal);
	write(fisierIesire, buffer, strlen(buffer));
	
	stat(director_principal, &checkFile);
	sprintf(buffer, "Identificatorul utilizatorului: %d\n", checkFile.st_uid);
	write(fisierIesire, buffer, strlen(buffer));
	
	printAllPermissions(fisierIesire, checkFile);
}

void printSymbolicLink(struct stat link, int fisierIesire){
	char buffer[BUFFSIZE];
	
	sprintf(buffer, "\nDimensiune legatura: %ld\n", link.st_size);
	write(fisierIesire, buffer, strlen(buffer));
	
	printAllPermissions(fisierIesire,link);
}

void travelDirector(const char *director_intrare, const char *director_iesire){
	/*	
	struct stat checkFile;
	struct dirent *index;
	DIR *director = opendir(director_principal);
	
	while((index = readdir(director))!=0){
		char path[BUFFSIZE];
		char buffer[BUFFSIZE];
		sprintf(path,"%s/%s",director_principal, index->d_name);
		
		if(lstat(path, &checkFile) !=0){
			perror("Path-ul nu e bun\n");
			exit(-1);
		}
		
		if(S_ISREG(checkFile.st_mode)){
			char aux[100]; 
    		strcpy(aux, index->d_name);
    		char *trunc;
    		trunc=strrchr(aux, '.');
    		sprintf(buffer, "\nnumele fisierului: %s", index->d_name);
    		write(fisierIesire,buffer, strlen(buffer));
    		if(strcmp(trunc,".bmp") == 0){ // este fisier bmp
    			printWidthAndHeight(path, fisierIesire);
    		}
    		printFileInfo(checkFile, fisierIesire);
    		printAllPermissions(fisierIesire, checkFile);
		}
		
		if(S_ISDIR(checkFile.st_mode)){
			printDirectorInfo(path, fisierIesire);
		}
		
		if(S_ISLNK(checkFile.st_mode)){
			printSymbolicLink(checkFile, fisierIesire);
			//cum afisez dimensiunea fisierului pointat de symbolic link?
		}
	}
	*/
}

int main(int argc, char *argv[]){

    int fisierIntrare;
    int fisierIesire;
    char buffer[BUFFSIZE];
    struct stat checkFile;

    if(argc != 3){
        perror("Nu sunt sufieciente argumente");
         exit(10);
    }
    
	const char *director_intrare = argv[1];
    const char *director_iesire = argv[2];
	
    if(stat(director_intrare, &checkFile) != 0){
    	perror("Nu luam bine statutul fisierului!\n");
    	exit(-1);
    } 
    
    if(!S_ISDIR(checkFile.st_mode)){
        perror("Nu trimitem director intrare!\nIncarcati altceva!");
        exit(-1);
    } 
  	
  	if(stat(director_iesire, &checkFile) != 0){
    	perror("Nu luam bine statutul fisierului!\n");
    	exit(-1);
    } 
    
    if(!S_ISDIR(checkFile.st_mode)){
        perror("Nu trimitem director iesire!\nIncarcati altceva!");
        exit(-1);
    } 
    
    pid_t pId = fork();
    pid_t asteptam;
    
    if(pId < 0){
    	perror("Eroare la functia fork()\n");
    	exit(66);
    }
    else{
    	if(pId == 0){
    		travelDirector(director_intrare, director_iesire);
    		exit(0);
    	}
    	else{
    		asteptam = wait(NULL);
    		
    	}
    }
    
    
    
    
  	//travelDirector(director_principal, fisierIesire);
  
    //close(fisierIesire);
    return 0;
}