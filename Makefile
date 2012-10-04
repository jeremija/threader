CFLAGS=-Wall -g -pthread
CC=gcc
EXECUTABLE=threader

.PHONY : all
all:
	$(CC) $(CFLAGS) -c lib/logger/logger.c threader.c lib/logger/string_allocation.c config.c
	$(CC) $(CFLAGS) logger.o threader.o string_allocation.o config.o -o $(EXECUTABLE)

.PHONY : clean
clean:
	rm -f $(EXECUTABLE)
	rm -f threader.o
	rm -f logger.o
	rm -f string_allocation.o
	rm -f config.o
