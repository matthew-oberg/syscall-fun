CC = cc
CFLAGS = -Wall -Werror -g -O0 -std=c99 -pedantic

SRC = runner.c
EXE = runner.o

.PHONY: clean all run

all: clean build

build:
	$(CC) $(SRC) $(CFLAGS) -o $(EXE)

clean:
	-rm -f *.o

