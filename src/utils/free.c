#include "Http.h"

void freeHttpRequest(struct HttpRequest *req)
{
	if (!req)
		return;
	free(req->path);
	free2d(req->headers);
	free(req->body);
	free(req);
}

void freeHttpResponse(struct HttpResponse *resp)
{
	if (!resp)
		return;
	free(resp->statusMessage);
	free(resp->contentType);
	free(resp->headers);
	free(resp->body);
	free(resp);
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
