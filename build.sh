#!/bin/sh
CC=gcc
SOURCES="window.c morsetree.c key_X11.c main.c"
CFLAGS="`pkg-config --cflags clutter-1.0` `pkg-config --cflags clutter-x11-1.0`"
LDFLAGS="`pkg-config --libs clutter-1.0` `pkg-config --libs clutter-x11-1.0`"
$CC -std=c99 -pedantic -g $CFLAGS $LDFLAGS $SOURCES -o test
