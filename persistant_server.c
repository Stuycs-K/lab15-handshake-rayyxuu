#include "pipe_networking.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int to_client;
int from_client;
int server_running = 1;

void sigint_handler(int signo) {
    if (signo == SIGINT) {
        printf("\nServer shutting down gracefully...\n");
        unlink(WKP);
        server_running = 0;
        if (to_client > 0) {
            int sig = -1;
            write(to_client, &sig, sizeof(sig));
        }
        close(to_client);
        close(from_client);
        exit(0);
    }
}

int main() {
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);

    while (server_running) {
        printf("Waiting for a new client...\n");
        from_client = server_handshake(&to_client);
        if (from_client == -1 || to_client == -1) {
            printf("Error during handshake. Continuing to next client...\n");
            continue;
        }
        printf("Client connected. Sending...\n");
        srand(time(NULL));
        sleep(1);
        while (1) {
            int randnum = rand() % 101;
            printf("Sending: %d\n", randnum);
            int test = write(to_client, &randnum, sizeof(randnum));
            //printf("%d\n", test);
            if (test <= 0) {
                printf("Error writing to client\n");
                break;
            }
            sleep(1);
            int recv_sig;
            if (read(from_client, &recv_sig, sizeof(recv_sig)) > 0 && recv_sig == -1) {
                printf("Client sent termination signal. Disconnecting...\n");
                break;
            }
        }
        printf("Client disconnected\n");
        close(from_client);
        close(to_client);
    }
    return 0;
}
