#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void hupHandler(int signum) {
   printf("Ouch!\n");
}

void intHandler(int signum) {
    printf("Yeah!\n");
}

int main(int argc, char *argv[]) {
    signal(SIGHUP, hupHandler);
    signal(SIGINT, intHandler);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Invalid input: n must be a positive integer\n");
        return 1;
    }

    int evenNumber = 0;

    while (n > 0) {
        printf("%d\n", evenNumber);
        fflush(stdout);
        sleep(5);
        evenNumber += 2;
        n--;
    }

    return 0;
}
