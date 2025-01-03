#include "pipe_networking.h"

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

    int to_server;
    int from_server;

    from_server = client_handshake( &to_server );
    sleep(2);
    signal(SIGINT, sighandler);
    while (1) {
        char buffer[20];
        int b = read(from_server, buffer, sizeof(buffer));
        if (b < 0) {
            printf("Client read failed.\n");
            break;
        }
        int recnum;
        sscanf(buffer, "%d", &recnum);
        printf("Client received: %d\n", recnum);
    }

}
