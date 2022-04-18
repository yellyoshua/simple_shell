#include "simple_shell.h"

int execute(cmd_t *cmd, char *commandPath)
{
	pid_t childPid = fork();
	int statusLock;
	char **arg_list = NULL;
	int totalCommand = cmd->n_args + 1;
	int i = 0;

	arg_list = malloc(sizeof(char *) * totalCommand + 1);
	if (!arg_list)
		return (EXIT_FAILURE);

	while (i < totalCommand)
	{
		if (i == 0)
			arg_list[i] = strdup(commandPath);
		else
			arg_list[i] = strdup(cmd->args[i - 1]);
		i++;
	}
	arg_list[i] = NULL;

	if (childPid == 0)
	{
		execv(commandPath, arg_list);
	}
	wait(&statusLock);

	if (arg_list)
	{
		for (i = 0; arg_list[i]; i++)
			free(arg_list[i]);
		free(arg_list);
	}
	return (EXIT_SUCCESS);
}

/**
 * main - Shell program (entry point)
 * Return: Always 0 (Success)
 */
int main(int argc, char *argv[], char **envs)
{
	/* Variables */
	size_t len = 0;
	ssize_t nread = 0;
	char *line = NULL;
	int count = 0;
	struct stat statistics;
	char *commandPath = NULL, *envPath;

	envPath = getenv("PATH");

	signal(SIGINT, new_signal_handler);

	while (true)
	{
		line = NULL;
		commandPath = NULL;
		count++;

		if (isatty(STDIN_FILENO) == 1)
		{
			if (write(STDOUT_FILENO, "$ ", 2) == EOF)
				exit(EXIT_FAILURE);
		}
		if ((nread = getline(&line, &len, stdin)) == EOF)
		{
			if (isatty(STDIN_FILENO) != 0)
				write(STDOUT_FILENO, "\n", 1);
			exit(EXIT_FAILURE);
		}

		cmd_t *cmd = parse_cmd(line);
		free(line);

		if (strcmp(cmd->command, "exit") == 0)
		{
			free_all(cmd);
			exit(EXIT_SUCCESS);
		}

		if (strcmp(cmd->command, "env") == 0)
		{
			while (*envs)
			{
				printf("%s\n", *envs);
				envs++;
			}
			free_all(cmd);
			continue;
		}

		if (strcmp(cmd->command, "cd") == 0)
		{
			printf("I'm cd command\n");
			free_all(cmd);
			continue;
		}

		commandPath = get_path_from_command(cmd, envPath);
		if (!commandPath)
		{
			printf("%s: command not found\n", cmd->command);
			free(commandPath);
			free_all(cmd);
			continue;
		}

		execute(cmd, commandPath);

		free(commandPath);
		free_all(cmd);
	}
	return (EXIT_SUCCESS);
}
