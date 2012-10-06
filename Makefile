CFLAGS=-Wall -g -pthread
CC=gcc
EXECUTABLE=threader
PREFIX=/usr/local
BIN=$(PREFIX)/bin

.PHONY : all
all:
	@echo "MAKING ALL..."
	$(CC) $(CFLAGS) -c threader.c config.c logger/logger.c logger/string_allocation.c
	$(CC) $(CFLAGS) threader.o config.o logger.o string_allocation.o -o $(EXECUTABLE)

.PHONY : clean
clean:
	@echo "MAKE CLEANING..."
	rm -f $(EXECUTABLE)
	rm -f threader.o
	rm -f config.o
	rm -f logger.o
	rm -f string_allo

.PHONY : install-config
install-config:
	@echo "INSTALLING CONFIG FOLDER..."
	rm -rf .threader
	rm -rf ~/.threader
	cp -r config .threader
	mv -f .threader ~/

.PHONY : install
install: all
	@echo "INSTALLING..."
	cp -r config .threader
	mv .threader ~/
	install -m 755 $(EXECUTABLE) $(BIN)/$(EXECUTABLE)
	@echo "NOTE: Make sure that you have run `make install-config` too, it will create a config folder named ~/.threader. The program won't be able to start without that folder."

.PHONY : uninstall
uninstall:
	@echo "UNINSTALLING..."
	rm $(BIN)/$(EXECUTABLE)
	@echo "NOTE: For a full uninstall remember to delete the ~/.threader config folder if it exists."

