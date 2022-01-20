/* Do not modify this file */

#ifndef TEXTPROC_H
#define TEXTPROC_H

#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdbool.h>
#include<errno.h>
#include<signal.h>

#include "logging.h"

/* Constants */
#define MAXLINE 100 /* the max number of chars in one command line */
#define MAXARGS 25 /* the max number of arguments for one program */

#endif /*TEXTPROC_H*/
