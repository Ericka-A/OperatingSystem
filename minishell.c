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

#define NV 20			/* max number of command tokens */
#define NL 100			/* input buffer size */

char line[NL];	/* command input buffer */
char lineCopyBackup[NL];


void prompt()
{
  //fprintf(stdout, "\n msh> ");
  //fflush(stdout);

}


int main(int argk, char *argv[], char *envp[])
/* argk - number of arguments */
/* argv - argument vector from command line */
/* envp - environment pointer */

{
  int      PidChildren;	/* value returned by fork sys call */
  //int      wpid;		/* value returned by wait */
  char     *v[NV];	/* array of pointers to command line tokens */
  char     *sep = " \t\n";/* command line token separators    */
  int       i;		/* parse index */

  /* prompt for and process one command line at a time  */

  while (1) {			/* do Forever */
    prompt();
    fgets(line, NL, stdin);
    fflush(stdin);

    if (feof(stdin)) {		//end-of-file 

      printf("\nEnd of file reached.\n");
      exit(0);
    }
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\000') //skips whenever there is input for emptylines,# and \000
      continue;

    for (int i=0;i<NL;i++){      //Copyofline and taking backup
      lineCopyBackup[i]=line[i];
    }	

    v[0] = strtok(line, sep); //taking input line command as an argument and using delimiter sep which is t&n


    for (i = 1; i < NV; i++) {

      v[i] = strtok(NULL, sep);

      if (v[i] == NULL)
      break;
    }
    /* assert i is number of tokens + 1 */

    /* fork a child process to exec the command in v[0] */

    switch (PidChildren = fork()) {
    case -1:			/* fork returns error to parent process */
      {
	break;
      }
    case 0:			/* code executed only by child process */
      {
	execvp(v[0], v);
	
      }
    default:			/* code executed only by parent process */
      {
	//wpid = wait(0);
	printf("%s done \n", v[0]);
	break;
      }
    }				/* switch */
  }				/* while */
}				/* main */
