/* Do Not Modify This File */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>

#include "util.h"

#define READ_SIZE 1000
#define MAX_READ_SIZE 100000

typedef struct slice_s {
    char b[READ_SIZE];
    struct slice_s *next;
} Slice;


static size_t fill_buffer(int fd, void *buf, size_t size) {
    size_t remaining = size;
    size_t len = 0;
    ssize_t read_len = 0;
    do {
        read_len = read(fd, buf, size);
	if (read_len < 0) {
	    if (errno = EINTR) continue;
	    else return len;
	}
	remaining -= read_len;
	buf += read_len;
	len += read_len;

    } while (read_len && remaining);
    return len;
}

void free_slices(Slice *head) {
    Slice *current = head;
    while (current) {
        Slice *temp = current;
	current = current->next;
	free(temp);
    }
}

char *fd_to_text(int fd) {
    Slice *head = NULL;
    Slice **current = &head;
    size_t len = 0, read_len = 0;

    do {
        *current = malloc(sizeof(Slice));
	if (*current) {
	    (*current)->next = NULL;
            read_len = fill_buffer(fd, (*current)->b, READ_SIZE);
	} else { read_len = 0; }
	len += read_len;
    } while (len < MAX_READ_SIZE && read_len == READ_SIZE);

    char *str = malloc(len+1);
    if (str) {
	Slice *sptr = head;
	char *bptr = str;
        str[len] = 0;
	while (sptr && len) {
	    if (len < READ_SIZE) { 
	        strncpy(bptr, sptr->b, len);
                bptr += len;
		len = 0;
	    } else {
                strncpy(bptr, sptr->b, READ_SIZE);
		bptr += READ_SIZE;
		len -= READ_SIZE;
	    }
	}

    }
    free_slices(head);
    return str;
}

size_t text_to_fd(const char *data, int fd) {
    size_t remaining = strlen(data);
    size_t len = 0;
    ssize_t wr_len = 1;
    while (remaining && wr_len) {
        wr_len = write(fd, data, remaining);
	if (wr_len < 0) {
	    if (errno == EINTR) continue;
	    else return len;
	}
	remaining -= wr_len;
	data += wr_len;
	len += wr_len;
    }
    return len;
}
