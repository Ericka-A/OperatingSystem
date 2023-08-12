/*********************************************************************
   Program  : miniShell                   Version    : 1.3
 --------------------------------------------------------------------
   skeleton code for linix/unix/minix command line interpreter
 --------------------------------------------------------------------
   File			: minishell.c
   Compiler/System	: gcc/linux

********************************************************************/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#define NV 20  /* max number of command tokens */
#define NL 100 /* input buffer size */
#define MB 100 /* max no of background commands */
char line[NL]; /* command input buffer */
char lineBackup[NL];
int backgroundCounter = 0;

typedef enum {
	NotAssigned,
	Running,
	Completed
} BgStatus;

typedef struct {
	int pid;
	int counter;
	char command[NL];
	BgStatus status;
} BackgroundProcess;

BackgroundProcess bgProcesses[MB];

void printDoneAndClearBgProcess(int idx) {

	fprintf(stdout, "[%d]+ Done %s\n", bgProcesses[idx].counter, bgProcesses[idx].command);

	bgProcesses[idx].counter = -1;
	bgProcesses[idx].pid = -1;
	bgProcesses[idx].status = NotAssigned;
	strncpy(bgProcesses[idx].command, "", NL);

}

void checkForCompletedBgProcesses() {
	int ctr = 0;

	for (int i = 0; i < MB; ++i) {
		if (bgProcesses[i].pid != -1 && bgProcesses[i].status != NotAssigned) {

			int status;
			int result = waitpid(bgProcesses[i].pid, &status, WNOHANG);
			if (result > 0) {
				printDoneAndClearBgProcess(i);
			}
			if (bgProcesses[i].status == Running) { //completed status not used anymore, no need of enum? maybe try using above &status for this purposes
				ctr++;
			}
		}
	}
	if (ctr == 0) {
		// printf("ctr 0\n");
		fflush(stdout);
		backgroundCounter = 0; //issue...
	}
}


void prompt(void)
{
	// fprintf(stdout, "\n msh> ");
	// fflush(stdout);
}


int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
	int childPid;		 /* value returned by fork sys call */
	// int wpid;			 /* value returned by wait */
	char *v[NV];		 /* array of pointers to command line tokens */
	char *sep = " \t\n"; /* command line token separators    */
	int i;				 /* parse index */
	bool isBackground;

	while (1)
	{ 
		prompt();
		fgets(line, NL, stdin);
		fflush(stdin);

		if (feof(stdin))
		{ 
			exit(0);
		}

		if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
			continue; /* to prompt */

		strcpy(lineBackup, line); 

		v[0] = strtok(line, sep);

		for (i = 1; i < NV; i++)
		{
			v[i] = strtok(NULL, sep);
			
			if (v[i] == NULL)
				break;
		}
	

		if (strcmp(v[0], "cd") == 0) {
			if (v[1] != NULL) {
				if (chdir(v[1]) != 0) {

					continue;
				}
				else {
					
				}
			}
			else {
		
				continue;
			}
		}
		else {

			isBackground = (strcmp(v[i - 1], "&") == 0);
			if (isBackground) {
		
				isBackground = true;
				v[i - 1] = NULL;

			
				int len = strlen(lineBackup);
				if (len - 3 >= 0) {
					lineBackup[len - 3] = '\0';
				}
				else if (len - 2 >= 0) {
					lineBackup[len - 2] = '\0';
				}
				else {
					
				}
				
			}

			

			switch (childPid = fork())
			{
				case -1:
				{
					break;
				}
				case 0: 
				{
					
					if (isBackground) {
						backgroundCounter++;
						printf("[%d] %d\n", backgroundCounter, getpid());
						fflush(stdout);
					}
					execvp(v[0], v);
					
				}
				default: 
				{
					if (!isBackground) {

						int status;
						waitpid(childPid, &status, 0); 
						checkForCompletedBgProcesses(); 

					}
					else {

						backgroundCounter++;

						bgProcesses[backgroundCounter].pid = childPid;
						bgProcesses[backgroundCounter].counter = backgroundCounter; 
						bgProcesses[backgroundCounter].status = Running;
						
						for (int i = 0; i < strlen(lineBackup); ++i) {
							bgProcesses[backgroundCounter].command[i] = lineBackup[i];
						}

						checkForCompletedBgProcesses();

					}
					break;
				}
			} 

		}
	}	 

	return 0;

} 
