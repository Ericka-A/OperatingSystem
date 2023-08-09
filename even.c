#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t hup_received = 0;
volatile sig_atomic_t int_received = 0;

void hupHandler(int signum) {
    hup_received = 1;
}

void intHandler(int signum) {
    int_received = 1;
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

    while (n > 0 && !hup_received && !int_received) {
        printf("%d\n", evenNumber);
        fflush(stdout);
        sleep(5);
        evenNumber += 2;
        n--;
    }

    if (hup_received) {
        printf("Ouch!\n");
    }

    if (int_received) {
        printf("Yeah!\n");
    }

    return 0;
}
