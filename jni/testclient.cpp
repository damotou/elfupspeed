#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFSIZE 32
void Die(char *mess) {
	perror(mess);
	exit(1);
}
int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in echoserver;
	char buffer[BUFFSIZE];
	unsigned int echolen;
	int received = 0;

	/* Create the TCP socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("Failed to create socket");
	}

	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver)); /* Clear struct */
	echoserver.sin_family = AF_INET; /* Internet/IP */
	echoserver.sin_addr.s_addr = inet_addr("127.0.0.1"); /* IP address */
	echoserver.sin_port = htons(atoi(argv[1])); /* server port */
	/* Establish connection */
	if (connect(sock, (struct sockaddr *) &echoserver, sizeof(echoserver))
			< 0) {
		printf("Failed to connect with server");
	}

	/* Send the word to the server */
	echolen = strlen(argv[2]);
	if (send(sock, argv[2], echolen, 0) != echolen) {
		printf("Mismatch in number of sent bytes");
	}
	/* Receive the word back from the server */
	fprintf(stdout, "Received: ");
	while (received < echolen) {
		int bytes = 0;
		if ((bytes = recv(sock, buffer, BUFFSIZE - 1, 0)) < 1) {
			printf("Failed to receive bytes from server");
		}
		received += bytes;
		buffer[bytes] = '\0'; /* Assure null terminated string */
		printf("%s",buffer);
	}

	fprintf(stdout, "\n");
	close(sock);
	exit(0);
}
