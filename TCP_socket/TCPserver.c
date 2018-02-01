//filename:TCPserver.c
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define BACKLOG 10
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if(2 != argc)
    {
        printf("Usage:%s port_number\n", argv[0]);
        return -1;
    }

    /***1.create a socket***/
    int fd_server = socket(AF_INET, SOCK_STREAM, 0); //TCP
    if(-1 == fd_server)
    {
        printf("%s\n", strerror(errno));   // errno 用于表示最近一个函数调用是否产生了错误,若为0,则无错误,其它值均表示一类错误.perror()和strerrot()函数可以把errno的值转化为有意义的字符输出。
        return -1;
    }

    /***2.bind the socket***/
    int listen_port = atoi(argv[1]);
    struct sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(listen_port);
    addr_server.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(fd_server, (struct sockaddr *)&addr_server, sizeof(addr_server)) == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

    /***3.listen the socket***/
    if(listen(fd_server, BACKLOG) == -1)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

    /***4.accept the requirement of some client***/
    struct sockaddr_in addr_client;
    int len_addr_client = sizeof(addr_client);
    int fd_client = accept(fd_server, (struct sockaddr *)&addr_client, &len_addr_client);
    if(-1 == fd_client)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }

    /****5.serve the client******/
    char buf[BUFFER_SIZE];
    int size;
    while(1)
    {
        /***read from client***/
        size = recv(fd_client, buf, sizeof(buf), 0);
        buf[size] = '\0';
        printf("%s\n", buf);

        /***write to client***/
        size = send(fd_client, buf, strlen(buf), 0);
    }

    /****6.close the socket******/
    close(fd_server);
    close(fd_client);
}
