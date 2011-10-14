CC=gcc

INCLUDES=

CFLAGS=-std=c99 -g -pedantic -Wall
CFLAGS+=$(shell pkg-config --cflags clutter-1.0)
CFLAGS+=$(shell pkg-config --cflags clutter-x11-1.0)

LDFLAGS=$(shell pkg-config --libs clutter-1.0)
LDFLAGS+=$(shell pkg-config --libs clutter-x11-1.0)

OBJS=window.o morsetree.o key_X11.o main.o
BINARYNAME=test

all: main

clean:
	rm -f $(BINARYNAME) $(OBJS)

main: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(BINARYNAME) $(OBJS)

main.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c main.c

.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c
