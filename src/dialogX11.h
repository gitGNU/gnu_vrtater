/* dialogX11.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_DIALOGX11_H
#define VRT_DIALOGX11_H

#include "hmap.h"

void dialog(select_t *);
int write_dialog(select_t *, char *);
int append_dialog(select_t *, char *);

#endif /* VRT_DIALOGX11_H */
