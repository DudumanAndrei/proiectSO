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

#define BUFFSIZE 4096

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

void printDirectorInfo(const char *director_principal, const char* path_iesire){
	struct stat checkFile;
	char buffer[BUFFSIZE];
	//DIR *director = opendir(director_principal);
	int fisierIesire;
	char numeFisierStatistica[120];
	char bufferIesire[120];
	
	if(director_principal == NULL){
		perror("Nu s-a transmis corect calea catre director!\n");
		exit(-1);
	}
	
	sprintf(numeFisierStatistica,"%s_%s",director_principal, "statistica.txt");
	sprintf(bufferIesire, "%s/%s",path_iesire, numeFisierStatistica);
		
	if((fisierIesire=open(bufferIesire, O_WRONLY|O_TRUNC|O_CREAT,S_IRWXU))<0){
        perror("Nu se poate crea fisierul de iesire");
        exit(10);
    }
	
	sprintf(buffer, "\nNumele directorului: %s\n", director_principal);
	write(fisierIesire, buffer, strlen(buffer));
	
	lstat(director_principal, &checkFile);
	sprintf(buffer, "Identificatorul utilizatorului: %d\n", checkFile.st_uid);
	write(fisierIesire, buffer, strlen(buffer));
	
	printAllPermissions(fisierIesire, checkFile);
	
	if((close(fisierIesire))!=0){
		perror("eroare la inchidere fisier iesire\n");
		exit(222);
	}
}

void printSymbolicLink(struct stat link,int fisierIesire){
	char buffer[BUFFSIZE];
	
	sprintf(buffer, "\nDimensiune legatura: %ld\n", link.st_size);
	write(fisierIesire, buffer, strlen(buffer));
	
	printAllPermissions(fisierIesire,link);
}

void colorImage(int fisierIntrare) {
    printf("Vreau sa colorez!\n");
    int width;
    int height;
    int rd;
    char buffer[BUFFSIZE];
    
     lseek(fisierIntrare,18, SEEK_SET);
    if(read(fisierIntrare, &width, 4)==-1){
        perror("Nu s-a realizat corect citirea inaltimii.\n");
        exit(-1);
    }
	lseek(fisierIntrare,0,SEEK_SET);
	
    lseek(fisierIntrare,22, SEEK_SET);
    if(read(fisierIntrare,&height,4)==-1){
        perror("Nu s-a realizat corect citirea lungimii.\n");
        exit(-1);
    }
    
    
	lseek(fisierIntrare,0,SEEK_SET);
    printf("width=%d and height=%d\n", width, height);

    lseek(fisierIntrare,54,SEEK_SET);
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            int redPixel, greenPixel, bluePixel;
            
            if((read(fisierIntrare,&redPixel,1))==-1){
        		perror("Nu s-a putut citi pixelul albastru.\n");
        		exit(-1);
    		}
    		
    		if((read(fisierIntrare,&greenPixel,1))==-1){
        		perror("Nu s-a putut citi pixelul albastru.\n");
        		exit(-1);
    		}
    		
    		if((read(fisierIntrare,&bluePixel,1))==-1){
        		perror("Nu s-a putut citi pixelul albastru.\n");
        		exit(-1);
    		}
            
            int greyPixel = 0.299*redPixel + 0.587*greenPixel + 0.114*bluePixel;
            lseek(fisierIntrare,-3,SEEK_CUR);
            write(fisierIntrare,&greyPixel,1);
            write(fisierIntrare,&greyPixel,1);
            write(fisierIntrare,&greyPixel,1);
        }
    }
    lseek(fisierIntrare,0,SEEK_SET);
    printf("Am colorat cu succes!\n");
}




void travelDirector(DIR *director_intrare,const char *path_intrare, const char *path_iesire){
		
	struct stat checkFile;
	struct dirent *index;
	
	while((index = readdir(director_intrare)) != NULL){
		int pid , status = -1;
		printf("index is:%s\n", index->d_name);
		pid = fork();
		
		if(pid < 0){
			perror("Nu putem creea procese\n");
			exit(22);
		}
		if(pid == 0){
			char aux[400];
			int fisierIesire = 0;
			char numeFisierStatistica[120];
			char bufferIesire[120];
			sprintf(aux, "%s%s", path_intrare, index->d_name);
			printf("aux.........%s\n", aux);
			
			if(lstat(aux, &checkFile) !=0){
				perror("Path-ul nu e bun\n");
				exit(-1);
			}
			sprintf(numeFisierStatistica,"%s_%s", index->d_name, "statistica.txt");
			
			sprintf(bufferIesire, "%s/%s", path_iesire, numeFisierStatistica);
			
			if((fisierIesire=open(bufferIesire, O_WRONLY|O_TRUNC|O_CREAT,S_IRWXU))<0){
        		perror("Nu se poate crea fisierul de iesire");
        		exit(10);
    		}
			if(S_ISREG(checkFile.st_mode)){
				char aux2[100]; 
				char buffer[100];
    			strcpy(aux2, index->d_name);
    			char *trunc;
    			trunc=strrchr(aux2, '.');
    			sprintf(buffer, "\nnumele fisierului: %s", index->d_name);
    			write(fisierIesire,buffer, strlen(buffer));
    			if(strcmp(trunc,".bmp") == 0){ // este fisier bmp
    				printWidthAndHeight(aux, fisierIesire);
    			}
    			printFileInfo(checkFile, fisierIesire);
    			printAllPermissions(fisierIesire, checkFile);
			}
		
			if(S_ISDIR(checkFile.st_mode)){
				printDirectorInfo(index->d_name, path_iesire);
			}
		
			if(S_ISLNK(checkFile.st_mode)){
				printSymbolicLink(checkFile, fisierIesire);
				//cum afisez dimensiunea fisierului pointat de symbolic link?
			}
			if((close(fisierIesire))!=0){
				perror("eroare la inchidere fisier iesire\n");
				exit(222);
			}
			exit(0);
		}
		
		const char *separator = strrchr(index->d_name, '.');
	
    	if((strstr(index->d_name,".bmp") == separator) && separator!=NULL){ // este fisier bmp
    		pid = fork();
    		if(pid < 0){
    			perror("Nu s-a putut crea proces pt imagine\n");
    			exit(233);
    		}
    		if(pid == 0){
    			int fisierIesire = 0;
    			int poza = 0;
				char numeFisierStatistica[120];
				char bufferIesire[120];
				char bufferIntrare[120];
			
    		
    			sprintf(bufferIntrare, "%s/%s", path_intrare, index->d_name);
    		
    			if((poza=open(bufferIntrare, O_RDWR, S_IRWXU)) < 0) {
    				perror("Nu se poate deschide fisierul de intrare");
    				exit(10);
				}
    		
    			colorImage(poza);
    			if((close(poza))!=0){
					perror("eroare la inchidere fisier pooooza\n");
					exit(222);
				}
    			exit(0);
    		}
    	}
    	wait(&status);
		if(!WIFEXITED(status)){
            perror("eroare nu exista");
            exit(22);
        }
		printf("S-a incheiat cu succes procesul cu pid=%d si cod=%d\n", pid, WEXITSTATUS(status));
	}
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
    
  
	
    if(stat(argv[1], &checkFile) != 0){
    	perror("Nu luam bine statutul fisierului!\n");
    	exit(-1);
    } 
    
    if(!S_ISDIR(checkFile.st_mode)){
        perror("Nu trimitem director intrare!\nIncarcati altceva!");
        exit(-1);
    } 
  	
  	if(stat(argv[2], &checkFile) != 0){
    	perror("Nu luam bine statutul fisierului!\n");
    	exit(-1);
    } 
    
    if(!S_ISDIR(checkFile.st_mode)){
        perror("Nu trimitem director iesire!\nIncarcati altceva!");
        exit(-1);
    } 
      
    DIR *director_intrare;
    if((director_intrare = opendir(argv[1])) == NULL){
 		perror("Nu s-a putut deschide director intrare");
 		exit(22);
    }  
  	travelDirector(director_intrare,argv[1], argv[2]);
  
    //close(fisierIesire);
    return 0;
}