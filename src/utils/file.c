#include "Http.h"

long getFileSize(int fileFd)
{
	struct stat st;

	if (fstat(fileFd, &st) == 0)
		return (st.st_size);
	else
		return (-1); // Error
}
