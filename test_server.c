#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SUCCESS 0

#define ERR_SOCK -1

#define FD_UNINIT_C -1

#define NUM_CONN_BEFORE_CLOSE 3
#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 2023

void die_with_error(char *die_with_error_msg)
{
    printf("Error: %s\n", die_with_error_msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int                listen_fd         = FD_UNINIT_C;
    int                client_fd         = FD_UNINIT_C;
    int                sock_so_reuseaddr = 1;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t          client_addr_size;
    char               lcv;

    printf("This dummy test server will accept and close a connection %d times\n",
           NUM_CONN_BEFORE_CLOSE);
    printf("Test with:\n");
    printf("\t $ telnet %s %d\n\n", SERVER_IP, SERVER_PORT);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == ERR_SOCK)
    {
        die_with_error("Could not allocate socket");
    }

    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &sock_so_reuseaddr,
                   sizeof(sock_so_reuseaddr)) == ERR_SOCK)
    {
        die_with_error("Could not set SO_REUSEADDR option");
    }

    printf("We have a server socket ready\n");

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == ERR_SOCK)
    {
        die_with_error("Could not bind server socket");
    }

    printf("Server socket bound to:#%x #%x\n",
           server_addr.sin_addr.s_addr,
           server_addr.sin_port);

    if (listen(listen_fd, 1) == ERR_SOCK)
    {
        die_with_error("Could not listen");
    }

    printf("Listening...\n");

    lcv = 0;

    while(lcv < NUM_CONN_BEFORE_CLOSE)
    {
        client_addr_size = sizeof(client_addr);

        memset(&client_addr, 0, client_addr_size);

        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr,
                           &client_addr_size);

        if (client_fd == ERR_SOCK)
        {
            die_with_error("Error accepting connection");
        }

        printf("Client connected\n");

        sleep(5); /* sleep for 5 seconds */

        close(client_fd);

        printf("Intentionally closed client\n");

        lcv++;
    }

    close(listen_fd);

    printf("Closed server\n\n");

    return SUCCESS;
}
