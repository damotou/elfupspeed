/*
 * epollserver.cpp
 *
 *  Created on: 2015年12月4日
 *      Author: damotou
 */
#include <stdio.h>
#include <cutils/sockets.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <unistd.h>
#include "ElfIPCServer.h"
#include <sys/types.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string>
#include <cutils/log.h>
#include <list>

#include "epollserver.h"

#include "local.h"


using namespace std;

// 存放客户端socket描述符的list
list<int> clients_list;

int setnonblocking(int sockfd)
{
    CHK(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK));
    return 0;
}

int main(int argc, char *argv[])
{
    int listener;   //监听socket
    struct sockaddr_in addr, their_addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);
    socklen_t socklen;
    socklen = sizeof(struct sockaddr_in);

    static struct epoll_event ev, events[EPOLL_SIZE];
    ev.events = EPOLLIN | EPOLLET;     //对读感兴趣，边沿触发

    char message[BUF_SIZE];

    int epfd;  //epoll描述符
    clock_t tStart;  //计算程序运行时间

    int client, res, epoll_events_count;

    CHK2(listener, socket(PF_INET, SOCK_STREAM, 0));             //初始化监听socket
    setnonblocking(listener);                                    //设置监听socket为不阻塞
    CHK(bind(listener, (struct sockaddr *)&addr, sizeof(addr))); //绑定监听socket
    CHK(listen(listener, 1));                                    //设置监听

    CHK2(epfd,epoll_create(EPOLL_SIZE));                         //创建一个epoll描述符，并将监听socket加入epoll
    ev.data.fd = listener;
    CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev));

    while(1)
    {
        CHK2(epoll_events_count,epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT));
        tStart = clock();
        for(int i = 0; i < epoll_events_count ; i++)
        {
            if(events[i].data.fd == listener)                    //新的连接到来，将连接添加到epoll中，并发送欢迎消息
            {
                CHK2(client,accept(listener, (struct sockaddr *) &their_addr, &socklen));
                setnonblocking(client);
                ev.data.fd = client;
                CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev));

                clients_list.push_back(client);                  // 添加新的客户端到list
                bzero(message, BUF_SIZE);
                res = sprintf(message, STR_WELCOME, client);
                CHK2(res, send(client, message, BUF_SIZE, 0));

            }else
            {
                CHK2(res,handle_message(events[i].data.fd)); //注意：这里并没有调用epoll_ctl重新设置socket的事件类型，但还是可以继续收到客户端发送过来的信息
            }
        }
        printf("Statistics: %d events handled at: %.2f second(s)\n", epoll_events_count, (double)(clock() - tStart)/CLOCKS_PER_SEC);
    }

    close(listener);
    close(epfd);

    return 0;
}

int handle_message(int client)
{
    char buf[BUF_SIZE], message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    int len;

    CHK2(len,recv(client, buf, BUF_SIZE, 0));  //接受客户端信息

    if(len == 0)   //客户端关闭或出错，关闭socket，并从list移除socket
    {
        CHK(close(client));
        clients_list.remove(client);
    }
    else          //向客户端发送信息
    {
        if(clients_list.size() == 1)
        {
            CHK(send(client, STR_NOONE_CONNECTED, strlen(STR_NOONE_CONNECTED), 0));
                return len;
        }

        sprintf(message, STR_MESSAGE, client, buf);
        list<int>::iterator it;
        for(it = clients_list.begin(); it != clients_list.end(); it++)
        {
           if(*it != client)
           {
                CHK(send(*it, message, BUF_SIZE, 0));
           }
        }
    }

    return len;
}
