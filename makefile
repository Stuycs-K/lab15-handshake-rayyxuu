all: client server

client: persistant_client.o pipe_networking.o
	gcc -o client persistant_client.o pipe_networking.o

server: persistant_server.o pipe_networking.o
	gcc -o server persistant_server.o pipe_networking.o

persistant_client.o: persistant_client.c pipe_networking.h
	gcc -c persistant_client.c

persistant_server.o: persistant_server.c pipe_networking.h
	gcc -c persistant_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

clean:
	rm *.o
	rm *~
