#include<header.h>

char hostname[200],cwd[200],string[100],thishome[200]="/OS";
char *line,*dir;
int status;
char **args;

int main(int argc, char* argv[])
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
		else
		{
			char *temp=strtok(dir,thishome);
			strcpy(dir,"~");
			strcat(dir,temp);
		}
		printf("<%s@%s:%s>", p->pw_name,hostname,dir);
		line = eve_read_line();
		status = eve_split_command(line);
	}
	return 0;
}
