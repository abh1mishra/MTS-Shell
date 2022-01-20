all : execdemo multi ctrlc

clean :
	rm -f execdemo multi ctrlc 

# A demo of running a new program by forking a process and using exec()

execdemo : execdemo.c
	gcc -o execdemo execdemo.c

# What happens when we fork() many children and wait for each to finish?

multi : multi.c
	gcc -o multi multi.c

# Using a signal handler to intercept Ctrl-C presses

ctrlc : ctrlc.c
	gcc -o ctrlc ctrlc.c

