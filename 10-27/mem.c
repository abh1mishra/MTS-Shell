#include <stdio.h>
#include <unistd.h>

/* Analyzing the effect of forking on variables and memory addresses */

int main() {
	 /* Two local varaibles: */

	int i = 10;  // an int, which we assign a value to 
	int *p = &i; // a pointer to that int

	/* Now, let's fork and see what happens. */
	fork();

	/* Scenario 1: the values of the variable in each process. */
	//printf("i = %d\n", i);

	/* Scenario 2: each process incrememts its i. */
	// i++;
	//printf("i = %d\n", i);
	
	/* Scenario 3: each process increments the value referenced by the
	 * original process's pointer (pointing to the original process's i. */
	(*p)++;
	printf("i = %d\n", i);
	//printf("i = %d\n", *p);

	/* Analysis: a fork() creates an identical copy of the process,
	 * so all variables and code are the same in each.  Thus, in
	 * Scenario 1, both processes would print 10.
	 *
	 * In Scenario 2, since each process has an independent copy of the
	 * data from the point of the fork(), each process increments its
	 * own version.  Thus, both processes would print 11.  Note that
	 * the copy may be performed as a lazy-copy: it may defer the copy,
	 * and use the original data in a read-only way until the moment
	 * that it needs to be changed, and only then perform a copy.  The
	 * program wouldn't know the difference. 
	 *
	 * In Scenario 3, we are updating the variable which is pointed
	 * to by the original pointer.  At first thought, we might think that
	 * we're updating the first process's i value twice.  This is not
	 * what happens - we once again get two 11's.  How come, when the
	 * pointer value isn't changed?  Because not only does each process
	 * get its own version of i, it also gets its own *private address
	 * space*.  The same pointer value in each process points to a 
	 * completely different memory address, in each case representing i.
	 * This is done with the help of virtual memory hardware, which we
	 * will learn about later in the course.
	 */

	return 0;
}
