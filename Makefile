CFLAGS=-Wall -g -pthread
CC=gcc
EXECUTABLE=flac2mp3

.PHONY : all
all:
	$(CC) $(CFLAGS) -c js_logger.c flac2mp3.c string_allocation.c config.c
	$(CC) $(CFLAGS) js_logger.o flac2mp3.o string_allocation.o config.o -o $(EXECUTABLE)

.PHONY : clean
clean:
	rm -f $(EXECUTABLE)
	rm -f flac2mp3.o
	rm -f js_logger.o
	rm -f string_allocation.o
	rm -f config.o
