#include "Http.h"

int main(int argc, const char *argv[])
{
	if (argc > 1 && strcmp(argv[1], "--directory") == 0 && argc > 2)
	{
		if (chdir(argv[2]) != 0)
		{
			perror("server: chdir");
			return (EXIT_FAILURE);
		}
		printf("server: changed working directory to '%s'\n", argv[2]);
	}

	// Disable output buffering
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	// Install signal handlers
	installSignals();

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
	while (true)
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
