CFLAGS=-Wall -g -pthread
CC=gcc
EXECUTABLE=threader

.PHONY : all
all:
	$(CC) $(CFLAGS) -c threader.c config.c logger/logger.c logger/string_allocation.c
	$(CC) $(CFLAGS) threader.o config.o logger.o string_allocation.o -o $(EXECUTABLE)

.PHONY : clean
clean:
	rm -f $(EXECUTABLE)
	rm -f threader.o
	rm -f config.o
	rm -f logger.o
	rm -f string_allocation.o
