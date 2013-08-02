/* transform.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_TRANSFORM_H
#define VRT_TRANSFORM_H

#include "hmap.h"

/* meta_u */
union meta_unit_u {
	char c;
	int i;
	float f;
};
typedef union meta_unit_u meta_u;

int join_hmaps(select_t *);
int intersection(select_t *);
int group_hmaps(select_t *);
int extend_hmaps(select_t *);
int recycle(select_t *);
int hmapwrapf(select_t *);
int hmapunwrapf(select_t *);
int cp_hmapf(select_t *);
int surface_inv_hmapf(select_t *);
int alloc_dialog(select_t *, int len);
int add_dialog(select_t *, char *s, int count, int offset);
int write_dialog(select_t *, char *s);

#endif /* VRT_TRANSFORM_H */
