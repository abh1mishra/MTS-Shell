/* Do not modify this file */
#ifndef UTIL_H
#define UTIL_H

#include <unistd.h>  // read(), write()
#include <string.h>  // strlen()

/* reads all available data from the fd, and returns the result as a malloc'd string; NULL on failure  */
char *fd_to_text(int fd);

/* outputs all of the available string data into the fd; returns the number of characters written */
size_t text_to_fd(const char *data, int fd);

#endif /*UTIL_H*/
