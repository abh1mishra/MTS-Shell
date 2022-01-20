#include <stdio.h>
#include <unistd.h>

int main() {

	/* First, fork() three times. */

	fork();
	fork();
	fork();

	/* Each running process prints "done" when it finishes. */

	printf("done\n");

	/* How many times does "done" get printed?  It's the same as the
	 * number of processes.  How many times is that?  Every time we
	 * fork(), we create one more process.  However, each child has
	 * the exact same code, so each child is forking as well.  Thus,
	 * every time we see a fork() in this program, it has the effect
	 * of doubling the number of processes which are running. 
	 *
	 * The picture will be clear if we draw a process graph. 
	 */

	return 0;
}
