#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

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

int main() {
  int i, exist_ampersand, closefd, exist_greater_than, exist_lesser_than;
  char user_commands[50];
  char *argv[50]; 
  char cwd[50];
  char filename;
  pid_t pid;
  FILE *fd;
  
  while(1) 
  {
    i = 0;
	closefd = 0;
	exist_ampersand = 0;
	sigset(SIGCHLD, SIG_DFL);
	//Ask user to type in commands
    printf("Commands:");
    //Get the user commands
    if(fgets(user_commands, 50, stdin) == NULL)
    {
      printf("ERROR: Unable to get User Commands\n");
      break;
    }
	
	// Tokenizer that will take the input and put it in an array
	//Break the buffer into tokens that are separated by a delimiter
	argv[i] = strtok(user_commands, " \t\n");
	while(argv[i] != NULL)
	{
		// printf("%s ", argv[i]);
		// printf("%d", i);
		argv[++i] = strtok(NULL, " \t\n");
	}
	
    if(argv[0] != NULL)
    {
      // printf("I am in the != NULL ");
	  // printf("%s ", argv[0]);
	  if(check_ampersand(argv, 50) == 1)
	  {
		  exist_ampersand = 1;
	  }
	  //1.The internal shell command "exit" which terminates the shell
	  if(strcmp(argv[0],"exit") == 0)
      {
        printf("GoodBye\n");
        exit(0);
      } 
	  else
	  {
		  //2. A command with no arguments
		  //3. A command with arguments
		  pid = fork();
		  //printf("pid is %d \n",pid);
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
				wait(NULL);
			}
		  }
		  //Child Process
		  else
		  {
			exist_greater_than = check_greater_than(argv, 50);
			exist_lesser_than = check_lesser_than(argv, 50);
			if(exist_greater_than > 0)
			{
				fd = freopen(argv[exist_greater_than], "w", stdout);
				argv[exist_greater_than - 1] = NULL;
				argv[exist_greater_than] = NULL;
				closefd = 1;
				if(execvp(argv[0], argv) == -1)
				{
					printf("ERROR: %s \n", strerror(errno));
				}
			}
			else if(exist_greater_than == -2)
			{
				printf("Error: missing file name in the command");
			}
			//It will read the user_command and execute the argv[0]
			else
			{
				if(execvp(argv[0], argv) == -1)
				{
					printf("ERROR: %s \n", strerror(errno));
				}
			}
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
