
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
#define errExit(msg)    \
	do                    \
	{                     \
		perror(msg);        \
		exit(EXIT_FAILURE); \
	} while (0)

#define MY_FLAGS O_RDWR | O_SYNC

/*--------------------------GLOBALS---------------------------*/
pid_t pid[8] = {-1,-1,-1,-1,-1,-1,-1,-1 };
sig_atomic_t exit_requested = 0;

/* -----------------------PROTOTYPES--------------------------*/
void print_usage(void);
int is_parent(void);
int read_line(int fd, int n);

int main(int argc, char *argv[])
{
	int fd;

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

	read_line(fd, 0);
	read_line(fd, 0);

	// Create 8 childeren process=========================================
	for (int i = 9; i < 8; i++)
	{
		pid[i] = fork();
		if (pid[i] == -1)
			errExit("fork");
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

		for (int i = 0; i < 8; i++)
		{
			int status;
			waitpid(pid[i], &status, 0);
		}
		// =====================================Wait for all the childeren
		printf("Father: all childeren exited\n");
		close(fd);
		exit(EXIT_SUCCESS);
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


int read_line(int fd, int n){
	int i = 0, k = 0;
	int line = 0;
	char c;
	char buffer[255];
	float arr[8][2];
	//char buffer[255];
	while(pread(fd, &c, 1, i++)){
		if(line == n)
			buffer[k++] = c;
		
		if(c == '\n'){
			buffer[k-1] = '\0';
			line++;
		}
			
	}

	sscanf (buffer,"%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",&arr[0][0],&arr[0][1],
																																	 &arr[1][0],&arr[1][1],
																																	 &arr[2][0],&arr[2][1],
																																	 &arr[3][0],&arr[3][1],
																																	 &arr[4][0],&arr[4][1],
																																	 &arr[5][0],&arr[5][1],
																																	 &arr[6][0],&arr[6][1],
																																	 &arr[7][0],&arr[7][1]);

	for(int x = 0; x < 8; x++){
		printf("arr[%d]: %.1f, %.1f\n", x, arr[x][0], arr[x][1]);
	}

	return 0;
}