/* generator.h 
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_GENERATOR_H
#define VRT_GENERATOR_H

#include "bittoggles.h"
#include "vectors.h"
#include "hmap.h"
#include "partial.h"

/* These generator options are all set by ifnode**.c, some through data
   maintained in dialog.c.  balance_criteria is still in the works, and may be
   proposed as some hybrid of bits vs. a quantity once needed.
   vobspace_criteria bit defines are still in the works, are listed below. */
struct genopts_s {
	btoggles_t balance_criteria; /* balance_filter options */
	btoggles_t vobspace_criteria; /* context options */
	int why; /* fail, ... */
	int what; /* shutdown, ... */
	double when; /* shutdown, ... */
};
typedef struct genopts_s genopts_t;

/* genopts_t vobspace_criteria.  Effective while held high, some of these
   where noted, will be cleared after use. */
enum {
	VRT_ORDINAL_SHUTDOWN,
#define VRT_MASK_SHUTDOWN (1 << VRT_ORDINAL_SHUTDOWN)
	VRT_ORDINAL_DASHF,
#define VRT_MASK_DASHF (1 << VRT_ORDINAL_DASHF)
};

int generate_node_orgin(void);
void regenerate_scene(vf_t *);
partial_t *mk_partial(char *desc, hmapf_t *map);
void rm_partial(partial_t *partial);
void close_vobspace(double time_till_closed);
void close_node_orgin(void);
int resize_node_orgin(int, int);
/* Diagnostics. */
void diag_ls_partials(int full);
int diag_hmaps_in_partial(session_t *partial);
void diag_generator_key_f(void);
void diag_generator_key_g(void);
void diag_generator_key_h(void);
/* Supported calls for bus input. */
void generator_join_hmaps(void);
void generator_intersection(void);

#endif /* VRT_GENERATOR_H */
