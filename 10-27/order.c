#include <stdio.h>
#include <unistd.h>

int main() {

	/* fork() combined with conditionals. */

	printf("begin\n");
	
	if (fork()) {
		printf("mid\n");
		fork();
	}
	printf("end\n");

	/* If we want to know which process is which, use this instead: */
	//printf("end %ld\n", getpid());

	/* What happens in this problem?  Drawing a process graph is 
	 * the key to knowing what happens:
	 *
	 *                                   end
	 *             +--------------------->o
	 *             |                    printf
	 *             |
	 *             |                     end           
	 *             |             +------->o
	 *             |             |      printf
	 *             |             |
	 *    begin    |     mid     |       end     
	 * ---->o----->o----->o----->o------->o
	 *    printf fork  printf  fork     printf
	 *
	 *    Note that the order of the "mid" and the first "end" is 
	 *    determined partly by chance, because we can't control the
	 *    relative speed at which two processes execute.  However,
	 *    the "mid" must come before the last two "end" outputs.
	 */

	return 0;
}
