#include "Http.h"

struct HttpResponse *httpPostMethod(struct HttpRequest *req)
{
	if (strncmp(req->path, "/files", 6) == 0)
		return (postFilesEndPoint(req));
	else {
		struct HttpResponse *resp = calloc(1, sizeof(struct HttpResponse));
		resp->statusCode = HTTP_BAD_REQUEST;
		resp->statusMessage = strdup("Bad Request");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 12;
		resp->body = strdup("Bad Request\n");
		return (resp);
	}
}

struct HttpResponse *httpGetMethod(struct HttpRequest *req)
{
	if (strncmp(req->path, "/echo", 5) == 0)
		return (echoEndPoint(req));
	else if (strncmp(req->path, "/user-agent", 11) == 0)
		return (userAgentEndPoint(req));
	else if (strncmp(req->path, "/files", 6) == 0)
		return (getFilesEndPoint(req));
	else if (strcmp(req->path, "/") == 0) {
		struct HttpResponse *resp = calloc(1, sizeof(struct HttpResponse));
		resp->statusCode = HTTP_OK;
		resp->statusMessage = strdup("OK");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 14;
		resp->body = strdup("Hello, world!\n");
		return (resp);
	} else {
		struct HttpResponse *resp = calloc(1, sizeof(struct HttpResponse));
		resp->statusCode = HTTP_NOT_FOUND;
		resp->statusMessage = strdup("Not Found");
		resp->contentType = strdup("text/plain");
		resp->contentLength = 10;
		resp->body = strdup("Not Found\n");
		return (resp);
	}
}
