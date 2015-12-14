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
	float times = 0;
	/* Receive message */
	if ((received = recv(sock, buffer, BUFFSIZE, 0)) < 0) {
		Die("Failed to receive initial bytes from client");
	}
	/* Send bytes and check for more incoming data in loop */
	while (received > 0) {
		/* Send back received data */
		times = atof(buffer);
		elfspeed->SetTimeScale(times);

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

	elfspeed = new ElfSpeed();
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
	if (bind(serversock, (struct sockaddr *) &echoserver, sizeof(echoserver)) < 0) {
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

