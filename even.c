#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// Signal handler for HUP
void hupHandler(int signum) {
    printf("Ouch!\n");
}

// Signal handler for INT
void intHandler(int signum) {
    printf("Yeah!\n");
}

int main(int argc, char *argv[]) {
    // Set signal handlers
    signal(SIGHUP, hupHandler);
    signal(SIGINT, intHandler);

    if (argc != 2) {
        fprintf(stderr, "The format of the command line is ./even : %s <n>\n", argv[0]);
        return 1;
    }

    int count = atoi(argv[1]);
    if (count <= 0) {
        fprintf(stderr, "Invalid input number must be a positive integer\n");
        return 1;
    }

    int evenNumber = 0;

    for (int i = 0; i < count; i += 2) {
        printf("%d\n", evenNumber);
        fflush(stdout);  // Flush output to ensure it's displayed
        sleep(5);  // Sleep for 5 seconds
        evenNumber += 2;
    }

    return 0;
}
