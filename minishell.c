#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

#define NV 20       /* max number of command tokens */
#define NL 100      /* input buffer size */
#define MB 100      /* max no of background commands */

int pidChild;		 /* value returned by fork sys call */
char line[NL];          /* command input buffer */
char lineCopy[NL];
int backgroundCounter = 0;

typedef enum {
    UnAssigned,
    ProcessRunning,
    Completed
} BackgroundStatusEnum;

typedef struct {
    int pid;
    int counter;
    char command[NL];
    BackgroundStatusEnum status;
} BackgroundProcess;

BackgroundProcess backgroundProcess[MB];

void printBackgroundProcessDoneAndProcess(int index) {
    fprintf(stdout, "[%d]+ Done %s\n", backgroundProcess[index].counter, backgroundProcess[index].command);
    backgroundProcess[index].pid = -1;
    backgroundProcess[index].counter = -1;
    backgroundProcess[index].status = UnAssigned;
    strncpy(backgroundProcess[index].command, "", NL);
}

void checkForCompletedbackgroundProcess() {
    int controlVariable = 0;

    for (int i = 0; i < MB; ++i) {
        if (backgroundProcess[i].pid != -1 && backgroundProcess[i].status != UnAssigned) {

            int status;
            int result = waitpid(backgroundProcess[i].pid, &status, WNOHANG); //extern __pid_t waitpid (__pid_t __pid, int *__stat_loc, int __options);
            if (result > 0) {
                printBackgroundProcessDoneAndProcess(i);
            }
            if (backgroundProcess[i].status == ProcessRunning) {
                controlVariable++;
            }
        }
    }
    if (controlVariable == 0) {
        fflush(stdout);
        backgroundCounter = 0;
    }
}

void prompt(void)
{
    // fprintf(stdout, "\n msh> ");
    // fflush(stdout);
}

int main(int argc, char *argv[], char *envp[])
{
    int childPid;
    char *v[NV];
    char *sep = " \t\n";
    bool isBackground;

    while (1)
    {
        prompt();
        fgets(line, NL, stdin);
        fflush(stdin);

        if (feof(stdin))
        {
            printf("\nEnd of file reached.\n");
            exit(0);
        }

        if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
            continue;

        for (int i = 0; i < NL; i++) {
            lineCopy[i] = line[i];
        }

        v[0] = strtok(line, sep);

        int i;
        for (i = 1; i < NV; i++)
        {
            v[i] = strtok(NULL, sep);

            if (v[i] == NULL)
                break;
        }

        if (strcmp(v[0], "cd") == 0)
        {
            if (v[1] != NULL)
            {
                if (chdir(v[1]) != 0)
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            isBackground = (strcmp(v[i - 1], "&") == 0);
            if (isBackground)
            {
                isBackground = true;
                v[i - 1] = NULL;

                int len = strlen(lineCopy);
                if (len - 3 >= 0)
                {
                    lineCopy[len - 3] = '\0';
                }
                else if (len - 2 >= 0)
                {
                    lineCopy[len - 2] = '\0';
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
                    if (isBackground)
                    {
                        backgroundCounter++;
                        printf("[%d] %d\n", backgroundCounter, getpid()); //extern __pid_t getpid (void) __THROW;
                        fflush(stdout);

                        backgroundProcess[backgroundCounter].pid = getpid();
                        backgroundProcess[backgroundCounter].counter = backgroundCounter;
                        strncpy(backgroundProcess[backgroundCounter].command, lineCopy, NL);
                        backgroundProcess[backgroundCounter].status = ProcessRunning;
                    }
                    execvp(v[0], v);
                    exit(0);
                }
                default:
                {
                    if (!isBackground)
                    {
                        int status;
                        waitpid(childPid, &status, 0);
                        checkForCompletedbackgroundProcess();
                    }
                    else
                    {
                        backgroundCounter++;
                        backgroundProcess[backgroundCounter].pid = childPid;
                        backgroundProcess[backgroundCounter].counter = backgroundCounter;
                        strncpy(backgroundProcess[backgroundCounter].command, lineCopy, NL);
                        backgroundProcess[backgroundCounter].status = ProcessRunning;

                        checkForCompletedbackgroundProcess();
                    }
                    break;
                }
            }
        }
    }

    return 0;
}
