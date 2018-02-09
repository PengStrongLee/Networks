#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <event.h>

using namespace std;

#define BUF_SIZE 1024

// connect to the server
int connect_server(char* ip, int port)
{
    // 1.create a socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == fd)
    {
        cout << "Error connect server() quit ! " << endl;
        return -1;
    }

    // 2.connect to the server
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(ip);
    remote_addr.sin_port = htons(port);
    if(connect(fd, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr)) < 0)
    {

        cout << "connect Error ! " << endl;
        close(fd);
        return -1;
    }

    return fd;
}

// read data if the socket can be read
void on_read(int sock, short event, void* arg)
{
    char* buff = new char[BUF_SIZE];
    memset(buff, 0, sizeof(char)*BUF_SIZE);
    int size = read(sock, buff, BUF_SIZE);
    if (0 == size)
    {

        cout << "read size 0 for socket" << endl;
        return;
    }

    cout << "Receive from server" << endl;
    delete[] buff;
}

// 在子线程中调用 init_read_event() 来将socket的读事件注册到libevent的上.并开启 event_base_dispatch 进行不断地轮询.
// 一旦 socket 可读,libevent就调用"读事件"上绑定的on_read()函数来读取数据.
void* init_read_event(void* arg)
{
    long long_sock = (long)arg;
    int sock = (int)long_sock;
    // init the lib_event, set the callback function on_read-----;
    // 添加监听服务器消息事件
    struct event_base* base = event_base_new();
    struct event* read_ev = (struct event*)malloc(sizeof(struct event)); // 发生读事件后从socket中取出数据.

    event_set(read_ev, sock, EV_READ | EV_PERSIST, on_read, NULL);
    event_base_set(base, read_ev);
    event_add(read_ev, NULL);
    // 进入循环
    event_base_dispatch(base);

    event_del(read_ev);
    free(read_ev);
    event_base_free(base);
}

// x创建一个新线程,在新的线程中初始化 libevent 读事件的相关设置,
void init_read_event_thread(int sock)
{
    pthread_t thread;
    pthread_create(&thread, NULL, init_read_event, (void*)sock);
    pthread_detach(thread);
}


int main()
{
    cout << "client main started ! " << endl;
    cout << "please input IP : " << endl;
    char ip[16];
    cin >> ip;
    cout << "please input port : " << endl;
    int port;
    cin >> port;

    // socket setting
    int socket_fd = connect_server(ip, port);
    cout << "socket_fd = " << socket_fd << endl;

    init_read_event_thread(socket_fd);

    char buffer[BUF_SIZE];
    bool isBreak = false;

    // 循环用来接收用户从终端的输入,并通过 socket 将用户的输入写到server端.
    while(!isBreak)
    {
        cout << "Input your data to server(\'q\'or\"quit\"to exit)" << endl;
        cin >> buffer;
        if(strcmp("q", buffer) == 0 || strcmp("quit", buffer) == 0)   // strcmp 按照ASCII码顺序比较两个数组中的字符串，并由函数返回值返回比较结果。字符串1＝字符串2，返回值＝0；
        {
            isBreak = true;
            close(socket_fd);
            break;
        }
        cout << "your input is : " << buffer << endl;
        int write_num = write(socket_fd, buffer, strlen(buffer));
        cout << write_num << " characters written" << endl;
        sleep(2);
    }

    cout << "main finished ! " << endl;
    return 0;
}

