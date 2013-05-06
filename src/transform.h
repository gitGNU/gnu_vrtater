/* transform.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_TRANSFORM_H
#define VRT_TRANSFORM_H

int hapticNormill(select_t *);
int intersection(select_t *);
int group_hmaps(select_t *);
int join(select_t *);
int recycler(select_t *);
int hmapwrapf(select_t *);
int hmapunwrapf(select_t *);
int alloc_dialog(select_t *);
int cp_hmapf(select_t *);

#endif /* VRT_TRANSFORM_H */
