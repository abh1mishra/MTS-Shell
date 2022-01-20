all: textproc my_pause slow_cooker my_echo

textproc: textproc.o logging.o parse.o util.o
	gcc -Wall -std=gnu11 -o textproc textproc.o logging.o parse.o util.o

textproc.o: textproc.c textproc.h
	gcc -Wall -g -std=gnu11 -c textproc.c   
#	gcc -D_POSIX_C_SOURCE -Wall -g -std=c99 -c textproc.c   

parse.o: parse.c parse.h
	gcc -Wall -g -std=c99 -c parse.c     

util.o: util.c util.h
	gcc -Wall -g -std=c99 -c util.c     

logging.o: logging.c logging.h
	gcc -Wall -Wformat-truncation=0 -g -std=c99 -c logging.c     

my_pause: my_pause.c
	gcc -D_POSIX_C_SOURCE -Wall -Og -std=c99 -o my_pause my_pause.c

slow_cooker: slow_cooker.c
	gcc -D_POSIX_C_SOURCE -Wall -Og -std=c99 -o slow_cooker slow_cooker.c

my_echo: my_echo.c
	gcc -D_POSIX_C_SOURCE -Wall -Og -std=c99 -o my_echo my_echo.c

clean:
	rm -rf textproc.o logging.o parse.o util.o textproc my_pause slow_cooker my_echo




