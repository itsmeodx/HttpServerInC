#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <netdb.h>

#define PORT "4221" // Port number for the server
#define BACKLOG 10	// Number of pending connections queue

#define BUFFERSIZE 1024

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
#define ForbiddenResponse "HTTP/1.1 403 Forbidden\r\n"   \
						  "Content-Type: text/plain\r\n" \
						  "Content-Length: 18\r\n"       \
						  "\r\n"                         \
						  "Permission Denied\n"

// HTTP status codes
#define HTTP_OK 200
#define HTTP_CREATED 201
#define HTTP_BAD_REQUEST 400
#define HTTP_NOT_FOUND 404
#define HTTP_FORBIDDEN 403

// HTTP methods
#define HTTP_UNKNOWN -1 // Unknown method
#define HTTP_GET 0
#define HTTP_POST 1

struct HttpRequest
{
	int method;			   // HTTP_GET or HTTP_POST
	char *path;			   // Request path (e.g., "/index.html")
	char **headers;		   // Array of headers, NULL-terminated
	char *body;			   // Request body, NULL if not present
	ssize_t contentLength; // Length of the body
};

struct HttpResponse
{
	int statusCode;		  // HTTP status code
	char *statusMessage;  // HTTP status message (e.g., "OK", "Bad Request")
	char *contentType;	  // Content-Type header (e.g., "text/plain", "application/octet-stream")
	size_t contentLength; // Content-Length header
	char *headers;		  // Response headers
	char *body;			  // Response body
};

// src/Http.c
const char *getHeader(const char *headerName, struct HttpRequest *req);
struct HttpRequest *parseHttpRequest(const char *request);
ssize_t sendHttpResponse(int clientFd, struct HttpResponse *response);

// src/HttpEndPoints.c
struct HttpResponse *echoEndPoint(struct HttpRequest *req);
struct HttpResponse *userAgentEndPoint(struct HttpRequest *req);
struct HttpResponse *getFilesEndPoint(struct HttpRequest *req);
struct HttpResponse *postFilesEndPoint(struct HttpRequest *req);

// src/HttpMethods.c
struct HttpResponse *httpGetMethod(struct HttpRequest *req);
struct HttpResponse *httpPostMethod(struct HttpRequest *req);

// src/server.c
int startServer(const char *name, const char *port, int backlog, int *serverFd);
int handleClient(int clientFd);

// src/utils/file.c
long getFileSize(int fileFd);

// src/utils/free.c
void freeHttpRequest(struct HttpRequest *req);
void free2d(char **array);
void free3d(char ***array);

// src/utils/signals.c
int installSignals(void);
int installSignal(int signal, void (*handler)(int));
void handleSignal(int signal);

// src/utils/split.c
char **splitStr(const char *str, const char *delim);

#endif // HTTP_H
