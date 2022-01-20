#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NUM 20

/* Spawn multiple processes and wait for each one to finish. 
 * We'll use NUM to define how many processes. */

int main() {

	/* The PID of each process we start, so we know which order
	 * to wait for them to finish. */
	pid_t pids[NUM] = {0};
	
	/* Loop through NUM times, and spawn a new process every time */
	for (int i = 0;  i < NUM;  i++) {
		pids[i] = fork();  // spawn a child, and remember its PID


		if (!pids[i]) { 
			// we are the child

			/* A delay loop, just to make sure the order of
			 * completion results are a bit more random */
			for (int j = 0;  j < 1000;  j++);

			// exit the child immediately
			exit(i); 
		}
	}

	// loop through each child process
	for (int i = 0;  i < NUM;  i++) {
		int status;
		
		//pid_t pid = wait(&status);  // wait for any child

		// wait for a *specific* child
		pid_t pid = waitpid(pids[i], &status, 0); 

		// we can print out the status of the child, too
		printf("child process %ld finished with exit code %d\n", 
				pid, WEXITSTATUS(status));
	}

	return 0;
}
