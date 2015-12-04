/*
 * serverClient.h
 *
 *  Created on: 2015年12月4日
 *      Author: damotou
 */

#ifndef SERVERCLIENT_H_
#define SERVERCLIENT_H_

class serverClient {
public:
	serverClient();
	virtual ~serverClient();
	int upspeed(char * package ,int speed);
	static int notstart ;
	void Die(char *mess);
	static int pid;
	static int speed;
};

#endif /* SERVERCLIENT_H_ */
