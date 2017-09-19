/*
// CMPT300 Assignment1
// File name: myShell.c
// Author: Eton Kan
// Student Number: 301235768
// Date: Sept 18,2017
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

/*
// Name: check_ampersand()
// Feature: Looking for '&' followed by NULL in the avgr
// Return Values: if condition is met return 1 
//				  else return 0
*/
int check_ampersand(char *argv[], int maxSize)
{
	int i = 0;
	//printf("i am in check_ampersand \n");
	while(argv[i] != NULL && i < maxSize - 1)
	{
		if(strcmp(argv[i],"&") == 0 && argv[i+1] == NULL)
		{
			argv[i] = NULL;
			return 1;
		}
		i++;
	}
	return 0;
}

/*3
// Name: check_greater_than()
// Feature: Looking for '>' and not followed by NULL in the avgr
// Return Values: if condition is met return the file name location in the argv array return i+1
//				  else if > is followed by NULL return -2
//				  else not found return -1
*/
int check_greater_than(char *argv[], int maxSize)
{
	int i = 0;
	while(argv[i] != NULL && i < maxSize - 1)
	{
		if(strcmp(argv[i],">") == 0 && argv[i+1] != NULL)
		{
			return i+1;
		}
		else if (strcmp(argv[i],">") == 0 && argv[i+1] == NULL)
		{
			return -2;
		}
		else
		{
		i++;
		}
	}
	return -1;
}

/*
// Name: check_greater_than()
// Feature: Looking for '<' and not followed by NULL in the avgr
// Return Values: if condition is met return the file name location in the argv array return i+1
//				  else if > is followed by NULL return -2
//				  else not found return -1
*/
int check_lesser_than(char *argv[], int maxSize)
{
	int i = 0;
	while(argv[i] != NULL && i < maxSize - 1)
	{
		if(strcmp(argv[i],"<") == 0 && argv[i+1] != NULL)
		{
			argv[i] = argv[i+1];
			argv[i+1] = NULL;
			return i+1;
		}
		else if (strcmp(argv[i],">") == 0 && argv[i+1] == NULL)
		{
			return -2;
		}
		else
		{
		i++;
		}
	}
	return -1;
}

/*
// Name: is_echo()
// Feature: check if the user commands is echo * and then change the command to ls
// Return Values: return 1 if the command is echo *
//				  return 0 if the it is just a normal commmand
*/
int is_echo(char *argv[], int maxSize)
{
	if(strcmp(argv[0],"echo") == 0 && strcmp(argv[1],"*") == 0)
	{
		argv[0] = "ls";
		argv[1] = NULL;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
// Name: main()
// Feature: a Shell that supports 'exit', a command with or without arguments,
			commands executed with &, redirecting output to a file, and 
			taking input from a file
// Return Values: No return Values 
//				  Program ends only on 'exit' command from user
*/
int main() {
  int i, exist_ampersand, closefd, exist_greater_than, exist_lesser_than;
  char user_commands[50];
  char *argv[50]; 
  char cwd[50];
  char time_array[100];
  pid_t pid; // for forking
  FILE *fd; //for freopen commands
  time_t cur_time;
  struct tm *time_now;
  
  
  //Infinite loop, exit only when user enter command 'exit'
  while(1) 
  {
    //Setting all values to default values
	i = 0;
	closefd = 0;
	exist_ampersand = 0;
	sigset(SIGCHLD, SIG_DFL);
	//Getting Current time
	cur_time = time(NULL);
	time_now = localtime(&cur_time);
	strftime(time_array, 100, "%a %b %d %Y %H:%M:%S# ", time_now);
	printf("%s", time_array);
	//cur_time_struct = *localtime(&cur_time);
	//printf("%d:%d:%d#", cur_time_struct.tm_hour, cur_time_struct.tm_min, cur_time_struct.tm_sec);
    //Getting the user commands
    if(fgets(user_commands, 50, stdin) == NULL)
    {
      printf("ERROR: Unable to get User Commands\n");
      break;
    }
	
	//Breaking the user commands into tokens by delimiter (space, tab, newline)
	argv[i] = strtok(user_commands, " \t\n");
	while(argv[i] != NULL)
	{
		argv[++i] = strtok(NULL, " \t\n");
	}
	
	//Checking to make sure user command is separated and stored in the argv array
    if(argv[0] != NULL)
    {
	  if(check_ampersand(argv, 50) == 1)
	  {
		  exist_ampersand = 1;
	  }
	  //1.The internal shell command "exit" which terminates the shell
	  if(strcmp(argv[0],"exit") == 0)
      {
        //printf("GoodBye\n");
        exit(0);
      } 
	  else
	  {
		  //2. A command with no arguments
		  //3. A command with arguments
		  pid = fork();
		  //Unable to fork due to error
		  if(pid < 0)
		  {
			printf("ERROR: Unable to fork\n");
			break;
		  }
		  //Parent Process
		  else if(pid != 0)
		  {
			if(exist_ampersand == 1)
			{
				//4.A command, with or without arguments, executed in the background using &
				sigset(SIGCHLD, wait(NULL));
			}
			else
			{
				//wait for the child process to finish
				wait(NULL);
			}
		  }
		  //Child Process
		  else
		  {
			//Checking if there is < or > in the user's command
			exist_greater_than = check_greater_than(argv, 50);
			exist_lesser_than = check_lesser_than(argv, 50);
			//If > exist, then create the file according to the name provided by user
			if(exist_greater_than > 0)
			{
				//Making the file and changing the output to the file
				fd = freopen(argv[exist_greater_than], "w", stdout);
				argv[exist_greater_than - 1] = NULL;
				argv[exist_greater_than] = NULL;
				//Setting a signal so it can close the connection to the file
				closefd = 1;
				//Execute user's command
				if(execvp(argv[0], argv) == -1)
				{
					printf("ERROR: %s \n", strerror(errno));
				}
			}
			else if(exist_greater_than == -2)
			{
				printf("Error: missing file name in the command");
			}
			else
			{
				if(is_echo(argv, 50))
				{
					if(execvp(argv[0], argv) == -1)
					{
					printf("ERROR: %s \n", strerror(errno));
					}
					continue;
				}
				//Execute user's command
				if(execvp(argv[0], argv) == -1)
				{
					printf("ERROR: %s \n", strerror(errno));
				}
			}
			//Closing the file that were opened so the standard output will go back to the terminal
			if(closefd == 1)
			{
				fclose(fd);
			}
		  }
	  }
    }
  }
  return 0;
}

