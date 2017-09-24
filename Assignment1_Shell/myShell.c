/*
// CMPT300 Assignment1
// File name: myShell.c
// Author: Eton Kan
// Student Number: 301235768
// Due Date: Sept 29, 2017
// Date Created: Sept 18,2017
// Last Modify: Sept 23,2017
*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
//For the special case "echo *.txt"
#include <dirent.h>
#include <sys/stat.h>

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
// Feature: check if the user's command is echo * and then change the command to ls
//				  if the user's command is echo *.extension, then look in the current 
//				working directory and output all filename that have matching extension
// Return Values: return 2 if the command is echo *.extension
//				  return 1 if the command is echo *
//				  return 0 if the it is just a normal commmand
*/
int is_echo(char *argv[], int MaxSize)
{
	char cwd[MaxSize];
	char *user_ext = ".";
	char *file_ext = ".";
	struct dirent *p_file;
	struct stat statbuf;
	DIR *p_directory; //Special Data type for opendir
	if(strcmp(argv[0],"echo") == 0 && strcmp(argv[1],"*") == 0)
	{
		//The Linux command echo * is same as the command ls 
		argv[0] = "ls";
		argv[1] = NULL;
		return 1;
	}
	else if(strcmp(argv[0],"echo") == 0 && strchr(argv[1],'.') != NULL)
	{
		//Getting the extension of user's command by looking the last period 
		user_ext = strrchr(argv[1], '.');
		if(user_ext == NULL)
		{
			printf("ERROR: Unable to get user command's extension\n");
			return 0;
		}
		//Getting the current working directory
		getcwd(cwd, sizeof(cwd));
		if(cwd == NULL)
		{
			printf("ERROR: Unable to get current working directory\n");
			return 0;
		}
		//Opening and changing the working directory
		p_directory = opendir(cwd);
		if (p_directory == NULL)
		{
			printf("ERROR: Unable to open directory\n");
			return 0;
		}
		chdir(cwd);
		//Checking all the file extensions in the directory to see if it matches user's extension
		while((p_file = readdir(p_directory)) != NULL)
		{
			//Grabbing information about the file
			lstat(p_file->d_name, &statbuf);
			//Checking the file current pointing to is it a regular file
			if(S_ISREG(statbuf.st_mode))
			{
				//Getting the extension by looking the last period
				file_ext = strrchr(p_file->d_name, '.');
				if (file_ext == NULL)
				{
					//The current file is not a a regular file
					continue;
				}
				//Comparing both extensions
				if(strcmp(file_ext, user_ext) == 0)
				{
					printf("%s ", p_file->d_name);
				}
			}
		}
		closedir(p_directory);
		printf("\n");
		return 2;
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
  int test_echo;
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
	test_echo = 0;
	closefd = 0;
	exist_ampersand = 0;
	sigset(SIGCHLD, SIG_DFL);
	//Getting and Printing Current time
	cur_time = time(NULL);
	time_now = localtime(&cur_time);
	strftime(time_array, 100, "%a %b %d %Y %H:%M:%S# ", time_now);
	printf("%s", time_array);
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
				if((test_echo = is_echo(argv, 50)) == 1)
				{
					if(execvp(argv[0], argv) == -1)
					{
					printf("ERROR: %s \n", strerror(errno));
					}
					continue;
				}
				else if (test_echo == 2)
				{
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


