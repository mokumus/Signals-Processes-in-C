
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
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>

/*--------------------------DEFINES---------------------------*/
#define errExit(msg)    \
	do                    \
	{                     \
		perror(msg);        \
		exit(EXIT_FAILURE); \
	} while (0)

#define MY_FLAGS O_RDWR | O_SYNC

/*--------------------------GLOBALS---------------------------*/
pid_t pid[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
sig_atomic_t exit_requested = 0, childs_done = 0, parent_done = 0;
int *i_child_done;
struct flock lock;
sigset_t mask, oldmask;

/* -----------------------PROTOTYPES--------------------------*/
void print_usage(void);
int is_parent(void);
void process_line(int fd, int n);
void sig_handler(int sig_no);

int main(int argc, char *argv[])
{
	int fd;
	i_child_done = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*i_child_done = 0;

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

	setbuf(stdout, NULL); // Disable stdout buffering for library functions
	signal(SIGINT, sig_handler);
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);

	// Create 8 childeren process=========================================
	for (int i = 0; i < 8; i++)
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

		// Wait for all the childeren(1. Wait(SIGUSR1))=====================================

		/* Set up the mask of signals to temporarily block. */
		sigemptyset(&mask);
		sigaddset(&mask, SIGUSR1);
		/* Wait for a signal to arrive. */
		sigprocmask(SIG_BLOCK, &mask, &oldmask);
		printf("Parent waiting: %d\n", childs_done);
		while (!childs_done)
			sigsuspend(&oldmask);
		sigprocmask(SIG_UNBLOCK, &mask, NULL);

		printf("Parent's done waiting: %d\n", childs_done);

		//DO PARENT THINGS

		//SIGNAL CHILDEREN(SIGUSR2)
		for(int i = 0; i < 8; i++){
			printf("Parent signalling to C%d\n",i);
			kill(pid[i], SIGUSR2);
		}



		// Wait for all the childeren(2. Wait(Waitpid))=====================================

		for (int i = 0; i < 8 || exit_requested != 0; i++)
		{
			int status;
			if(waitpid(pid[i], &status, 0) == -1){
				errExit("waitpid");
			}
			printf("waitpid%d\n", i);
		}

		// =====================================Wait for all the childeren
		if (exit_requested)
		{
			printf("\nExit request by the user. Signal: %d\n", exit_requested);
			close(fd);
			exit(EXIT_FAILURE);
		}
		else
		{
			printf("Parent: all childeren exited\n");
			close(fd);
			exit(EXIT_SUCCESS);
		}
	}

	// Child processes ===================================================
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (pid[i] == 0)
			{
				printf("I'm C%d [pid: %d, ppid: %d]\n", i, getpid(), getppid());
				process_line(fd, i);
	

				sigemptyset(&mask);
				sigaddset(&mask, SIGUSR2);
				/* Wait for a signal to arrive. */
				sigprocmask(SIG_BLOCK, &mask, &oldmask);
				printf("C%d is waiting\n", i);
				while (!parent_done)
					sigsuspend(&oldmask);
				sigprocmask(SIG_UNBLOCK, &mask, NULL);

				printf("C%d is done waiting\n", i);

				_exit(EXIT_SUCCESS);
			}
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

void process_line(int fd, int n)
{
	int i = 0, k = 0, j = 0;
	int line = 0;
	char c;
	char buffer[1024];
	char after_buffer[1024];
	float arr[8][2];
	//char buffer[255];

	/* Initialize the flock structure. */
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;
	/* Place a write lock on the file. */
	fcntl(fd, F_SETLKW, &lock);

	while (pread(fd, &c, 1, i++) && line <= n)
	{
		if (line == n)
			buffer[k++] = c;

		if (c == '\n')
		{
			buffer[k - 1] = '\0';
			line++;
		}
	}

	//printf("buff%d: %s\n", n,buffer);
	while (pread(fd, &c, 1, (i++) - 1))
	{
		after_buffer[j++] = c;
	}
	after_buffer[j] = '\0';

	const char padding[] = ": x.x\n";

	pwrite(fd, padding, strlen(padding), i - j - 3);
	pwrite(fd, after_buffer, j, i - j - 3 + strlen(padding));

	/* Release the lock. */
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);

	//printf("after_buffer:\n%s ", after_buffer);

	sscanf(buffer, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &arr[0][0], &arr[0][1],
				 &arr[1][0], &arr[1][1],
				 &arr[2][0], &arr[2][1],
				 &arr[3][0], &arr[3][1],
				 &arr[4][0], &arr[4][1],
				 &arr[5][0], &arr[5][1],
				 &arr[6][0], &arr[6][1],
				 &arr[7][0], &arr[7][1]);

	(*i_child_done)++;
	if (*i_child_done == 8)
		kill(getppid(), SIGUSR1);
}

void sig_handler(int sig_no)
{
	if (sig_no == SIGUSR1)
		childs_done = 1;
	else if (sig_no == SIGUSR2)
		parent_done = 1;
	else
		exit_requested = sig_no;
}