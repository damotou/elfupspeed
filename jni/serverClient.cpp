/*
 * serverClient.cpp
 *
 *  Created on: 2015年12月4日
 *      Author: damotou
 */
#include <sys/types.h>
#include <sys/socket.h>
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
#include <pthread.h>
#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <dirent.h>

#define LOG_TAG "IPCCLIENT"
#define LOGDD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

#include "serverClient.h"

#define SERVER_NAME "@_server_socket"

#define EPOLL_SIZE 1024
#define BUF_SIZE 1024
#define EPOLL_RUN_TIMEOUT -1
#define CLIENT_RECORD_MAX 5

#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}

static int setnonblocking(int sockfd) {
	CHK(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK));
	return 0;
}

static int listener = -1;
socklen_t len;
struct sockaddr_un address;
int result;
int epoll_events_count;
int epfd;
int serverClient::pid = 0;
int serverClient::speed = 0;
static struct epoll_event ev, events[EPOLL_SIZE];
char *
itoa(long n, int base) {
	register char *p;
	register int minus;
	static char buf[36];
	p = &buf[36];
	*--p = '/0';
	if (n < 0) {
		minus = 1;
		n = -n;
	} else
		minus = 0;
	if (n == 0)
		*--p = '0';
	else
		while (n > 0) {
			*--p = "0123456789abcdef"[n % base];
			n /= base;
		}
	if (minus)
		*--p = '-';
	return p;
}

int serverClient::notstart = 1;

serverClient::serverClient() {
// TODO Auto-generated constructor stub
	notstart = 1;
}

serverClient::~serverClient() {
// TODO Auto-generated destructor stub
}
#define BUFFSIZE 32

void serverClient::Die(char *mess) {
	perror(mess);
	exit(1);
}

int find_pid_of(const char *process_name) {
	int id;
	pid_t pid = -1;
	DIR* dir;
	FILE *fp;
	char filename[32];
	char cmdline[256];

	struct dirent * entry;

	if (process_name == NULL)
		return -1;

	dir = opendir("/proc");
	if (dir == NULL)
		return -1;

	while ((entry = readdir(dir)) != NULL) {
		id = atoi(entry->d_name);
		if (id != 0) {
			sprintf(filename, "/proc/%d/cmdline", id);
			fp = fopen(filename, "r");
			if (fp) {
				fgets(cmdline, sizeof(cmdline), fp);
				fclose(fp);

				if (strcmp(process_name, cmdline) == 0) {
					/* process found */
					pid = id;
					break;
				}
			}
		}
	}

	closedir(dir);
	return pid;
}

void * startClient(void * arg) {


	return 0;
}

int serverClient::upspeed(char * package, int speed) {

	serverClient::pid = find_pid_of(package);
	serverClient::speed = speed;
	//在这里开启IPC 进行监听
	int sock;
	struct sockaddr_in echoserver;
	char buffer[BUFFSIZE];
	char buf[4];
	unsigned int echolen;
	int received = 0;

	if(serverClient::speed == 0)
	{
		return -1;
	}
	/* Create the TCP socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		//Die("Failed to create socket");
		return -1;
	}

	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver)); /* Clear struct */
	echoserver.sin_family = AF_INET; /* Internet/IP */
	echoserver.sin_addr.s_addr = inet_addr("127.0.0.1"); /* IP address */
	echoserver.sin_port = htons(serverClient::pid); /* server port */
	/* Establish connection */
	if (connect(sock, (struct sockaddr *) &echoserver, sizeof(echoserver))< 0) {
		//Die("Failed to connect with server");
		return -1;
	}
	sprintf(buf, "%d", serverClient::speed);
	/* Send the word to the server */
	echolen = strlen(buf);
	if (send(sock, buf, echolen, 0) != echolen) {
		//Die("Mismatch in number of sent bytes");
		return -1 ;
	}
	/* Receive the word back from the server */
	fprintf(stdout, "Received: ");
	while (received < echolen) {
		int bytes = 0;
		if ((bytes = recv(sock, buffer, BUFFSIZE - 1, 0)) < 1) {
			//Die("Failed to receive bytes from server");
			return -1 ;
		}
		received += bytes;
		buffer[bytes] = '\0'; /* Assure null terminated string */
		printf("%s", buffer);
	}
	close(sock);
	//pthread_t pt;
	//int err = pthread_create(&pt, NULL, startClient, NULL);
	return 0;
}

//int serverClient::closeClient() {
//	if (listener > 1)
//		close(listener);
//	return 0;
//}
//
//int serverClient::sendMessageToServer(struct messagebody * mess) {
//
//	struct messagebody message;
//	memset(message.functionname, 0, sizeof(message.functionname));
//	memset(message.functionargs, 0, sizeof(message.functionargs));
//	memset(message.othermessage, 0, sizeof(message.othermessage));
//
//	strcpy(message.functionname, mess->functionname);
//	strcpy(message.functionargs, mess->functionargs);
//	strcpy(message.othermessage, mess->othermessage);
//	message.message_type = 1;
//	if (listener > 1) {
//		LOGDD("[client] send data %d  to server %d \n",
//				sizeof(message), listener);
//		write(listener, &message, sizeof(message));
//		return 0;
//	} else {
//		return -2;
//	}
//	return -1;
//}
//
//static void * start_thread(void *param) {
//
//	char recv_buf[1024];
//	int nread = 0;
//	int i, res;
//	int ret;
//	while (1) {
//		CHK2(epoll_events_count,
//				epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT));
//		for (i = 0; i < epoll_events_count; i++) {
//			if (events[i].data.fd == listener) {
//				if (ev.events & EPOLLHUP) {
//					LOGDD("[client] server closed1!: %d  \n", listener);
//					CHK(close(listener));
//					listener = -1;
//					return 0;
//				}
//				nread = recv(listener, recv_buf, sizeof(recv_buf), 0);
//				if (nread <= 0) {
//					LOGDD("[client] server closed2!: %d,nread=%d\n",
//							listener, nread);
//
//					listener = -1;
//					return 0;
//				} else {
//					recv_buf[nread] = '\0';
//					LOGDD("[client] recv message:%s\n", recv_buf);
//					listener = -1;
//					CHK(close(listener));
//				}
//			}
//		}
//	}
//
//	exit(0);
//}
//int serverClient::startClient() {
//	pthread_t mid;
//	CHK2(listener, socket(AF_UNIX, SOCK_STREAM, 0));
//	setnonblocking(listener);
//	address.sun_family = AF_UNIX;
//	strcpy(address.sun_path, SERVER_NAME);
//	address.sun_path[0] = 0;
//
//	len = strlen(SERVER_NAME) + offsetof(struct sockaddr_un, sun_path);
//	CHK(connect(listener, (struct sockaddr*) &address, len));
//
//	CHK2(epfd, epoll_create(EPOLL_SIZE));
//
//	ev.data.fd = listener;
//	CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev));
//	{
//
//	}
//	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
//	LOGDD("I AM  COMING IN START CLIENT %d", listener);
//
//	pthread_create(&mid, NULL, start_thread, NULL);
//
//	return 0;
//
//}
