#include "Http.h"

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
			perror("server: strSplit: malloc");
			goto BAD;
		}
		memcpy(token, start, len);
		token[len] = '\0';
		result = realloc(result, sizeof(char *) * (count + 1));
		if (!result)
		{
			perror("server: strSplit: realloc");
			goto BAD;
		}
		result[count++] = token;
		start = found + delim_len;
	}
	// Add the last token
	char *token = strdup(start);
	if (!token)
	{
		perror("server: strSplit: strdup");
		goto BAD;
	}
	result = realloc(result, sizeof(char *) * (count + 2));
	if (!result)
	{
		perror("server: strSplit: realloc");
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
