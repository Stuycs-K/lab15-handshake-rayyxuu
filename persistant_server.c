#include "pipe_networking.h"

static void sighandler(int tsig) {
    if (tsig == SIGINT){
        printf("Exiting very nicely\n");
        if (unlink(WKP) == -1) {
            printf("WKP closure failed\n");
            printerror();
        }
        exit(0);
    }
}

int main() {
    int to_client;
    int from_client;
    while (1) {
        signal(SIGINT, sighandler);
        from_client = server_handshake( &to_client );
        close(from_client);
        close(to_client);
    }
    
}
