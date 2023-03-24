test_server.o: test_server.c
	gcc -g -Wall -c test_server.c -o test_server.o

test_server: test_server.o
	gcc -g test_server.o -o test_server
