# Autoconf config for vrtater.
# Copyright (C) 2013 J. A. Green <green8@sdf-eu.org>
# license: GNU GPL v3, see COPYING, otherwise see vrtater.c

AC_INIT([vrtater], [0.3.65], [green8@sdf-eu.org])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
CC="gcc"
CFLAGS="-Wall -pedantic -std=gnu99 -fno-builtin"
LIBS="$LIBS -lGL -lGLU -lXi -lXmu -lXext -lX11 -lm"
AC_CONFIG_FILES([
  Makefile
  src/Makefile
  doc/Makefile
])
AC_OUTPUT
