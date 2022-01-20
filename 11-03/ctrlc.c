#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

/* A signal handler: the important thing is that it takes one
 * int parameter, and that it returns void.  We'll use this whenever
 * we hit ctrl-C. */
void handler(int sig) {

        // printf("hasta la vista, baby!\n");
	printf("catchphrase!\n");
	exit(0);
}

int main() {
	// create and initialize sa, a sigaction object
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa)); // initialze by zeroing the entire thing

	// set the new handler by using a pointer to the function above
	sa.sa_handler = handler;


	// install the new handler for SIGINT, sa
	sigaction(SIGINT, &sa, NULL);

	// loop forever (we need ^C to quit)
	while (1) {}

	return 0;
}
