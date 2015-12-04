/*
 * LocalSocketClient.cpp
 *
 *  Created on: 2015-7-21
 *      Author: HelloKitty
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

#define LOG_TAG "IPCSERVER"

#define LOGDD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

elfIPCServer::elfIPCServer() {
	// TODO Auto-generated constructor stub

}

elfIPCServer::~elfIPCServer() {
	// TODO Auto-generated destructor stub
	delete elfspeed;
}

#define MAXPENDING 1024    /* Max connection requests */
#define BUFFSIZE 32

void Die(char *mess) {
	perror(mess);
	exit(1);
}
void elfIPCServer::HandleClient(int sock) {

	char buffer[BUFFSIZE];
	int received = -1;
	/* Receive message */
	if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
		Die("Failed to receive initial bytes from client");
	}
	/* Send bytes and check for more incoming data in loop */
	while (received > 0) {
		/* Send back received data */
		elfspeed->SetTimeScale(atoi(buffer));
		if (send(sock, buffer, received, 0) != received) {
			Die("Failed to send bytes to client");
		}
		/* Check for more data */
		if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
			Die("Failed to receive additional bytes from client");
		}
	}
	close(sock);
}
int elfIPCServer::StartIPCServer() {

//	int listener, client_sockfd;
//	socklen_t server_len, client_len;
//
//	struct sockaddr_un server_addr;
//
     elfspeed = new ElfSpeed();
//
//	struct sockaddr_un client_addr;
//	static struct epoll_event ev, events[EPOLL_SIZE];
//	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
//	char message[BUF_SIZE];
//	int epfd;
//	clock_t tStart;
//
//	int client, res, epoll_events_count;
//
//	CHK2(listener, socket(AF_UNIX, SOCK_STREAM, 0));
//	setnonblocking(listener);
//	server_addr.sun_family = AF_UNIX;
//	strcpy(server_addr.sun_path, SERVER_NAME);
//	server_addr.sun_path[0] = 0;
//	server_len = strlen(SERVER_NAME) + offsetof(struct sockaddr_un, sun_path);
//	CHK(bind(listener, (struct sockaddr *)&server_addr, server_len));
//	CHK(listen(listener, 5));
//
//	CHK2(epfd, epoll_create(EPOLL_SIZE));
//	ev.data.fd = listener;
//	CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev));
//	//elfspeed->SetTimeScale(1);
//
//	LOGDD("start server ! \n");
//
//	while (1) {
//		int i;
//		LOGDD("wait client ! \n");
//		CHK2(epoll_events_count,epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT));
//
//		tStart = clock();
//		for (i = 0; i < epoll_events_count; i++) {
//			LOGDD(" event comming from client ! \n");
//			if (events[i].data.fd == listener) {
//				LOGDD(" client comming  ! \n");
//				CHK2(client,accept(listener, (struct sockaddr *) &client_addr, &client_len));
//
//				//setnonblocking(client);
//				//ev.data.fd = client;
//
//				//CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev));
//
//			} else {
//
//				LOGDD(" message comming ! \n");
//				//CHK2(res, handle_message(events[i].data.fd,&events[i]));
//			}
//		}
//		LOGDD("Statistics: %d events handled at: %.2f second(s)\n",epoll_events_count, (double) (clock() - tStart) / CLOCKS_PER_SEC);
//	}
	int serversock, clientsock;
	struct sockaddr_in echoserver, echoclient;

	/* Create the TCP socket */
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}
	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver)); /* Clear struct */
	echoserver.sin_family = AF_INET; /* Internet/IP */
	echoserver.sin_addr.s_addr = htonl(INADDR_ANY); /* Incoming addr */
	echoserver.sin_port = htons(getpid()); /* server port */

	/* Bind the server socket */
	if (bind(serversock, (struct sockaddr *) &echoserver, sizeof(echoserver))
			< 0) {
		Die("Failed to bind the server socket");
	}
	/* Listen on the server socket */
	if (listen(serversock, MAXPENDING) < 0) {
		Die("Failed to listen on server socket");
	}
	/* Run until cancelled */
	LOGDD("server with fd: %d closed! \n", serversock);
	while (1) {
		unsigned int clientlen = sizeof(echoclient);
		/* Wait for client connection */
		if ((clientsock = accept(serversock, (struct sockaddr *) &echoclient,(socklen_t *)&clientlen)) < 0) {
			Die("Failed to accept client connection");
		}
		fprintf(stdout, "Client connected: %s\n",
				inet_ntoa(echoclient.sin_addr));
		HandleClient(clientsock);
	}
}

//int elfIPCServer::setnonblocking(int sockfd) {
//	CHK(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK));
//	return 0;
//}

// *** Handle incoming message from clients

//int elfIPCServer::handle_message(int client, struct epoll_event *ev) {

//	char recv_buf[BUF_SIZE], message[BUF_SIZE];
//	int len;
//
//	bzero(recv_buf, BUF_SIZE);
//	bzero(message, BUF_SIZE);
//
//	if (ev->events & EPOLLERR || ev->events & EPOLLHUP) {
//
//		printf("Client with fd: %d closed! \n", client);
//		CHK(close(client));
//		return 0;
//	}
//#ifdef DEBUG_MODE
//	LOGDD("Try to read from fd(%d)\n", client);
//#endif
//
//	CHK2(len, recv(client, recv_buf, sizeof(struct messagebody), 0));
//
//	if (len == 0) {
//		CHK(close(client));
//#ifdef DEBUG_MODE
//		LOGDD("Client with fd: %d closed! \n", client);
//#endif
//	} else {
//
//		LOGDD("message:%d\n", len);
//
//		if (len == sizeof(struct messagebody)) {
//			struct messagebody mess;
//			memset(&mess, 0, sizeof(mess));
//			memcpy(&mess, recv_buf, sizeof(mess));
//
//			//std::string data = recv_buf;
//
//			LOGDD("functionname:%s\n", mess.functionname);
//			LOGDD("functionargs:%s\n", mess.functionargs);
//			LOGDD("othermessage:%s\n", mess.othermessage);
//
//			switch (mess.message_type) {
//			case 1: //function call
//				if (strcmp(mess.functionname, "SetTimeScale") == 0) {
//					//elfspeed->SetTimeScale(atoi(mess.functionargs));
//				}
//				break;
//			}
//			return 0;
//		} else {
//			return 0;
//		}
//	}
//	return 0;
//}

