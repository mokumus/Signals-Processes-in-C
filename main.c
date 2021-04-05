
//
//  main.c
//  Signals and Process
//
//  Created by Muhammed Okumuş on 02.04.2021.
//  Copyright © 2021 Muhammed Okumuş. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

/*--------------------------DEFINES---------------------------*/

#define MY_FLAGS O_RDWR | O_SYNC

/*--------------------------GLOBALS---------------------------*/
pid_t pid[8];
sig_atomic_t exit_requested = 0;

/* -----------------------PROTOTYPES--------------------------*/
void print_usage(void);
int is_parent(void);

int main(int argc, char *argv[])
{
	int fd;
	pid_t wpid;

	printf("argv[1]: %s\n", argv[1]);
	if (argv[1] == NULL)
	{
		print_usage();
		exit(EXIT_FAILURE);
	}

	// File access validity====================================
	fd = open(argv[1], MY_FLAGS);

	if (fd == -1)
	{
		fprintf(stderr, "Input file could not be opened: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// Create 8 childeren process=========================================
	for (int i = 0; i < 8; i++)
	{
		pid[i] = fork();
		if (pid[i] == 0)
			break;
	}
	// ===================================================================

	// Parent process ====================================================
	if (is_parent())
	{
		// That's the father, it waits for all the childs
		printf("I'm the father [pid: %d, ppid: %d]\n", getpid(), getppid());

		// Wait for all the childeren=====================================
		do
		{
			wpid = wait(NULL);
		} while (wpid == -1 && errno == EINTR);

		if (wpid == -1)
		{
			perror("Wait error\n");
			exit(EXIT_FAILURE);
		}
		// =====================================Wait for all the childeren
	}

	// Child processes ===================================================

	else
	{

		if (pid[0] == 0)
		{
			printf("I'm C0 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}
		else if (pid[1] == 0)
		{
			printf("I'm C1 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}
		else if (pid[2] == 0)
		{
			printf("I'm C2 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}
		else if (pid[3] == 0)
		{
			printf("I'm C3 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}
		else if (pid[4] == 0)
		{
			printf("I'm C4 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}
		else if (pid[5] == 0)
		{
			printf("I'm C5 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}

		else if (pid[6] == 0)
		{
			printf("I'm C6 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}
		else if (pid[7] == 0)
		{
			printf("I'm C7 [pid: %d, ppid: %d]\n", getpid(), getppid());

			_exit(EXIT_SUCCESS);
		}
	}
	// ===================================================================
	 exit(EXIT_SUCCESS);

	return 0;
}

void print_usage(void)
{
	printf("========================================\n"
				 "Usage:\n"
				 "$ ./processM pathToFile\n");
}

int is_parent(void)
{
	return pid[0] != 0 && pid[1] != 0 && pid[2] != 0 && pid[3] != 0 && pid[4] != 0 && pid[5] != 0 && pid[6] != 0 && pid[7] != 0;
}