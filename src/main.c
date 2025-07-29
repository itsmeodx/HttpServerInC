#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <netdb.h>

#define PORT "4221" // Port number for the server
#define BACKLOG 10	// Number of pending connections queue
#define HelloWorldResponse "HTTP/1.1 200 OK\r\n"          \
						   "Content-Type: text/plain\r\n" \
						   "Content-Length: 14\r\n"       \
						   "\r\n"                         \
						   "Hello, world!\n"
#define BadRequestResponse "HTTP/1.1 400 Bad Request\r\n" \
						   "Content-Type: text/plain\r\n" \
						   "Content-Length: 12\r\n"       \
						   "\r\n"                         \
						   "Bad Request\n"
#define NotFoundResponse "HTTP/1.1 404 Not Found\r\n"   \
						 "Content-Type: text/plain\r\n" \
						 "Content-Length: 10\r\n"       \
						 "\r\n"                         \
						 "Not Found\n"

char **splitStr(const char *str, const char *delim)
{
	char **result = NULL;
	size_t count = 0;
	size_t delim_len = strlen(delim);
	const char *start = str;
	const char *found;

	while ((found = strstr(start, delim)) != NULL)
	{
		size_t len = found - start;
		char *token = malloc(len + 1);
		if (!token)
		{
			perror("server: strsplit: malloc");
			goto BAD;
		}
		memcpy(token, start, len);
		token[len] = '\0';
		result = realloc(result, sizeof(char *) * (count + 1));
		if (!result)
		{
			perror("server: strsplit realloc");
			goto BAD;
		}
		result[count++] = token;
		start = found + delim_len;
	}
	// Add the last token
	char *token = strdup(start);
	if (!token)
	{
		perror("server: strsplit strdup");
		goto BAD;
	}
	result = realloc(result, sizeof(char *) * (count + 2));
	if (!result)
	{
		perror("server: strsplit realloc");
		goto BAD;
	}
	result[count++] = token;
	result[count] = NULL; // Null-terminate the array
	return (result);

BAD: // Free previously allocated tokens
	for (size_t i = 0; i < count; i++)
		free(result[i]);
	free(result);
	free(token);
	return (NULL);
}

int installSignal(int signal, void (*handler)(int))
{
	struct sigaction sa;

	sa = (struct sigaction){0};
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if (sigaction(signal, &sa, NULL) == -1)
	{
		perror("server: sigaction");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

void handleSignal(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		printf("\rserver: shutting down...\n");
		exit(EXIT_SUCCESS);
	}
	else if (signal == SIGCHLD)
	{
		int m_errno = errno; // Preserve errno

		while (waitpid(-1, NULL, WNOHANG) > 0) // Reap zombie processes
			;								   // No action needed, just reaping
		errno = m_errno;					   // Restore errno after waitpid
	}
}

char ***parseRequest(const char *request)
{
	// Split the request into lines
	if (!request || strlen(request) == 0)
	{
		fprintf(stderr, "server: empty request\n");
		return (NULL);
	}

	char **lines = splitStr(request, "\r\n");
	if (!lines)
	{
		fprintf(stderr, "server: failed to parse request\n");
		return (NULL);
	}

	// Count lines
	size_t lineCount = 0;
	while (lines[lineCount])
		lineCount++;

	// Allocate array of arrays
	char ***tokens = malloc(sizeof(char **) * (lineCount + 1));
	if (!tokens)
	{
		perror("server: parseRequest: malloc");
		for (size_t i = 0; i < lineCount; i++)
			free(lines[i]);
		free(lines);
		return (NULL);
	}

	for (size_t i = 0; i < lineCount; i++)
	{
		tokens[i] = splitStr(lines[i], " ");
	}

	tokens[lineCount] = NULL; // Null-terminate the array of arrays

	// Free lines (tokens[i] are new allocations)
	for (size_t i = 0; i < lineCount; i++)
		free(lines[i]);
	free(lines);

	return (tokens);
}

char **getHeader(const char *headerName, char ***parsedRequest)
{
	if (!headerName || !parsedRequest || !*parsedRequest)
		return (NULL);

	for (size_t i = 0; parsedRequest[i]; i++)
		if (strncmp(parsedRequest[i][0], headerName, strlen(headerName)) == 0)
			return (parsedRequest[i]);
	return (NULL);
}

const char *echoEndPoint(char ***parsedRequest)
{
	const char *httpResponse;
	char *start = (*parsedRequest)[1] + 5; // Skip "/echo"
	if (*start == '/')
		start++; // Skip leading slash if present
	size_t len = strlen(start);
	if (len == 0)
		httpResponse = HelloWorldResponse;
	else
	{
		httpResponse = malloc(strlen("HTTP/1.1 200 OK\r\n"
									 "Content-Type: text/plain\r\n"
									 "Content-Length: ") +
							  10 + len + 4);
		if (!httpResponse)
		{
			perror("server: httpGetMethod: malloc");
			return (NULL);
		}
		sprintf((char *)httpResponse, "HTTP/1.1 200 OK\r\n"
									  "Content-Type: text/plain\r\n"
									  "Content-Length: %zu\r\n"
									  "\r\n%s\n",
				len, start);
	}
	return (httpResponse);
}

const char *userAgentEndPoint(char ***parsedRequest)
{
	const char *httpResponse;
	char **userAgentHeader = getHeader("User-Agent", parsedRequest);
	if (userAgentHeader == NULL)
	{
		httpResponse = BadRequestResponse;
	}
	else
	{
		size_t len = strlen(userAgentHeader[1]);
		httpResponse = malloc(strlen("HTTP/1.1 200 OK\r\n"
									 "Content-Type: text/plain\r\n"
									 "Content-Length: ") +
							  10 + len + 4);
		if (!httpResponse)
		{
			perror("server: httpGetMethod: malloc");
			return (NULL);
		}
		sprintf((char *)httpResponse, "HTTP/1.1 200 OK\r\n"
									  "Content-Type: text/plain\r\n"
									  "Content-Length: %zu\r\n"
									  "\r\n%s\n",
				len, userAgentHeader[1]);
	}
	return (httpResponse);
}

const char *httpGetMethod(char ***parsedRequest)
{
	const char *httpResponse;

	if (strncmp((*parsedRequest)[1], "/echo", 5) == 0)
		httpResponse = echoEndPoint(parsedRequest);
	else if (strcmp((*parsedRequest)[1], "/user-agent") == 0)
		httpResponse = userAgentEndPoint(parsedRequest);
	else if (strcmp((*parsedRequest)[1], "/") == 0)
		httpResponse = HelloWorldResponse;
	else
		httpResponse = NotFoundResponse;
	return (httpResponse);
}

void free2d(char **array)
{
	for (size_t i = 0; array && array[i]; i++)
		free(array[i]);
	free(array);
}

void free3d(char ***array)
{
	for (size_t i = 0; array && array[i]; i++)
		free2d(array[i]);
	free(array);
}

int handleClient(int clientFd)
{
	const char *httpResponse = NULL;
	char ***parsedRequest = NULL;
	char requestBuffer[4096];
	int status = EXIT_SUCCESS;

	// Receive HTTP request from the client
	int bytesReceived = recv(clientFd, requestBuffer, sizeof(requestBuffer) - 1, 0);
	if (bytesReceived < 0)
	{
		perror("server: recv");
		status = EXIT_FAILURE;
		goto RET;
	}
	requestBuffer[bytesReceived] = '\0'; // Null-terminate the string

	printf("server: received HTTP request:\n%s\n", requestBuffer);
	printf("server: processing request\n");

	// Prepare HTTP response
	parsedRequest = parseRequest(requestBuffer);
	// Check if the request is empty
	if (parsedRequest == NULL || *parsedRequest == NULL)
	{
		fprintf(stderr, "server: received empty request\n");
		httpResponse = BadRequestResponse;
	}
	else if (strcmp((*parsedRequest)[0], "GET") == 0)
		httpResponse = httpGetMethod(parsedRequest);
	else
		httpResponse = NotFoundResponse;

	// Send HTTP response
	if (send(clientFd, httpResponse, strlen(httpResponse), 0) == -1)
	{
		perror("server: send");
		status = EXIT_FAILURE;
		goto RET;
	}

	printf("server: response sent\n");

RET:
	// Free allocated memory
	free3d(parsedRequest);
	if (strcmp(httpResponse, HelloWorldResponse) != 0 &&
		strcmp(httpResponse, BadRequestResponse) != 0 &&
		strcmp(httpResponse, NotFoundResponse) != 0)
		free((char *)httpResponse);
	close(clientFd);
	if (status == EXIT_FAILURE)
		fprintf(stderr, "server: failed to handle client request\n");
	return (status);
}

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

int main()
{
	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// Install signal handlers
	if (installSignal(SIGINT, handleSignal) != EXIT_SUCCESS ||
		installSignal(SIGTERM, handleSignal) != EXIT_SUCCESS ||
		installSignal(SIGCHLD, handleSignal) != EXIT_SUCCESS)
	{
		fprintf(stderr, "server: failed to install signal handlers\n");
		return (EXIT_FAILURE);
	}

	// Start the server
	int serverFd;
	if (startServer("localhost", PORT, BACKLOG, &serverFd) != EXIT_SUCCESS)
	{
		fprintf(stderr, "server: failed to start\n");
		return (EXIT_FAILURE);
	}

	// Accept incoming connections
	struct sockaddr_storage clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientFd;
	for (int i = 0; i < 1; i = i)
	{
		clientFd = accept(serverFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (clientFd == -1)
		{
			perror("server: accept");
			close(serverFd);
			return (EXIT_FAILURE);
		}

		printf("server: new connection accepted\n");
		if (!fork())
		{
			// Child process handles the client
			close(serverFd); // Close the server socket in the child process
			printf("server: handling client in child process\n");
			exit(handleClient(clientFd));
		}
	}

	// Close the server socket
	close(serverFd);
	return 0;
}
