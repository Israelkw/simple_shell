#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define MAXARGS 128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
extern char **environ;

/**
 * main - main function of the shell
 * Return: 0 on success
 */
int main(void)
{
	char cmdline[MAXLINE];

	while (1)
	{
		printf("> ");
		fgets(cmdline, MAXLINE, stdin);
		if (feof(stdin))
			exit(0);
		eval(cmdline);
	}
}

/**
 * eval - evaluate the command line
 * @cmdline: the input command line
 */
void eval(char *cmdline)
{
	char *argv[MAXARGS];
	char buf[MAXLINE];
	int bg;
	pid_t pid;

	strcpy(buf, cmdline);
	bg = parseline(buf, argv);
	if (argv[0] == NULL)
		return;

	if (!builtin_command(argv))
	{
		pid = fork();
		if (pid == 0)
		{
			if (execve(argv[0], argv, environ) < 0)
			{
				printf("%s: Command not found.\n", argv[0]);
				exit(0);
			}
		}

		if (!bg)
		{
			int status;

			if (waitpid(pid, &status, 0) < 0)
				printf("waitfg: waitpid error");
		}
		else
			printf("%d %s", pid, cmdline);
	}
}

/**
 * parseline - parse the command line and build the argv array
 * @buf: the input buffer
 * @argv: the output argument array
 * Return: 1 if the command line runs in the background, 0 otherwise
 */
int parseline(char *buf, char **argv)
{
	char *delim;
	int argc;
	int bg;

	buf[strlen(buf) - 1] = ' ';
	while (*buf && (*buf == ' '))
		buf++;

	argc = 0;
	while ((delim = strchr(buf, ' ')))
	{
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' '))
			buf++;
	}
	argv[argc] = NULL;

	if (argc == 0)
		return (1);

	bg = *argv[argc - 1] == '&';
	if (bg)
		argv[--argc] = NULL;

	return (bg);
}

/**
 * builtin_command - check if the command is a built-in command
 * @argv: the argument array
 * Return: 1 if it is a built-in command, 0 otherwise
 */
int builtin_command(char **argv)
{
	if (!strcmp(argv[0], "quit"))
		exit(0);
	if (!strcmp(argv[0], "&"))
		return (1);
	return (0);
}
