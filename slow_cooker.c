/* A sample program provided as local executable.
 * - Once started, this program will slowly print out a count-down
 *   until the counter reaches zero.
 * - The default initial counter value is 10.
 * - Optionally the user can specify the initial counter value.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define N 10

int main(int argc, char *argv[]){
   int counter;

   if (argc==1)
	counter = N;
   else
	counter = atoi(argv[1]);
	
   while (counter>=0){
  	printf("slow_cooker count down: %d ...\n", counter);
 	fflush(stdout);
	counter--;
	sleep(1);
   }

   return 0;
}
