#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define PORT "4221" // Port number for the server
#define BACKLOG 10 // Number of pending connections queue

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	struct addrinfo hints, *servAddr;

	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE; // For wildcard IP address

	// Get address information for the server
	int status = getaddrinfo(NULL, PORT, &hints, &servAddr);
	if (status != 0) {
		fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(status));
		return (EXIT_FAILURE);
	}

	// Create a socket
	int sockfd = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
	if (sockfd == -1) {
		perror("server: socket");
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Set socket options to allow reuse of the address
	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		perror("server: setsockopt");
		close(sockfd);
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Bind the socket to the address
	if (bind(sockfd, servAddr->ai_addr, servAddr->ai_addrlen) == -1) {
		perror("server: bind");
		close(sockfd);
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Free the address information structure
	freeaddrinfo(servAddr);

	// Listen for incoming connections
	if (listen(sockfd, BACKLOG) == -1) {
		perror("server: listen");
		close(sockfd);
		return (EXIT_FAILURE);
	}

	printf("server: waiting for connections on port %s...\n", PORT);

	// Accept incoming connections
	struct sockaddr_storage clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSockfd == -1) {
		perror("server: accept");
		close(sockfd);
		return (EXIT_FAILURE);
	}

	printf("server: connection accepted\n");

	// Send HTTP response
	const char *httpResponse = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 14\r\n"
		"\r\n"
		"Hello, world!\n";
	send(clientSockfd, httpResponse, strlen(httpResponse), 0);

	close(clientSockfd);
	close(sockfd);
	return 0;
}
