#include "Http.h"

char *get_output(char **cmd)
{
	pid_t pid;
	int fd[3], status, len;
	char *output = NULL;

	if (pipe(fd) == -1)
	{
		perror("server: get_output: pipe");
		return (NULL);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("server: get_output: fork");
		close_pipe(fd);
		return (NULL);
	}
	if (pid == 0)
	{
		dup2(fd[1], STDOUT_FILENO);
		close_pipe(fd);
		close(fd[2]);
		execvp(cmd[0], cmd);
		perror("server: get_output: execvp");
		exit(EXIT_FAILURE);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		output = get_next_line(fd[0]);
	len = output ? strlen(output) : 0;
	if (len > 0 && output[len - 1] == '\n')
		output[len - 1] = '\0';
	close_pipe(fd);
	return (output);
}
