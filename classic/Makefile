CC = cc
CFLAGS = -Wall -Werror -g -O0 -std=c99 -pedantic

SRC = streamer.c
EXE = streamer.o

.PHONY: clean all run

all: clean build

build:
	$(CC) $(SRC) $(CFLAGS) -o $(EXE)

clean:
	-rm -f *.o

