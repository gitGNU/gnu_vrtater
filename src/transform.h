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

/* Options to hmap transform functions affecting ip network or filesystem. */
enum {
	VRT_ORDINAL_OPT_SESSION_NAME,
#define VRT_MASK_OPT_SESSION_NAME (1 << VRT_ORDINAL_OPT_SESSION_NAME)
	VRT_ORDINAL_OPT_COMPOUNDED,
#define VRT_MASK_OPT_COMPOUNDED (1 << VRT_ORDINAL_OPT_COMPOUNDED)
	VRT_ORDINAL_OPT_MINIMAL,
#define VRT_MASK_OPT_MINIMAL (1 << VRT_ORDINAL_OPT_MINIMAL)
	VRT_ORDINAL_OPT_UPDATE,
#define VRT_MASK_OPT_UPDATE (1 << VRT_ORDINAL_OPT_UPDATE)
	VRT_ORDINAL_OPT_FULL,
#define VRT_MASK_OPT_FULL (1 << VRT_ORDINAL_OPT_FULL)
	VRT_ORDINAL_OPT_COMPRESS,
#define VRT_MASK_OPT_COMPRESS (1 << VRT_ORDINAL_COMPRESS)
	VRT_ORDINAL_OPT_ENCRYPT
#define VRT_MASK_OPT_ENCRYPT (1 << VRT_ORDINAL_OPT_ENCRYPT)
};

int join_hmaps(select_t *);
int intersection(select_t *);
int group_hmaps(select_t *);
int extend_hmaps(select_t *);
int recycle(select_t *);
int cp_hmapf(select_t *);
int surface_inv_hmapf(select_t *);
int hmapwrapf(select_t *);
int hmapunwrapf(select_t *);
int alloc_dialog(select_t *, int len);
int add_dialog(select_t *, char *s, int count, int offset);
int write_dialog(select_t *, char *s);

#endif /* VRT_TRANSFORM_H */
