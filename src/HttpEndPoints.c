#include "Http.h"

struct HttpResponse *echoEndPoint(struct HttpRequest *req)
{
	struct HttpResponse *resp = calloc(1, sizeof(struct HttpResponse));
	if (!resp)
		return (NULL);
	const char *start = req->path + 5; // Skip "/echo"
	if (*start == '/')
		start++;
	size_t len = strlen(start);
	resp->statusCode = HTTP_OK;
	resp->statusMessage = strdup("OK");
	resp->contentType = strdup("text/plain");
	resp->contentLength = len == 0 ? 14 : len;
	resp->body = len == 0 ? strdup("Hello, world!\n") : strdup(start);
	return (resp);
}

struct HttpResponse *userAgentEndPoint(struct HttpRequest *req)
{
	struct HttpResponse *resp = calloc(1, sizeof(struct HttpResponse));
	if (!resp)
		return (NULL);
	const char *start = req->path + 11; // Skip "/user-agent"
	if (*start == '/')
		start++;
	if (*start != '\0')
	{
		resp->statusCode = HTTP_BAD_REQUEST;
		resp->statusMessage = strdup("Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 12;
		resp->body = strdup("Bad Request\n");
		return (resp);
	}
	const char *userAgent = getHeader("User-Agent", req);
	if (!userAgent)
	{
		resp->statusCode = HTTP_BAD_REQUEST;
		resp->statusMessage = strdup("Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 12;
		resp->body = strdup("Bad Request\n");
	}
	else
	{
		size_t len = strlen(userAgent);
		resp->statusCode = HTTP_OK;
		resp->statusMessage = strdup("OK");
		resp->contentType = strdup("text/plain");
		resp->contentLength = len;
		resp->body = strdup(userAgent);
	}
	return (resp);
}

struct HttpResponse *getFilesEndPoint(struct HttpRequest *req)
{
	struct HttpResponse *resp = calloc(1, sizeof(struct HttpResponse));
	if (!resp)
		return (NULL);
	const char *file;
	const char *start = req->path + 6; // Skip "/files"
	if (*start == '/')
		start++;
	if (*start == '\0')
		file = "index.html";
	else
		file = start;
	int fileFd = open(file, O_RDONLY);
	if (fileFd == -1)
	{
		resp->statusCode = (errno == EACCES) ? HTTP_FORBIDDEN : (errno == ENOENT) ? HTTP_NOT_FOUND
																				  : HTTP_BAD_REQUEST;
		resp->statusMessage = strdup((errno == EACCES) ? "Forbidden" : (errno == ENOENT) ? "Not Found"
																						 : "Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = strlen(resp->statusMessage);
		resp->body = strdup(resp->statusMessage);
		return (resp);
	}
	long fileSize = getFileSize(fileFd);
	if (fileSize == -1)
	{
		close(fileFd);
		resp->statusCode = HTTP_BAD_REQUEST;
		resp->statusMessage = strdup("Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 12;
		resp->body = strdup("Bad Request\n");
		return (resp);
	}
	resp->statusCode = HTTP_OK;
	resp->statusMessage = strdup("OK");
	resp->contentType = getMimeType(file);
	resp->contentLength = fileSize;
	resp->body = malloc(fileSize);
	if (!resp->body)
	{
		close(fileFd);
		resp->statusCode = HTTP_BAD_REQUEST;
		resp->statusMessage = strdup("Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 12;
		resp->body = strdup("Bad Request\n");
		return (resp);
	}
	ssize_t totalRead = 0;
	while (totalRead < fileSize)
	{
		ssize_t rc = read(fileFd, resp->body + totalRead, fileSize - totalRead);
		if (rc <= 0)
		{
			close(fileFd);
			free(resp->body);
			resp->statusCode = HTTP_BAD_REQUEST;
			resp->statusMessage = strdup("Bad Request");
			resp->contentType = strdup("text/plain");
			resp->contentLength = 12;
			resp->body = strdup("Bad Request\n");
			return (resp);
		}
		totalRead += rc;
	}
	close(fileFd);
	return (resp);
}

struct HttpResponse *postFilesEndPoint(struct HttpRequest *req)
{
	struct HttpResponse *resp = calloc(1, sizeof(struct HttpResponse));
	if (!resp)
		return NULL;
	const char *file;
	const char *start = req->path + 6;
	if (*start == '/')
		start++;
	if (*start == '\0')
	{
		resp->statusCode = HTTP_BAD_REQUEST;
		resp->statusMessage = strdup("Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 12;
		resp->body = strdup("Bad Request\n");
		return resp;
	}
	file = start;
	int fileFd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fileFd == -1)
	{
		resp->statusCode = (errno == EACCES) ? HTTP_FORBIDDEN : HTTP_BAD_REQUEST;
		resp->statusMessage = strdup((errno == EACCES) ? "Forbidden" : "Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = strlen(resp->statusMessage);
		resp->body = strdup(resp->statusMessage);
		return (resp);
	}
	if (req->body && req->contentLength > 0)
	{
		ssize_t written = write(fileFd, req->body, req->contentLength);
		if (written < 0)
		{
			close(fileFd);
			resp->statusCode = HTTP_BAD_REQUEST;
			resp->statusMessage = strdup("Bad Request");
			resp->contentType = strdup("text/plain");
			resp->contentLength = 12;
			resp->body = strdup("Bad Request\n");
			return (resp);
		}
	}
	close(fileFd);
	resp->statusCode = HTTP_CREATED;
	resp->statusMessage = strdup("Created");
	resp->contentType = strdup("text/plain");
	resp->contentLength = 26;
	resp->body = strdup("File created successfully\n");
	return (resp);
}
