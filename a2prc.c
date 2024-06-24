#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/signal.h>

#define PROC_PATH 256  // proc_path max length
#define MAX_PROCESS 32768 // maximum number of process that linux kernel create

//zombie method take one argument as integer process id and return the integre saying that the process is zombie or not.
int zombie(int processId){
	// allocate memory to path variable
	char *path = malloc(PROC_PATH * sizeof(char));
	// replace the placeholder %d with process id
	sprintf(path,"/proc/%d/task/%d/stat",processId,processId);
	int pathFd = open(path,O_RDONLY); // open file in read only mode
	if(pathFd == -1){
		printf("\n Unable to open file for a process %d\n",processId);
		exit(EXIT_FAILURE);
	}
	free(path);// free allocated memory
	char buff[1024];
	read(pathFd, buff, sizeof(buff));
	char *arr = strtok(buff, " ");
	int index=0;// to track the status of process
	int flag=0; // to identify the process is zombie or not.
	while( arr!= NULL ){
		if(index++==2){// to get process running status
			if(strcmp(arr,"Z")==0){ // Z =  defunct
				flag=1;
			}else{
				flag=-1;
			}
			break;
		}
		arr = strtok(NULL, " "); // next token
	}
	close(pathFd); //  close file
	return flag;
}

// sigcont method send SIGCONT  signal to the stopped process.
int sigcont(){
	int i=2; // process id always greater than 0 and init have process id 1;
	int count=0;
	while(i<MAX_PROCESS){
		//allocate memory for proc path
		char *path = malloc(PROC_PATH * sizeof(char));
		sprintf(path,"/proc/%d/task/%d/stat",i,i);
		int pathFd = open(path,O_RDONLY); // open file in read only mode/
		if(pathFd == -1){
			i++;
			continue;
		}
		free(path);// free allocated memory
		char buff[1024];
		read(pathFd, buff, sizeof(buff));
		char *arr = strtok(buff, " ");
		int index=0; // to track the process status
		while( arr!= NULL ){
			if(index++==2){
				if(strcmp(arr,"T")==0){ // check if process is in  in stopped status or not
					if(kill(i, SIGCONT)==-1){ // sent the SIGCONT to the stopped process
						printf("Error while resuming process %d \n", i);
					}else{
						count++;
						printf("Process %d resumed successfully \n",i);
					}
				}
			}
			arr = strtok(NULL, " "); // next token
		}
		i++;
		close(pathFd);
	}
	return count;
}

//grandchild method takesm 3 parameters 1. processid 2. level 3. dupli  array
// processid :  pid of the process
// level : depth of the process
// dupli : to check process already visited or not. 
void grandchild(int processId,int level,int dupli[]){
	if(processId<0){ // process id always greater than 0;
		return;
	}
	char path[256];
	sprintf(path,"/proc/%d/task/%d/children", processId,processId);
	int pathFd = open(path,O_RDONLY); // open file in read only mode
	if(pathFd == -1){
		return;
	}
	char buffer[1024];
	int n=read(pathFd,buffer,sizeof(buffer));
	char *arr = strtok(buffer," ");
	int index=0;
	int childProcess[2048]; // to create array that stores the child id of the process
	while(arr != NULL){
		int childId = atoi(arr); // convert string to integer
		childProcess[index++] = childId;
		arr = strtok(NULL," "); // next token
	}
	for(int i=0;i<index;i++){
		if(dupli[childProcess[i]]!=1){ // if process is not traversed 
			dupli[childProcess[i]]=1;
			grandchild(childProcess[i],level+1,dupli); // recursive call the same function to find grand child of the each child.
		}
	}
	close(pathFd);
	if(level==2){ //depth == 3 means, grand child of the process 
		printf("%d\n", processId);
	}
}

// allchildzombie find the  all descendents of the process that  are defunct
void allchildzombie(int processId,int dupli[]){
	if(processId<0){ // process id always greater than 0
		return;
	}
	char path[256];
	sprintf(path,"/proc/%d/task/%d/children", processId,processId);
	int pathFd = open(path,O_RDONLY); // open file in read only mode
	if(pathFd == -1){
 		return;
	}
	char buffer[1024];
	int n=read(pathFd,buffer,sizeof(buffer));
	char *arr = strtok(buffer," ");
	int index=0;
	int childProcess[2048];
	while(arr != NULL){
		int childId = atoi(arr);
		childProcess[index++] = childId; // create a child process to hold  pid of child process
		arr = strtok(NULL," "); // next token
	}
	for(int i=0;i<index;i++){
		if(dupli[childProcess[i]]!=1){
			dupli[childProcess[i]]=1;
			allchildzombie(childProcess[i],dupli); // recursively call the same function to check the childrens of children
		}
	}
	if(zombie(processId)!=-1){ // call zombie method to check process is in defunct mode or not.
		printf("%d defunct\n",processId);
	}
	close(pathFd);//close file/
}

//this method is use to get all non direct descendents of the process id.
void nddescendents(int processId,int level,int parentId,int dupli[]){
	if(processId<0){//process id greater than 0
		return;
	}
	char path[256];
	sprintf(path,"/proc/%d/task/%d/children", processId,processId);
	int pathFd = open(path,O_RDONLY); // open file in read only mode.
	if(pathFd == -1){
		return;
	}
	char buffer[1024];
	int n=read(pathFd,buffer,sizeof(buffer));
	char *arr = strtok(buffer," \n");
	int index=0;
	int childProcess[2048];
	while(arr != NULL){
		int childId = atoi(arr);
		childProcess[index++] = childId;
		arr = strtok(NULL," \n"); //next token
	}
	for(int i=0;i<index;i++){
		if(childProcess[i]!=parentId && dupli[childProcess[i]]!=1){
			dupli[childProcess[i]]=1;
			nddescendents(childProcess[i],level+1,parentId,dupli);
		}
	}
	close(pathFd); // close file
	if(level>1){ // depth == 1 means non direct descendents of the process id.
		printf("%d\n", processId);
	}

}

// child method is used to get immediate child of the process.
int child(int processId,int pid){
	char path[256];
	sprintf(path,"/proc/%d/task/%d/children",processId,processId);
	int pathFd = open(path, O_RDONLY);//open in read only mode
	if(pathFd == -1){
		printf("Unable to open file for a process %d\n",processId);
		exit(EXIT_FAILURE);
	}
	char buffer[1024];
	int n = read(pathFd, buffer, sizeof(buffer)); // read file 
	char *arr = strtok(buffer," ");
	while(arr!=NULL && atoi(arr)!=0){
		if(pid!=atoi(arr)) // condition for sibling 
			printf("%d\n",atoi(arr));
		arr = strtok(NULL," ");// next token
	}
	return processId;
}

void printPID(int pid,int ppid, char* option){
	int exist=1; //check process id belongs to the root process tree or not 
	int i=0; //  to track the parent of the process id
	int parentId = 0; // to hold the process id of the parent
	int currentPID = pid; // to hold the current process id
	while(pid!=ppid && pid!=1){
		char *path = malloc(PROC_PATH * sizeof(char));
		sprintf(path,"/proc/%d/task/%d/stat",pid,pid);
		int pathFd = open(path, O_RDONLY); // open file in read only mode
		if(pathFd==-1){
			printf("\n Unable to open process %d file \n",pid);
			exit(EXIT_FAILURE);
		}
		free(path); // free  memory
		char buff[1024];
		read(pathFd,buff,256);
		char *arr = strtok(buff," ");
		int index=0;
		while(arr!=NULL){
			if(index==3){ // to get parent id of the process 
				pid=atoi(arr);
				if(i++==0)parentId = pid;
				break;
			}
			arr = strtok(NULL," "); //next token 
			index++;
		}

		if(pid < ppid){
			exist=0;
			break;
		}
	}
	// process belongs to the tree of the root process and option is not null 
	if(exist==1 && option != NULL){
		if(strcmp(option,"-rp")==0){ // process id kill
			if(kill(currentPID,SIGTERM)==-1){
				printf("Unable to kill a process %d \n", currentPID);
			}else{
				printf("Process killed %d \n", currentPID);
			}
		}else if(strcmp(option,"-pr")==0){ // root process kill
			if(kill(ppid,SIGTERM)==-1){
				printf("Unable to kill a root process %d \n",ppid);
			}else{
				printf("Root process killed %d \n",ppid);
			}
		}
		else if(strcmp(option,"-xt")==0){ // stop the process id
			if(kill(currentPID,SIGSTOP)==-1){
				printf(" Unable to Pause a process %d \n",currentPID);
			}else{
				printf(" Pause a process %d \n",currentPID);
			}
		}else if(strcmp(option,"-xc")==0){
			sigcont();  // send SIGCONT to the all stopped process
		}
		else if(strcmp(option,"-xs")==0){
			child(parentId,currentPID); // to get sibling of the processid
		}else if(strcmp(option,"-xd")==0){
			child(currentPID, currentPID); // to get direct child of the process id
		}
		else if(strcmp(option,"-xn")==0){ // to get non direct child of the process id
			int *dupli = calloc(MAX_PROCESS, sizeof(int));
			nddescendents(currentPID,0,parentId,dupli);
			free(dupli);
		}
		else if(strcmp(option,"-zs")==0){ // to check defunct status of the process id
			if(zombie(currentPID)==1){
				printf("Defunct\n");
			}else{
				printf("Not Defunct\n");
			}
		}
		else if(strcmp(option,"-xg")==0){ // to get all grand child of the process id
			int *dupli = calloc(MAX_PROCESS,sizeof(int));
			grandchild(currentPID,0,dupli);
			free(dupli);
		}else if(strcmp(option,"-xz")==0){ // to get all defunct descendents of the process id
			int *dupli = calloc(MAX_PROCESS, sizeof(int));
			allchildzombie(currentPID,dupli);
			free(dupli);
		}
	}
	else if(exist==1 && option == NULL){ // process id belongs to the root process and option is not provided simply print the process id and parent process id.
		printf("%d %d \n",currentPID,parentId);
	}
	else{
		printf("Does not belong to process tree \n");
	}
}

int main(int argc, char *argv[]){
	if(argc<=2 || argc >4){
		printf("Invalid number of arguments\n");
		printf("Usage a2prc <processID> <rootProcess> [option] \n");
		exit(EXIT_FAILURE);
	}

	if(argc==4 && !( strcmp(argv[3],"-rp")==0 || strcmp(argv[3],"-pr")==0 || strcmp(argv[3],"-xn")==0 || strcmp(argv[3],"-xd")==0 || strcmp(argv[3],"-xs")==0 ||
			strcmp(argv[3],"-xt")==0 || strcmp(argv[3],"-xc")==0 || strcmp(argv[3],"-xz")==0 || strcmp(argv[3],"-xg")==0 || strcmp(argv[3],"-zs")==0)){
		printf("Invalid option provided \n");
		printf("Valid options are -rp, -pr, -xn, -xd, -xs, -xt, -xc, -xz, -xg and -zs");
		exit(EXIT_FAILURE);

	}
	int pid = atoi(argv[1]);
	int rootId = atoi(argv[2]);
	printPID(pid,rootId,argv[3]);
}
