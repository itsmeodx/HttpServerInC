#include "Http.h"

int startServer(const char *name, const char *port, int backlog, int *serverFd)
{
	// Initialize address information
	struct addrinfo hints, *servAddr;

	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET;		 // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP
	hints.ai_flags = AI_PASSIVE;	 // For wildcard IP address

	// Get address information for the server
	int status = getaddrinfo(name, port, &hints, &servAddr);
	if (status != 0)
	{
		fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(status));
		return (EXIT_FAILURE);
	}

	// Create a socket
	*serverFd = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
	if (*serverFd == -1)
	{
		perror("server: socket");
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Set socket options to allow reuse of the address
	int optval = 1;
	if (setsockopt(*serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("server: setsockopt");
		close(*serverFd);
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Bind the socket to the address
	if (bind(*serverFd, servAddr->ai_addr, servAddr->ai_addrlen) == -1)
	{
		perror("server: bind");
		close(*serverFd);
		freeaddrinfo(servAddr);
		return (EXIT_FAILURE);
	}

	// Free the address information structure
	freeaddrinfo(servAddr);

	// Listen for incoming connections
	if (listen(*serverFd, backlog) == -1)
	{
		perror("server: listen");
		close(*serverFd);
		return (EXIT_FAILURE);
	}

	printf("server: running on port %s\n", PORT);
	printf("server: waiting for incoming connections...\n");
	return (EXIT_SUCCESS);
}

int handleClient(int clientFd)
{
	struct HttpResponse *resp = NULL;
	struct HttpRequest *req = NULL;
	char requestBuffer[4096];
	int status = EXIT_SUCCESS;
	bool keepAlive = true;

	// Keep connection alive until client requests to close
	while (keepAlive)
	{
		// Clear buffer for next request
		memset(requestBuffer, 0, sizeof(requestBuffer));

		// Receive HTTP request from the client
		int bytesReceived = recv(clientFd, requestBuffer, sizeof(requestBuffer) - 1, 0);
		if (bytesReceived <= 0)
		{
			if (bytesReceived == 0)
				printf("server: client closed connection\n");
			else
			{
				perror("server: recv");
				status = EXIT_FAILURE;
			}
			break; // Exit loop on connection close or error
		}
		requestBuffer[bytesReceived] = '\0';

		printf("server: received HTTP request:\n%s\n", requestBuffer);
		printf("server: processing request\n");

		// Parse HTTP request
		req = parseHttpRequest(requestBuffer);
		if (!req || req->method == HTTP_UNKNOWN)
		{
			fprintf(stderr, "server: received empty or invalid request\n");
			resp = calloc(1, sizeof(struct HttpResponse));
			resp->statusCode = HTTP_BAD_REQUEST;
			resp->statusMessage = strdup("Bad Request");
			resp->contentType = strdup("text/plain");
			resp->contentLength = 12;
			resp->body = strdup("Bad Request\n");
		}
		else if (req->method == HTTP_GET)
			resp = httpGetMethod(req);
		else if (req->method == HTTP_POST)
			resp = httpPostMethod(req);
		else {
			resp = calloc(1, sizeof(struct HttpResponse));
			resp->statusCode = HTTP_NOT_FOUND;
			resp->statusMessage = strdup("Not Found");
			resp->contentType = strdup("text/plain");
			resp->contentLength = 10;
			resp->body = strdup("Not Found\n");
		}

		// Check if client wants to close connection
		if (req)
		{
			const char *connectionHeader = getHeader("Connection", req);
			if (connectionHeader && strcasecmp(connectionHeader, "close") == 0)
			{
				keepAlive = false;
				printf("server: client requested connection close\n");
			}
		}

		// Send HTTP response
		if (sendHttpResponse(clientFd, resp, keepAlive) < 0)
		{
			perror("server: sendHttpResponse");
			status = EXIT_FAILURE;
			break;
		}

		printf("server: response sent, connection %s\n", keepAlive ? "kept alive" : "will close");

		// Free allocated memory for this request
		freeHttpRequest(req);
		freeHttpResponse(resp);
		req = NULL;
		resp = NULL;
	}

	// Clean up any remaining allocated memory
	if (req) freeHttpRequest(req);
	if (resp) freeHttpResponse(resp);
	close(clientFd);
	printf("server: connection closed\n");
	if (status == EXIT_FAILURE)
		fprintf(stderr, "server: failed to handle client request\n");
	return (status);
}
