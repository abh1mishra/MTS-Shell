#include <stdio.h>
#include <unistd.h>

/**********************************************************/
/***      Fork-based Schroedinger's Cat simulater       ***/
/**********************************************************/

int main() {
	// Let's look inside the box and check whether the cat is alive or dead
	if (fork()) {
		printf("the cat lives!\n");
	} else {
		printf("the cat died. :(\n");
	}

	/* What happened here?  It printed both messages?  How?
	 *
	 * When we fork(), we create a second process which is identical to
	 * the first one, but runs independently from the first.  After
	 * the fork(), one of the processes prints the first branch, while
	 * the other process prints the second branch.  
	 *
	 * Both print to the same terminal, so it looks like we're getting 
	 * both outputs, when it's really one output from two different
	 * running processes.  Fork is almost like a function which is
	 * called once but returns twice. 
	 *
	 * Which process picks which branch?  In the parent process, the
	 * fork() returns the (non-zero) Process ID of the child, which
	 * is effectively a TRUE result: it executes the "then" branch.
	 * In the child, fork() returns zero, which is effectively a FALSE
	 * result: it executes the "else" branch.
	 */

	return 0;
}
