CFLAGS=-Wall -g -pthread
CC=gcc
EXECUTABLE=flac2mp3

.PHONY : all
all:
	$(CC) $(CFLAGS) -c js_logger.c flac2mp3.c
	$(CC) $(CFLAGS) js_logger.o flac2mp3.o -o $(EXECUTABLE)

.PHONY : clean
clean:
	rm -f $(EXECUTABLE)
	rm -f flac2mp3.o
	rm -f js_logger.o
