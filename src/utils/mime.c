#include "Http.h"

char *getMimeType(const char *filename)
{
	static const char *exts[] = {
		"css", "gif", "htm", "html", "ico", "jpeg", "jpg", "js", "json", "mp3", "mp4", "ogg", "pdf", "png", "svg", "tar", "txt", "wav", "webm", "xml", "zip"};
	static const char *mimes[] = {
		"text/css", "image/gif", "text/html", "text/html", "image/x-icon", "image/jpeg", "image/jpeg", "application/javascript", "application/json", "audio/mpeg", "video/mp4", "audio/ogg", "application/pdf", "image/png", "image/svg+xml", "application/x-tar", "text/plain", "audio/wav", "video/webm", "application/xml", "application/zip"};
	const char *ext = strrchr(filename, '.');
	if (ext)
	{
		ext++;
		for (size_t i = 0; i < sizeof(exts) / sizeof(exts[0]); i++)
		{
			if (strcasecmp(ext, exts[i]) == 0)
				return strdup(mimes[i]);
		}
	}
	// Fallback to file command
	const char *cmd[] = {"file", "--mime-type", "-b", filename, NULL};
	char *mimeType = get_output((char **)cmd);
	if (!mimeType)
	{
		fprintf(stderr, "server: getMimeType: failed to get MIME type for '%s'\n", filename);
		return strdup("application/octet-stream");
	}
	return mimeType;
}
