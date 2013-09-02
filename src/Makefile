# Makefile: Initial, extra vanilla, for vrtater.
# Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
# license: GNU GPL v3, see COPYING, otherwise see vrtater.c

# For now, build directory and source directory are the same, ie: src/.
SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c

CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -fno-builtin
LIBS = -lGL -lGLU -lXi -lXmu -lXext -lX11 -lm
INCLUDE =

BIN = vrtater
MAIN = vrtater
SRCS = ifnodeglx.c vectors.c rotation.c generator.c session.c dialogX11.c attribs.c tug.c transform.c stock.c rendergl.c hmap.c bittoggles.c partial.c

j : clean all

all : $(BIN)

clean :
	rm -f $(BIN)

run : $(BIN)
	./$(BIN)

$(BIN) : $(MAIN).c
	$(CC) $(SRCS) $(INCLUDE) -o $@ $(CFLAGS) $< $(LIBS)
