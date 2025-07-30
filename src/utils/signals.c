#include "Http.h"

int installSignals(void)
{
	if (installSignal(SIGINT, handleSignal) != EXIT_SUCCESS ||
		installSignal(SIGTERM, handleSignal) != EXIT_SUCCESS ||
		installSignal(SIGCHLD, handleSignal) != EXIT_SUCCESS)
	{
		fprintf(stderr, "server: failed to install signal handlers\n");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
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
