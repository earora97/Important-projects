#include<header.h>

extern char* line;
int status;
int chldrun=0;
extern char **args;
extern char cwd[200];

void sig_chld(int signo)
{
	chldrun=0;
	pid_t pid;
	int stat;
	pid=wait(&stat);
	if(pid==-1)
		return;
	printf("Process with pid %d terminated now\n",pid);
	return;
}

int eve_launch(char **args)
{
	pid_t pid,wpid;
	int status;
	if(chldrun==0){
		signal(SIGCHLD, sig_chld);
		chldrun=1;}
	pid=fork();

	//childprocess
	if(pid==0)
	{
		if(execvp(args[0],args)==-1)
			fprintf(stderr,"error executing the command\n");
		chldrun=0;
	}
	//couldn't fork
	else if(pid<0)
	{
		fprintf(stderr,"error: fork\n");
		chldrun=0;
	}

	//parent process
	else
	{
		int i;
		//search if '&' is given as an argument
		for(i=0;args[i]!=NULL || args[i]!='\0';i++)
		{
			if(strcmp(args[i],"&")==0)
			{
				//chldpid=pid;
				return 1;
			}
		}

		do{
			wpid=waitpid(pid, &status,WUNTRACED);
		}while(!WIFEXITED(status) && !WIFSIGNALED(status));
		chldrun=0;
	}

	return 1;

}


char *eve_read_line(void)
{
	char *line = NULL;
	ssize_t bufsize = 0;
	getline(&line, &bufsize, stdin);
	return line;
}

//READ
/* #define EVE_RL_BUFSIZE 1024
   char *eve_read_line(void)
   {
   int bufsize=EVE_RL_BUFSIZE;
   int position=0;
   char *buffer=malloc(sizeof(char)*bufsize);
   int c;

//unable to allocate buffer
if(!buffer)
{
fprintf(stderr, "eve: allocation error\n");
exit(1);
}

//read character by character
while(1)
{
c=getchar();
//end of list of arguments, make last character=null
if(c==EOF||c=='\n')
{
buffer[position]='\0';
return buffer;
}
//keep reading
else
buffer[position]=c;

position++;

//we exceeded the buffer, reallocate it
if(position >= bufsize)
{
bufsize += EVE_RL_BUFSIZE;
buffer = realloc(buffer,bufsize);
if(!buffer)
{
fprintf(stderr, "eve: allocation error\n");
exit(1);
}
}
}
}
//END READONG
 */

#define EVE_TOK_BUFSIZE 64
#define EVE_TOK_DELIM " \t\n\r"

char **eve_split_line(char *line)
{

	int bufsize=EVE_TOK_BUFSIZE, position=0;
	char **tokens=malloc(bufsize*sizeof(char *));
	char *token;

	//can't allocate buffer
	if(!tokens)
	{
		fprintf(stderr, "Allocation\n");
		exit(1);
	}

	//save arguments in a 2-d array tokens
	token=strtok(line, EVE_TOK_DELIM);
	while(token!=NULL)
	{
		tokens[position]=token;
		//	printf("%s:",token);
		position++;

		//if buffer full, reallocate buffer
		if(position>=bufsize)
		{
			bufsize += EVE_TOK_BUFSIZE;
			tokens=realloc(tokens, bufsize*sizeof(char*));
			if(!tokens)
			{
				fprintf(stderr, "Allocation error\n");
				exit(1);
			}
		}

		//read next argument
		token=strtok(NULL, EVE_TOK_DELIM);
	}
	tokens[position]=NULL;
	//	printf("\n");
	return tokens;
}


void compare()
{

	if(strcmp(args[0],"pwd")==0)
	{
		getcwd(cwd, sizeof(cwd));
		printf("%s\n",cwd);
	}
	else if(strcmp(args[0],"cd")==0)
	{
		chdir(args[1]);
	}
	else if(strcmp(args[0],"echo")==0)
	{
		printf("%s\n",args[1]);
	}
	else if(strcmp(args[0],"pinfo")==0)
	{
		int c;
		char buf[80],buff[20],buff2[20],pid[20];
		strcpy(pid,args[1]);
		strcpy(buff,"/proc/");
		strcpy(buff2,"/cmdline");
		strcat(buff,pid);
		strcat(buff,buff2);
		//	printf("buff::%s\n",buff);
		char *filename = buff;
		FILE* file = fopen(filename, "r");
		if(file)
		{
			printf("Executable file: ");
			while((c = getc(file)) != EOF)
			{
				putchar(c);
			}
			fclose(file);
		}
		printf("\n");
		if(args[1]!=NULL)
		{
			//printf("hello\n");
			char infoFile[50],line[100],temp;
			int r,file,j=0;
			strcpy(infoFile,"/proc/");
			strcat(infoFile,args[1]);
			strcat(infoFile,"/status");

			if((file=open(infoFile,O_RDONLY))!=-1)
			{
				while((r=read(file,&temp,sizeof(char)))!=0)
				{
					if(temp!='\n')
					{
						line[j]=temp;
						j++;
					}
					else
					{
						if (strstr(line,"State")!=NULL)
							printf("%s\n",line );
						else if (strstr(line,"VmSize")!=NULL)
							printf("%s\n",line );
						memset(line,0,sizeof(line));
						j=0;

					}
				}
			}
		}
	}
	else
	{
		eve_launch(args);
	}
}

	//SPLITTING INTO COMMANDS
#define EVE_TOK_CDELIM ";\n"
	int eve_split_command(char* line)
	{
		//printf("line1:%s--",line);
		//	printf("haha\n");
		int bufsize = EVE_TOK_BUFSIZE, position = 0;
		char *token;
		char *saveptr;
		token = strtok_r(line, EVE_TOK_CDELIM,&saveptr);
		while (token != NULL)
		{
			args=eve_split_line(token);
			//		printf("args:%s\n",args[0]);
			compare();
			token = strtok_r(NULL, EVE_TOK_CDELIM,&saveptr);
		}
		return 1;
	}
	//END SPLITTING INTO COMMANDS
