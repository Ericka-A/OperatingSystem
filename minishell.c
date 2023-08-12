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
char lineCopyBackup[NL];
int backgroundCounter = 0;

typedef enum {
    NotAssigned,
    Running,
    Completed
} BackgroundEnumStatus;

typedef struct {
    int pid;
    int counter;
    char command[NL];
    BackgroundEnumStatus status;
} BackgroundProcess;

BackgroundProcess backgroundProcess[MB];

void printBackgroundDoneAndClearProcess(int idx) {
    fprintf(stdout, "[%d]+ Done %s\n", backgroundProcess[idx].counter, backgroundProcess[idx].command);
    backgroundProcess[idx].status = NotAssigned;
}

void checkForCompletedbackgroundProcess() {
    for (int i = 1; i <= backgroundCounter; ++i) {
        if (backgroundProcess[i].status != NotAssigned) {
            int status;
            int result = waitpid(backgroundProcess[i].pid, &status, WNOHANG);
            if (result > 0) {
                printBackgroundDoneAndClearProcess(i);
            }
        }
    }
}

int main(int argc, char *argv[], char *envp[]) {
    char *v[NV];
    char *sep = " \t\n";
    bool isBackground;

    while (1) {
        //prompt();
        printf("msh> ");
        fgets(line, NL, stdin);
        fflush(stdin);

        if (feof(stdin)) {
            printf("\nEnd of file reached.\n");
            exit(0);
        }
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\000')
            continue;

        for (int i = 0; i < NL; i++) {
            lineCopyBackup[i] = line[i];
        }

        v[0] = strtok(line, sep);

        int i;
        for (i = 1; i < NV; i++) {
            v[i] = strtok(NULL, sep);
            if (v[i] == NULL)
                break;
        }

        if (strcmp(v[0], "cd") == 0) {
            if (v[1] != NULL) {
                if (chdir(v[1]) != 0) {
                    perror("cd");
                }
            } else {
                // Go to home directory
                chdir(getenv("HOME"));
            }
            continue;
        } else {
            isBackground = (strcmp(v[i - 1], "&") == 0);

            if (isBackground) {
                backgroundCounter++;
                isBackground = true;
                v[i - 1] = NULL;

                int len = strlen(lineCopyBackup);
                if (len - 3 >= 0) {
                    lineCopyBackup[len - 3] = '\0';
                } else if (len - 2 >= 0) {
                    lineCopyBackup[len - 2] = '\0';
                }
            }

            switch (pidChild = fork()) {
                case -1: /* fork returns error to parent process */
                {
                    break;
                }
                case 0: /* code executed only by child process */
                {
                    if (isBackground) {
                        printf("[%d] %d\n", backgroundCounter, getpid()); //formed this logic with help of Geeks for Geeks and chatGpt
                        fflush(stdout);
                        backgroundProcess[backgroundCounter].pid = getpid();
                        backgroundProcess[backgroundCounter].counter = backgroundCounter;
                        strncpy(backgroundProcess[backgroundCounter].command, lineCopyBackup, NL);
                        backgroundProcess[backgroundCounter].status = Running;
                    }

                    execvp(v[0], v);
                    perror("execvp");
                    exit(1);
                }
                default: /* code executed only by parent process */
                {
                    if (!isBackground) {
                        int status;
                        waitpid(pidChild, &status, 0);
                    } else {
                        checkForCompletedbackgroundProcess();
                    }
                    break;
                }
            }
        }
    }

    return 0;
}
