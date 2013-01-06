/* transform.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_TRANSFORM_H
#define VRT_TRANSFORM_H

#include "progscope.h"

/* wishlist/in progress */
void hapticNormill(select_t *);
int intersection(select_t *);
void join (select_t *);
void recycler(select_t *);
void hmapwrapf(select_t *);
int hmapunwrapf(select_t *);
int alloc_dialog(select_t *);
void copy_hmapf(select_t *);

#endif /* VRT_TRANSFORM_H */
