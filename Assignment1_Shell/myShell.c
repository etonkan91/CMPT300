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
	while(argv[i] != NULL && i < maxSize)
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

int main() {
  int i;
  int temp;
  char user_commands[50];
  char *argv[50]; 
  char cwd[50];
  pid_t pid;

  while(1) 
  {
    i = 0;
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
	  //1.The internal shell command "exit" which terminates the shell
	  if(strcmp(argv[0],"exit") == 0)
      {
        printf("GoodBye\n");
        exit(0);
      } 
	  else if(check_ampersand(argv, 50) == 1)
	  {
		  //4.A command, with or without arguments, executed in the background using &
		  //printf("I found the ampersand but will do nothing for now \n");
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
			sigset(SIGCHLD, wait(NULL));
		  }
		  //Child Process
		  else
		  {
			//printf("I am before execvp and argv[0] is %s \n", argv[0]);
			//It will read the user_command and execute the argv[0]
			if(execvp(argv[0], argv) == -1)
			{
				printf("ERROR: %s \n", strerror(errno));
			}
		  }
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
			wait(NULL);
		  }
		  //Child Process
		  else
		  {
			//printf("I am before execvp and argv[0] is %s \n", argv[0]);
			//It will read the user_command and execute the argv[0]
			if(execvp(argv[0], argv) == -1)
			{
				printf("ERROR: %s \n", strerror(errno));
			}
		  }
	  }
    }
  }

  return 0;
}
