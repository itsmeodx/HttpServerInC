#include "Http.h"

const char *getHeader(const char *headerName, struct HttpRequest *req)
{
	if (!headerName || !req || !req->headers)
		return (NULL);
	size_t headerNameLen = strlen(headerName);
	for (char **h = req->headers; h && *h; h++)
	{
		// Match "Header-Name:" or "Header-Name: value"
		if (strncmp(*h, headerName, headerNameLen) == 0 && (*h)[headerNameLen] == ':')
		{
			// Skip colon and possible space
			const char *value = *h + headerNameLen + 1;
			while (*value == ' ')
				value++;
			return (value);
		}
	}
	return (NULL);
}

struct HttpRequest *parseHttpRequest(const char *request)
{
	if (!request || strlen(request) == 0)
		return (NULL);

	struct HttpRequest *req = calloc(1, sizeof(struct HttpRequest));
	if (!req)
		return (NULL);

	char **lines = splitStr(request, "\r\n");
	if (!lines)
	{
		free(req);
		return (NULL);
	}

	// Parse request line
	char **requestLine = splitStr(lines[0], " ");
	if (!requestLine || !requestLine[0] || !requestLine[1])
	{
		free2d(requestLine);
		free2d(lines);
		free(req);
		return (NULL);
	}
	if (strcmp(requestLine[0], "GET") == 0)
		req->method = HTTP_GET;
	else if (strcmp(requestLine[0], "POST") == 0)
		req->method = HTTP_POST;
	else
		req->method = HTTP_UNKNOWN;
	req->path = strdup(requestLine[1]);
	free2d(requestLine);

	// Parse headers
	size_t headerCount = 0;
	while (lines[headerCount + 1] && strlen(lines[headerCount + 1]) > 0)
		headerCount++;
	req->headers = malloc(sizeof(char *) * (headerCount + 1));
	for (size_t i = 0; i < headerCount; i++)
		req->headers[i] = strdup(lines[i + 1]);
	req->headers[headerCount] = NULL;

	// Find body using \r\n\r\n and Content-Length
	const char *bodyStartPtr = strstr(request, "\r\n\r\n");
	req->body = NULL;
	req->contentLength = 0;
	if (bodyStartPtr)
	{
		bodyStartPtr += 4; // Skip past \r\n\r\n
		// Get Content-Length header
		const char *cl = getHeader("Content-Length", req);
		if (cl)
		{
			ssize_t len = atoi(cl);
			if (len > 0)
			{
				req->body = malloc(len + 1);
				if (req->body)
				{
					memcpy(req->body, bodyStartPtr, len);
					req->body[len] = '\0';
					req->contentLength = len;
				}
			}
		}
	}

	free2d(lines);
	return (req);
}

ssize_t sendHttpResponse(int clientFd, struct HttpResponse *response)
{
	if (!response || !response->statusMessage || !response->contentType)
		return (-1);

	ssize_t bytesSent = 0;
	// Calculate required header buffer size
	size_t headersLen = strlen(response->statusMessage) + strlen(response->contentType) + 128;
	size_t extraHeadersLen = response->headers ? strlen(response->headers) : 0;
	headersLen += extraHeadersLen;
	char *responseBuffer = malloc(headersLen);
	if (!responseBuffer)
	{
		perror("server: sendHttpResponse: malloc");
		return (-1);
	}
	int len = snprintf(responseBuffer, headersLen,
					   "HTTP/1.1 %d %s\r\n"
					   "Content-Type: %s\r\n"
					   "Content-Length: %zu\r\n"
					   "Connection: close\r\n"
					   "%s"
					   "\r\n",
					   response->statusCode,
					   response->statusMessage,
					   response->contentType,
					   response->contentLength,
					   response->headers ? response->headers : "");
	if (len < 0 || (size_t)len >= headersLen)
	{
		free(responseBuffer);
		perror("server: sendHttpResponse: snprintf overflow");
		return (-1);
	}
	ssize_t totalSent = 0;
	while (totalSent < len)
	{
		ssize_t sent = send(clientFd, responseBuffer + totalSent, len - totalSent, 0);
		if (sent <= 0)
		{
			perror("server: sendHttpResponse: send");
			free(responseBuffer);
			return (-1);
		}
		totalSent += sent;
	}
	bytesSent = totalSent;
	if (response->body && response->contentLength > 0)
	{
		ssize_t bodySent = 0;
		while (bodySent < response->contentLength)
		{
			ssize_t sent = send(clientFd, response->body + bodySent, response->contentLength - bodySent, 0);
			if (sent <= 0)
			{
				perror("server: sendHttpResponse: send body");
				free(responseBuffer);
				return (-1);
			}
			bodySent += sent;
		}
		bytesSent += bodySent;
	}
	free(responseBuffer);
	return (bytesSent);
}
