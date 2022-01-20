/* A sample program provided as local executable.
 * - Once started, this program need to receive SIGINT N times 
 *   to complete.
 * - Each time a SIGINT is received, it will print a hello msg.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <signal.h>
#include <string.h>

#define N 3
int count = 0;

void sigint_handler(int sig){
    printf("hello %d from my_pause!\n", ++count);
    fflush(stdout); 
}

int main(){
   struct sigaction new;
   memset(&new, 0, sizeof(struct sigaction));
   new.sa_handler = sigint_handler;
   sigaction(SIGINT, &new, NULL);

   while (count<N)
	  pause();

   return 0;
}
