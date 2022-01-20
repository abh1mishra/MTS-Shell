all : s_cat mult order mem

clean :
	rm -f s_cat mult order mem 

# Schroedinger's cat simulation using fork()
# note: we don't want to confuse this with the Unix cat command

s_cat : cat.c
	gcc -o s_cat cat.c

# What happens when we fork() several times?

mult : mult.c
	gcc -o mult mult.c

# Investigating the sequence of events when multiple processes are running

order : order.c
	gcc -o order order.c

# Finding out what happens to variables and memory after a fork()

mem : mem.c
	gcc -o mem mem.c
