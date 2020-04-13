#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h> 
#include <stdbool.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
pid_t pid;
char** args;



void quit();
void list(char* dir);
void cd(char* dir);
void help();
void jobs();
void history(FILE *file);
void contproc(char* pid);

struct proc {
	pid_t pid;
	char* pname;
};

struct proc lproc[10000];
int numProc=0;

void sig_handler(int signo) {
	
		
	

	if (signo==SIGINT) {
                if(kill(pid,SIGKILL)==0)
			printf(" Process with pid= %d has  been interrupted\n",pid);
			
		
	}	
	else if (signo==SIGTSTP) {
		
	
		
	                if(kill(pid,SIGSTOP)==0)
				{
				printf("\n Process with pid= %d has been stopped\n",pid);
				struct proc newproc;
				newproc.pid=pid;
				newproc.pname=malloc(sizeof(args[0]));
				newproc.pname=strdup(args[0]);
				lproc[numProc++]=newproc;					
				}	
	}
	fflush(stdout);
//        waitpid(pid,&status,WNOHANG);
		
	
	return;
	
		
           
}




int main(int argc, char **argv)
	{
	int cmdfile=-1;
	if (argc==2) {
		if ((cmdfile=open(argv[1],O_RDONLY, 0700))==-1)
		{
			fprintf(stderr, "Error <%s> No such file\n",argv[1]);
			exit(EXIT_FAILURE);		
		}
		else {
			dup2(cmdfile,STDIN_FILENO);
		}	
	}

        

	char *cmd, *argpiece;
	
	int size,i,status, input=-1,output=-1;
	bool inexist=false,outexist=false;
        setenv("HOME",getenv("PWD"),1);	
	FILE *wf = fopen("blazersh.log","w+");
	for ( ; ; )
		{
	        bool executed=false;
                

	        pid=-1;
		
		size=0;
		cmd=malloc(sizeof(char)*BUFSIZ);
		printf("blazersh> ");
	        
		signal(SIGINT, SIG_IGN);
		signal(SIGTSTP,SIG_IGN);		

		if(fgets(cmd,BUFSIZ,stdin)==NULL) {
			printf("End of file reached\n");
			exit(EXIT_SUCCESS);
		} 
		

	
		
	
		args=malloc(sizeof(char)*BUFSIZ);
		argpiece=malloc(sizeof(cmd));
		argpiece=strtok_r(cmd, " \n\t\v\r\f",&cmd);
                
                for ( ; ; ) {
                	

                	if (argpiece==NULL){
                        	args[size]=(char*)NULL;
                                break;
			}
                        else {  
				if (strcmp(argpiece,"<")==0 && inexist==false)
				{
					if((input=open(strtok_r(NULL, " \n\t\v\r\f",&cmd),O_RDONLY,0700))==-1){
						fprintf(stderr,"Error:\n");
                                                executed=true;		
					}
					else{
						inexist=true;
					}	
				}
				else if (strcmp(argpiece,">")==0 && outexist==false )
                                {
                                        if((output=open(strtok_r(NULL, " \n\t\v\r\f",&cmd),O_WRONLY|O_CREAT|O_TRUNC,0700))==-1){
                                        	fprintf(stderr,"Error:\n");
						executed=true;        
                                 	}   
					else{
						outexist=true;
					}    
                                }

				else{
                        		args[size++]=strdup(argpiece);}
                        	argpiece=strtok_r(NULL, " \n\t\v\r\f",&cmd);
				                                
                           }       
                                
                        }
		
		if (size==1){
                 	if (strcmp(args[0],"quit")==0){
				executed=true;
                        	for (i=0;i<size;i++)
                                	free(args[i]);
                                free(args);
				fclose(wf);
                                quit();
                        }
                        else if (strcmp(args[0],"list")==0)
                                {executed=true;
				list(".");
				}
                	else if (strcmp(args[0],"jobs")==0){
				jobs();
				executed=true;
			}                
			         
			else if (strcmp(args[0],"cd")==0)
                               { executed=true;
				cd(getenv("HOME") );
                               }
			else if (strcmp(args[0],"help")==0) {
				help();
				executed=true;
				}
			
			else if(strcmp(args[0],"history")==0) {
					history(wf);
					executed=true;	
				}
				
                        }

		else if (size==2) {
                        if (strcmp(args[0],"cd")==0){
                               { executed=true; 
				cd(args[1]);
                                }
                                

   
                                }
                        	else if (strcmp(args[0],"list")==0){
                     	           	list(args[1]);
					executed=true;
                        	}

				else if (strcmp(args[0],"continue")==0){
					contproc(args[1]);
					waitpid(pid,&status,0);
					executed=true;
				}

                }
		
		for (i=0; i<size; i++)
			fprintf(wf,"%s ",args[i]);
		fprintf(wf,"\n");			

		if (executed==false) {
		pid=fork();
		if (pid==0){
			free(argpiece);
				
			if (size==0) //Only white space has been entered
				exit(EXIT_SUCCESS);
			if (outexist==true)
				dup2(output,STDOUT_FILENO);
			if (inexist==true)
				dup2(input,STDIN_FILENO);				
				        
					
	

  			execvp(args[0],args);
			perror("Error executing the program\n");			
			exit(-1);
		}

		else if(pid>0){
			

			if (signal(SIGINT,sig_handler)==SIG_ERR) {
				fprintf(stderr, "Error in catching SIGINT\n");
				exit(-1);
			}
			if (signal(SIGTSTP,sig_handler)==SIG_ERR) {
				fprintf(stderr, "Error in catching SIGTSTP\n");
				exit(-1);
			}
			if (signal(SIGCHLD,sig_handler)==SIG_ERR){
				fprintf(stderr,"Could not catch SIGCHLD\n");
				exit(-1);
			}	
		        

			for ( ; ; ){
				 
				if(pause()==-1)
					break;
				
				
			}

                        waitpid(pid,&status,WNOHANG);
 

			if (input!=-1)
				close(input);
			if (output!=-1)
				close(output);

       	
				
		}
		else {

			printf("Error in fork\n");
			exit(-1);
		
		
		}		

	}
}		
}

void quit()
{
	exit(EXIT_SUCCESS);		

}
void list(char* dir)
{
        printf("\n\n");	
	struct dirent *dirent;
	DIR *parentdir;

	parentdir=opendir(dir);

	if (parentdir==NULL){
		printf("Error: <%s> No such directory\n",dir);
	         	
	}
	else{
	while((dirent=readdir(parentdir))!=NULL)
		if (strcmp((*dirent).d_name,".")!=0 &&  strcmp((*dirent).d_name,"..")!=0 )
			printf("%s\n",(*dirent).d_name);
	}
	closedir(parentdir);	
	printf("\n\n");
}
void cd(char* dir){
	
	char* path;
	path = malloc(1+sizeof("./")+sizeof(dir));
	
	if (strcmp(dir,"~")==0 || strcmp(getenv("HOME"),dir)==0) {
		chdir(getenv("HOME"));
		setenv("PWD",getenv("HOME"),1);
		
	}
	else if(strcmp(dir,".")==0)
		return;
	 

	else {  
		strcpy(path,"./");
		path=strcat(path,dir);
	
		if (chdir(path)!=0)
			{
			printf("Error <%s> No such directory\n",dir);
			return; 
			
			}
		setenv("PWD",path,1);
		

	}
	
        
}

void help() {

	fprintf(stdout,"\n\nHere are the available commands for blazersh. Any other programs can also be executed through this shell\n\n");
        fprintf(stdout,"help - provides help on the commands available in blazersh shell\n");
	fprintf(stdout,"cd <directory> - Changes current directory to <directory>. Providing no other arguments changes directory to the one where the blazersh program was executed\n");
	fprintf(stdout,"history - lists a history of commands inputted\n");
	fprintf(stdout, "list <directory> - Lists all the contents of <directory> or the current directory if no directory name is provided\n");
	fprintf(stdout, "quit - Quits the blazersh\n\n");
} 
void history(FILE *file) {
	fprintf(stdout,"\n\n");
	char* line;
	fseek(file,SEEK_SET,0);
	line=malloc(sizeof(char)*BUFSIZ);
	while (fgets(line,BUFSIZ,file)!=NULL){
		fprintf(stdout,"%s\n",line);
		
	}
	free(line);
	fprintf(stdout,"\n\n");		
} 
void jobs() {
	if (numProc!=0){
	printf("Process id\tProcess Name\n");
	int i;
	for (i=0;i<numProc;i++)
		printf("%d\t\t%s\n",lproc[i].pid,lproc[i].pname);	
	}
	else 
		printf("\n");
}
void contproc(char* spid) {
	char *endptr;
	pid_t contpid=strtol(spid,&endptr,10);
	int i,j;

	errno=0;
	if ((errno == ERANGE && (contpid == LONG_MAX || contpid == LONG_MIN))
            || (errno != 0 && contpid == 0)) {
             		perror("strtol");
               		return;
           }

        if (endptr == spid) {
               fprintf(stderr, "No digits were found\n");
               return;
          }
	for (i=0;i<numProc;i++)	
	{
		if(lproc[i].pid==contpid)
		{	if(kill(contpid,SIGCONT)==0){
				printf("Resuming process with pid = %d\n",contpid);
	                        pid=contpid; 
				

				for(j=i ; j<numProc ; j++)
				{
					lproc[j]=lproc[j+1];
				} 
				numProc--;
				return;}
			else
				perror("Could not continue process\n");
	
		}
		

	}
	fprintf(stderr,"Errer: <%d> No such process\n",contpid); 
	}
	
