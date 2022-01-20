/* A sample program provided as local executable.
 * - This program takes an integer argument and use it 
 *      as the return value / exit status.
 * - The default return value / exit status is 0.
 */

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
   int answer = 0;

   if (argc>1)
	    answer = atoi(argv[1]);

   printf("my answer is %d\n", answer);  
   return answer;
}
