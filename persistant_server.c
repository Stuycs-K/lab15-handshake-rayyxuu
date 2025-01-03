#include "pipe_networking.h"
#include <time.h>
#include <stdlib.h>

static void sighandler(int tsig) {
    if (tsig == SIGINT){
        printf("Exiting very nicely\n");
        if (unlink(WKP) == -1) {
            printf("WKP closure failed\n");
        }
        exit(0);
    }
}

int main() {
    int to_client;
    int from_client;
    signal(SIGINT, sighandler);    
    signal(SIGPIPE, SIG_IGN);
    while (1) {
        from_client = server_handshake( &to_client );
        sleep(2);
        if (from_client == -1 || to_client == -1) {
            printf("Error during handshake\n");
        }
        else {
            printf("Entering loop\n");
            while (1) {
                sleep(1);
                srand(time(NULL)); 
                int randnum = (rand() % 100) + 1;
                char buffer[20];
                sprintf(buffer, "%d", randnum);
                printf("Server sending: %s\n", buffer);
                if (write(to_client, buffer, strlen(buffer) + 1) <= 0) {
                    if (errno == EPIPE) {
                        printf("Broken pipe detected. Client disconnected.\n");
                    } else {
                        printf("Write failed: %s\n", strerror(errno));
                    }
                    break; // Exit the loop if write fails
                    /*
                    printf("Server sending failed\n");
                    break;
                    */
                }
            }
        }
        close(from_client);
        close(to_client);
    }
    
    
}
