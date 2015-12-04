/*
 * local.h
 *
 *  Created on: 2015年12月4日
 *      Author: damotou
 */

#ifndef LOCAL_H_
#define LOCAL_H_


#define BUF_SIZE 1024                 //默认缓冲区
#define SERVER_PORT 44444             //监听端口
#define SERVER_HOST "127.0.0.1"   //服务器IP地址
#define EPOLL_RUN_TIMEOUT -1          //epoll的超时时间
#define EPOLL_SIZE 10000              //epoll监听的客户端的最大数目

#define STR_WELCOME "Welcome to seChat! You ID is: Client #%d"
#define STR_MESSAGE "Client #%d>> %s"
#define STR_NOONE_CONNECTED "Noone connected to server except you!"
#define CMD_EXIT "EXIT"

//两个有用的宏定义：检查和赋值并且检测
#define CHK(eval) if(eval < 0){perror("eval"); exit(-1);}
#define CHK2(res, eval) if((res = eval) < 0){perror("eval"); exit(-1);}

//================================================================================================
//函数名：                  setnonblocking
//函数描述：                设置socket为不阻塞
//输入：                    [in] sockfd socket标示符
//输出：                    无
//返回：                    0
//================================================================================================
int setnonblocking(int sockfd);

//================================================================================================
//函数名：                  handle_message
//函数描述：                处理每个客户端socket
//输入：                    [in] new_fd socket标示符
//输出：                    无
//返回：                    返回从客户端接受的数据的长度
//================================================================================================
int handle_message(int new_fd);


#endif /* LOCAL_H_ */
