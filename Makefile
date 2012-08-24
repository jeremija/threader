CFLAGS=-Wall -g -pthread
CC=gcc
EXECUTABLE=threader

.PHONY : all
all:
	$(CC) $(CFLAGS) -c js_logger.c threader.c string_allocation.c config.c
	$(CC) $(CFLAGS) js_logger.o threader.o string_allocation.o config.o -o $(EXECUTABLE)

.PHONY : clean
clean:
	rm -f $(EXECUTABLE)
	rm -f threader.o
	rm -f js_logger.o
	rm -f string_allocation.o
	rm -f config.o
