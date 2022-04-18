#include "simple_shell.h"

/**
 * main - Shell program (entry point)
 * Return: Always 0 (Success)
 */

int main(int argc, char *argv[], char **envs)
{
	char *line = NULL, *commandPath = NULL, *envPath;
	int count = 0;
	struct stat statistics;
	cmd_t *cmd;

	envPath = getenv("PATH");
	signal(SIGINT, new_signal_handler);
	while (true)
	{
		line = NULL, commandPath = NULL, count++;

		if (isatty(STDIN_FILENO) == 1)
		{
			if (write(STDOUT_FILENO, "$ ", 2) == EOF)
				exit(EXIT_FAILURE);
		}
		_getline(&line);
		cmd = parse_cmd(line);
		free(line);

		if (execute_builtin(cmd, envs))
			continue;

		commandPath = get_path_from_command(cmd, envPath);
		if (!commandPath)
		{
			printf("%s: command not found\n", cmd->command);
			free(commandPath), free_all(cmd);
			continue;
		}
		execute_non_builtin(cmd, commandPath);
		free(commandPath), free_all(cmd);
	}
	return (EXIT_SUCCESS);
}
