#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/resource.h>
#include<pwd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<signal.h>

void sig_chld(int signo);
int eve_launch(char **args);
char *eve_read_line(void);
char **eve_split_line(char *line);
void compare();
int eve_split_command(char* line);