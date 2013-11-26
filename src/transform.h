/* transform.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_TRANSFORM_H
#define VRT_TRANSFORM_H

#include "hmap.h"
#include "bittoggles.h"

/* Options to hmap transform functions affecting ip network or filesystem. */
enum {
	VRT_ORDINAL_OPT_NULL_SESSION_NAME,
#define VRT_MASK_OPT_NULL_SESSION_NAME (1 << VRT_ORDINAL_OPT_NULL_SESSION_NAME)
	VRT_ORDINAL_OPT_COMPOUNDED,
#define VRT_MASK_OPT_COMPOUNDED (1 << VRT_ORDINAL_OPT_COMPOUNDED)
	VRT_ORDINAL_OPT_MINIMAL,
#define VRT_MASK_OPT_MINIMAL (1 << VRT_ORDINAL_OPT_MINIMAL)
	VRT_ORDINAL_OPT_UPDATE,
#define VRT_MASK_OPT_UPDATE (1 << VRT_ORDINAL_OPT_UPDATE)
	VRT_ORDINAL_OPT_COMPRESSED,
#define VRT_MASK_OPT_COMPRESSED (1 << VRT_ORDINAL_OPT_COMPRESSED)
	VRT_ORDINAL_OPT_ENCRYPTED
#define VRT_MASK_OPT_ENCRYPTED (1 << VRT_ORDINAL_OPT_ENCRYPTED)
};

int join_hmaps(select_t *);
int unjoin_hmaps(select_t *);
int extend_hmaps(select_t *);
int add_dialog_members(select_t *);
int subtract_dialog_members(select_t *);
int swap_hmaps(select_t *);
int scale_hmap_c(select_t *);
int bifold_hmap_c(select_t *);
int intersection(select_t *);
int recycle(select_t *);
int cp_hmapf(select_t *);
int surface_inv_hmapf(select_t *);
int hmapwrapf(select_t *, btoggles_t options, char *filename, int **output);
int hmapunwrapf(select_t *, session_t *, char *filename, int *input);
int alloc_dialog(hmapf_t *, int len);
int add_dialog(hmapf_t *, char *s, int offset, int count);
int write_dialog(hmapf_t *, char *s);

#endif /* VRT_TRANSFORM_H */
