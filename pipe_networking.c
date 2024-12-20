#include "pipe_networking.h"
#include <fcntl.h>  
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdio.h>  
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

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
  int from_client = 0;
  char* pipe_name = WKP;
  if (mkfifo(pipe_name, 0666) == -1) {
    printf("WKP creation failed\n");
    printerror();
    return -1;  
  }
  printf("WKP created, waiting for a connection\n");
  from_client = open(pipe_name, O_RDONLY);
  if (from_client == -1) {
    printf("WKP opening failed\n");
    printerror();
    return -1;  
  }
  printf("Client connected, removing WKP\n");
  if (unlink(pipe_name) == -1) {
      printf("WKP removal failed\n");
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
  char buffer[200];
  char bufferrec[200];
  from_client = server_setup();
  printf("Server reading SYN\n");
  int b = read(from_client, buffer, sizeof(buffer));
  if (b <= 0) {
    printf("Server did not receive response\n");
    return -1;
  }
  if (mkfifo(buffer, 0666) < 0) {
    printf("Server mkfifo failed\n");
    printerror();
    return -1;
  }
  int fd = open("/dev/urandom", O_RDONLY);
  int seed;
  read(fd, &seed, sizeof(seed));
  close(fd);
  srand(seed);
  int randnum = rand();
  printf("Server opening PP\n");
  int fifo = open(buffer, O_RDWR);
  if (fifo == -1) {
    printerror();
    return -1;
  }
  printf("Server sending random number SYN_ACK\n");
  write(fifo, randnum, sizeof(randnum));
  sscanf(buffer, "%d", to_client);
  printf("Server reading ACK\n");
  int a = read(fifo, bufferrec, sizeof(bufferrec));
  if (a <= 0) {
    printf("Server did not receive ACK\n");
    return -1;
  }
  int numrec;
  sscanf(bufferrec, "%d", &numrec);
  if (numrec==randnum+1) {
    printf("Server received ACK, handshake complete\n");
    close(fifo);
    return from_client;
  }
  else {
    printf("Server did not receive correct ACK, failed\n");
    close(fifo);
    return -1;
  }
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
  printf("Client making private pipe PP\n");
  char buffer[200] = "/tmp/verysecretfifo";
  char bufferread[200];
  int numinco;
  buffer[strlen(buffer)] = '\0';
  printf("Client opening WKP\n");
  from_server = server_setup();
  printf("Client writing PP to WKP\n");
  write(from_server, buffer, strlen(buffer)+1);
  if (mkfifo(buffer, 0666) < 0) {
    printf("Client mkfifo failed\n");
    printerror();
    return -1;
  }
  printf("Client opening PP\n");
  int fifo = open(buffer, O_RDWR);
  if (fifo == -1) {
    printerror();
    return -1;
  }
  printf("Client deleting PP\n");
  unlink(buffer);
  printf("Client reading SYN_ACK\n");
  int b = read(fifo, bufferread, sizeof(bufferread));
  if (b <= 0) {
    printf("Client did not receive response\n");
    return -1;
  }
  sscanf(bufferread, "%d", &numinco);
  printf("Client sending ACK\n");
  write(fifo, numinco+1, sizeof(numinco+1));
  return from_server;
}


