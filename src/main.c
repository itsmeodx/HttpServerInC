#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#define PORT "4221" // Port number for the server
#define BACKLOG 10	// Number of pending connections queue

bool handleClient(int clientFd)
{
	// Receive HTTP request from the client
	char requestBuffer[4096];
	if (recv(clientFd, requestBuffer, sizeof(requestBuffer) - 1, 0) < 0)
	{
		perror("server: recv");
		close(clientFd);
		return (EXIT_FAILURE);
	}
	requestBuffer[sizeof(requestBuffer) - 1] = '\0'; // Null-terminate the string

	printf("server: received HTTP request:\n%s\n", requestBuffer);
	printf("server: processing request\n");

	// Prepare HTTP response
	const char *httpResponse;

	// Check if the request is empty
	if (strlen(requestBuffer) == 0)
	{
		fprintf(stderr, "server: received empty request\n");
		httpResponse = "HTTP/1.1 400 Bad Request\r\n"
					   "Content-Type: text/plain\r\n"
					   "Content-Length: 15\r\n"
					   "\r\n"
					   "Bad Request\n";
	}
	else if (strstr(requestBuffer, "GET / ") != NULL)
	{
		httpResponse = "HTTP/1.1 200 OK\r\n"
					   "Content-Type: text/plain\r\n"
					   "Content-Length: 14\r\n"
					   "\r\n"
					   "Hello, world!\n";
	}
	else
	{
		httpResponse = "HTTP/1.1 404 Not Found\r\n"
					   "Content-Type: text/plain\r\n"
					   "Content-Length: 10\r\n"
					   "\r\n"
					   "Not Found\n";
	}

	// Send HTTP response
	if (send(clientFd, httpResponse, strlen(httpResponse), 0) == -1)
	{
		perror("server: send");
		close(clientFd);
		return (EXIT_FAILURE);
	}

	printf("server: response sent\n");

	// Close the client socket
	close(clientFd);
}

int main()
{
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// Initialize address information
	struct addrinfo hints, *servAddr;

	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET;		 // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;	 // For wildcard IP address

	// Get address information for the server
	int status = getaddrinfo(NULL, PORT, &hints, &servAddr);
	if (status != 0)
	{
		fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(status));
		return (EXIT_FAILURE);
	}

	// Create a socket
	int serverFd = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
	if (serverFd == -1)
	{
		perror("server: socket");
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Set socket options to allow reuse of the address
	int optval = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("server: setsockopt");
		close(serverFd);
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Bind the socket to the address
	if (bind(serverFd, servAddr->ai_addr, servAddr->ai_addrlen) == -1)
	{
		perror("server: bind");
		close(serverFd);
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Free the address information structure
	freeaddrinfo(servAddr);

	printf("server: running on port %s\n", PORT);

	// Listen for incoming connections
	if (listen(serverFd, BACKLOG) == -1)
	{
		perror("server: listen");
		close(serverFd);
		return (EXIT_FAILURE);
	}

	printf("server: waiting for connections...\n", PORT);

	// Accept incoming connections
	struct sockaddr_storage clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (clientFd == -1)
	{
		perror("server: accept");
		close(serverFd);
		return (EXIT_FAILURE);
	}

	printf("server: connection accepted\n");
	handleClient(clientFd);

	// Close the server socket
	close(serverFd);
	return 0;
}
