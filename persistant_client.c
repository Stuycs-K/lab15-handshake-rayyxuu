#include "pipe_networking.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int to_server;
int from_server;
int client_running = 1;

void sigint_handler(int signo) {
    if (signo == SIGINT) {
        printf("\nClient shutting down gracefully...\n");
        client_running = 0;
        close(from_server);
        close(to_server);
        exit(0);
    }
}

int main() {
    signal(SIGINT, sigint_handler);
    from_server = client_handshake(&to_server);
    if (from_server == -1 || to_server == -1) {
        printf("Error during handshake\n");
        return 1;
    }
    printf("Handshake successful, waiting for server...\n");
    sleep(1);
    while (client_running) {
        int randnum;
        if (read(from_server, &randnum, sizeof(randnum)) <= 0) {
            printf("Error reading from server\n");
            break;
        }
        if (randnum == -1) {
            printf("Server exited, exiting.\n");
            break;
        }
        printf("Received: %d\n", randnum);
    }
    printf("Server disconnected. Exiting...\n");
    close(from_server);
    close(to_server);
    return 0;
}