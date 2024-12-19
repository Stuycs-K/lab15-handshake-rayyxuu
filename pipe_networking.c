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
  char buffer[200];
  from_client = server_setup();
  int b = read(from_client, buffer, sizeof(buffer));
  if (b <= 0) {
    printf("Parent did not receive response\n");
    return -1;
  }
  if (mkfifo(buffer, 0666) < 0) {
    printf("mkfifo failed\n");
    printerror();
    return -1;
  }
  int fd = open("/dev/urandom", O_RDWR);
  int seed;
  read(fd, &seed, sizeof(seed));
  close(fd);
  srand(seed);
  int randnum = rand();
  int fifo = open(buffer, RDWR);
  if (fifo == -1) {
    printerror();
    return -1;
  }
  write(fifo, randnum, sizeof(randnum));
  close(fifo);
  sscanf(buffer, "%d", to_client);
  //INCOMPLETE
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
  char buffer[200] = "/tmp/verysecretfifo";
  char bufferread[200];
  int numinco;
  buffer[strlen(buffer)] = '\0';
  from_client = server_setup();
  write(from_client, buffer, strlen(buffer)+1);
  if (mkfifo(buffer, 0666) < 0) {
    printf("mkfifo failed\n");
    printerror();
    return -1;
  }
  int fifo = open(buffer, O_RDWR);
  if (fifo == -1) {
    printerror();
    return -1;
  }
  remove(buffer);
  int b = read(fifo, bufferread, sizeof(bufferread));
  sscanf(bufferread, "%d", &numinco);
  return from_server;
}


