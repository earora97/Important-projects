#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/resource.h>
#include<pwd.h>
#include<string.h>
#include<sys/wait.h>
// #include "built.c"

char hostname[200],cwd[200],string[100],thishome[200]="/OS";
char *tokenPtr,*dir;

int lsh_launch(char *args)
{
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		// Child process
		if (execvp(args, &args) == -1) {
			perror("lsh");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// Error forking
		perror("lsh");
	} else {
		// Parent process
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}



int main()
{
	int i=0,count=0;
	gethostname(hostname, sizeof(hostname));
	struct passwd *p = getpwuid(getuid());
	while(1)
	{
		getcwd(cwd, 200);
		dir = strstr(cwd, thishome);
		if(strcmp(dir,thishome)==0)
			dir = "~"; 
		printf("<%s@%s:%s>", p->pw_name,hostname,dir);
		scanf("%[^\n]%*c",string);
		tokenPtr = strtok(string, " ");
		while (tokenPtr!=NULL)
		{
			if(strcmp(tokenPtr,"pwd")==0)
			{
				getcwd(cwd, sizeof(cwd));
				printf("%s\n",cwd);
			}
			else if(strcmp(tokenPtr,"cd")==0)
			{
				tokenPtr = strtok(NULL, " ");
				chdir(tokenPtr);
			}
			else if(strcmp(tokenPtr,"echo")==0)
			{
				tokenPtr = strtok(NULL, " ");
				printf("%s\n",tokenPtr);
			}
			else if(strcmp(tokenPtr,"pinfo")==0)
			{
				int pid;
				printf("pid -- ");
				//	struct rusage r_usage;
				//	getrusage(RUSAGE_SELF,&r_usage);
				//	printf("Memory usage: %ld bytes\n",r_usage.ru_maxrss);
				scanf("%d",&pid);
				FILE * status = fopen("/proc/pid/status", "r");
			}
			else
			{
			//	char *ab = tokenPtr;
				lsh_launch(tokenPtr);
			}
			tokenPtr = strtok(NULL, " ");
		}
	}
	return 0;
}
