CC=gcc
CFLAGS=-l.

simulator: simulator.o
	$(CC) -o simulator simulator.o -lpthread

run:
	./simulator

clean:
	rm simulator.o simulator
