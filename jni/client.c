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
#include "message.h"
#include <string.h>
#include <android/log.h>

#define LOG_TAG "IPCCLIENT"
#define LOGDD(fmt, args...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)

jstring Java_com_damotou_cyjhdev_hookapplication_util_RootUtil_sendMessageToServer(
		JNIEnv* env, jobject thiz);
jstring Java_com_damotou_cyjhdev_hookapplication_util_RootUtil_startClient(
		JNIEnv* env, jobject thiz);

#define SERVER_NAME "@speed_server_socket"
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

static struct epoll_event ev, events[EPOLL_SIZE];

JNIEXPORT jstring JNICALL Java_com_damotou_cyjhdev_hookapplication_util_RootUtil_closeClient(
		JNIEnv* env, jobject thiz) {
	if (listener > 1)
		close(listener);

	return (*env)->NewStringUTF(env, "closed");
}

JNIEXPORT jstring JNICALL Java_com_damotou_cyjhdev_hookapplication_util_RootUtil_sendMessageToServer(
		JNIEnv* env, jobject thiz) {
	LOGDD(
			"[client] Java_com_damotou_cyjhdev_hookapplication_util_RootUtil_sendMessageToServer %d \n",
			listener);

	struct messagebody mess;
	memset(mess.functionname, 0, sizeof(mess.functionname));
	memset(mess.functionargs, 0, sizeof(mess.functionargs));
	memset(mess.othermessage, 0, sizeof(mess.othermessage));

	strcpy(mess.functionname, "getRunningAPP");
	strcpy(mess.functionargs, "client send");
	strcpy(mess.othermessage, "hhh");
	mess.message_type = 1;
	if (listener > 1) {
		LOGDD("[client] send data  to server %d \n", listener);
		write(listener, &mess, sizeof(mess));
		return (*env)->NewStringUTF(env, "OK");
	} else {
		return (*env)->NewStringUTF(env, "FAIL");
	}
	return (*env)->NewStringUTF(env, "FAIL");
}

static void * start_thread(void *param) {
	char recv_buf[1024];
	int nread = 0;
	int i, res;
	int ret;
	while (1) {
		CHK2(epoll_events_count,
				epoll_wait(epfd, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT));
		for (i = 0; i < epoll_events_count; i++) {
			if (events[i].data.fd == listener) {
				if (ev.events & EPOLLHUP) {
					LOGDD("[client] server closed1!: %d  \n", listener);
					CHK(close(listener));
					listener = -1;
					return 0;
				}
				nread = recv(listener, recv_buf, sizeof(recv_buf), 0);
				if (nread <= 0) {
					LOGDD("[client] server closed2!: %d,nread=%d\n",
							listener, nread);

					listener = -1;
					return 0;
				} else {
					recv_buf[nread] = '\0';
					LOGDD("[client] recv message:%s\n", recv_buf);
					listener = -1;
					CHK(close(listener));
				}
			}
		}
	}

	exit(0);
}

JNIEXPORT jstring JNICALL Java_com_damotou_cyjhdev_hookapplication_util_RootUtil_startClient(
		JNIEnv* env, jobject thiz) {
	pthread_t mid;

	CHK2(listener, socket(AF_UNIX, SOCK_STREAM, 0));
	setnonblocking(listener);
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, SERVER_NAME);
	address.sun_path[0] = 0;

	len = strlen(SERVER_NAME) + offsetof(struct sockaddr_un, sun_path);
	CHK(connect(listener, (struct sockaddr*)&address, len));

	CHK2(epfd, epoll_create(EPOLL_SIZE));

	ev.data.fd = listener;
	CHK(epoll_ctl(epfd, EPOLL_CTL_ADD, listener, &ev));
	{

	}
	ev.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
	LOGDD("I AM  COMING IN START CLIENT %d", listener);

	pthread_create(&mid, NULL, start_thread, NULL);

	return (*env)->NewStringUTF(env, "dddd");
}

