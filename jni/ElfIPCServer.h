/*
 * LocalSocketClient.h
 *
 *  Created on: 2015-7-21
 *      Author: HelloKitty
 */
#include <jni.h>
//#include "ElfTransaction.h"
#include "message.h"
#include "speed.h"

#ifndef LOCALSOCKETCLIENT_H_
#define LOCALSOCKETCLIENT_H_

#define NO_ERR 0
#define CREATE_ERR -1
#define CONNECT_ERR -2
#define LINUX_MAKE_ADDRUN_ERROR -3
#define NO_LINUX_MAKE_ADDRUN_ERROR -4
#define CLOSE_ERR -5
#define DEBUG_MODE 1
#define SERVER_NAME "@_server_socket"
#define EPOLL_SIZE 1024
#define BUF_SIZE  4096
#define EPOLL_RUN_TIMEOUT -1

// Macros - exit in any error (eval < 0) case
#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}
// Macros - same as above, but save the result(res) of expression(eval)
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}


class elfIPCServer {
public:
	elfIPCServer();
	int StartIPCServer();
	//int setnonblocking(int sockfd);
	void HandleClient(int sock);
	virtual ~elfIPCServer();
public:
	int socketid;
private:
	ElfSpeed *  elfspeed;
};

#endif
