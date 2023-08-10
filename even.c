#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

volatile int hup_received = 0;
volatile int int_received = 0;

void *signalHandlerThread(void *arg) {
    while (1) {
        if (hup_received) {
            printf("Ouch!\n");
            break;
        }
        if (int_received) {
            printf("Yeah!\n");
            break;
        }
        usleep(100000);  // Sleep for 0.1 seconds
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Invalid input: n must be a positive integer\n");
        return 1;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, signalHandlerThread, NULL);

    int evenNumber = 0;

    while (n > 0 && !hup_received && !int_received) {
        printf("%d\n", evenNumber);
        fflush(stdout);
        sleep(5);
        evenNumber += 2;
        n--;
    }

    // Set the corresponding flag to simulate signal handling
    if (hup_received) {
        hup_received = 0;
    }

    if (int_received) {
        int_received = 0;
    }

    pthread_join(thread, NULL);

    return 0;
}
