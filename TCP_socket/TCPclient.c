//filename:TCPclient.c
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if(3 != argc)
    {
        printf("Usage:%s host_name port_number\n", argv[0]);
        return -1;
    }

    /***1.create a socket***/
    int fd_client_socket = socket(AF_INET, SOCK_STREAM, 0); //TCP
    if(-1 == fd_client_socket)
    {
        printf("%d\n", strerror(errno));
        return -1;
    }

    /***2.connect to the server***/
    int portnumber = atoi(argv[2]);
    struct sockaddr_in addr_server;
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(portnumber);
    if(0 == inet_pton(AF_INET, argv[1], (void *)&addr_server.sin_addr.s_addr))
    {
        printf("Invalid address.\n");
        return -1;
    }
    if(connect(fd_client_socket, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1)
    {
        printf("%d\n", strerror(errno));
        return -1;
    }


    /****3.get the server******/
    char buf[BUFFER_SIZE];
    int size;
    while(1)
    {
        /***write to server***/
        scanf("%s", buf);
        size = send(fd_client_socket, buf, strlen(buf), 0);

        /***read from server***/
        size = recv(fd_client_socket, buf, BUFFER_SIZE, 0);

        buf[size] = '\0';
        // printf("%s\n", buf);
    }

    /****4.close the socket******/
    close(fd_client_socket);
}
