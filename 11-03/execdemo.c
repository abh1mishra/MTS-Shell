#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	printf("getting ready to exec\n");

	// fork a child
	if (!fork()) {
		// we are the child

		/* run the ls program (this will take over and replace
	         * the entire process. */	
		execl("/usr/bin/ls", "ls", "-l", NULL);

		/* a different form of exec call, which we might use
		 * if we have the arg list as an array */
		//execv("/usr/bin/ls", argv);
	}

	// wait for the child to finish (in order to sync)
	pid_t pid = wait(NULL);

	printf("done with exec, process %ld finished\n", pid);

	return 0;
}
