#include "pipe_networking.h"
#include <fcntl.h>  
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdio.h>  
#include <errno.h>

int printerror(){
    printf("error number %d\n",errno);
    printf("error: %s\n", strerror(errno));
    return 0;
}

//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server
/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  char* pipe_name = WKP;
  int from_client = 0;
  if (mkfifo(pipe_name, 0666) == -1) {
    printf("WKP creation failed");
    printerror();
    return -1;  
  }
  printf("WKP created, waiting for a connection\n");
  from_client = open(pipe_name, O_RDONLY);
  if (from_client == -1) {
    printf("WKP opening failed");
    printerror();
    return -1;  
  }
  printf("Client connected, removing WKP\n");
  if (unlink(pipe_name) == -1) {
      printf("WKP removal failed");
      printerror();
  }
  return from_client;
}

/*=========================
  server_handshake 
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

  returns the file descriptor for the upstream pipe (see server setup).
  =========================*/
int server_handshake(int *to_client) {
  int from_client;
  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int from_server;
  return from_server;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  int to_client  = 0;
  return to_client;
}


