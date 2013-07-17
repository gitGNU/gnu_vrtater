/* generator.h 
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_GENERATOR_H
#define VRT_GENERATOR_H

#include "bittoggles.h"
#include "vectors.h"
#include "hmap.h"

/* generator_opts_t
   these are all set by ifnode**.c, some through data maintained in dialog.c
   balance_criteria is still in the works, and may be proposed as some hybrid
   of bits vs. a quantity once needed.  vobspace_criteria, still in the works
   is listed below */
struct generator_opts {
	btoggles_t balance_criteria; /* balance_filter options */
	btoggles_t vobspace_criteria; /* context options */
	int why; /* fail, ... */
	int what; /* shutdown, ... */
	double when; /* shutdown, ... */
} genopts;

/* gen_opts_t vobspace_criteria.  effective while held high, some of these
   where noted, will be cleared after use */
enum {
	VRT_ORDINAL_SHUTDOWN,
#define VRT_MASK_SHUTDOWN (1 << VRT_ORDINAL_SHUTDOWN)
	VRT_ORDINAL_DASHF,
#define VRT_MASK_DASHF (1 << VRT_ORDINAL_DASHF)
};

/* partial */
struct partial_space {
	session_t session; /* entering from in-node, hmaps take this name */
	char *desc; /* set when calling mk_partial() */
	hmapf_t *nodemap; /* hmap describing the volume of given partial */
	hmapf_t *selection; /* NULL terminated ref list for this partial */
};
typedef struct partial_space partial_t;

int generate_node(void);
void regenerate_scene(vf_t *);
partial_t *mk_partial(char *desc, hmapf_t *map);
void rm_partial(partial_t *partial);
int diag_hmaps_in_partial(session_t *partial_session);
session_t *diag_partial_by_ordinal(unsigned int idx);
void diag_ls_partials(int full);
void close_vobspace(double time_till_closed);
void close_node(void);
int resize_node(int, int);
/* diag */
void diag_generator_key_f(void);
void diag_generator_key_g(void);
void diag_generator_key_h(void);
/* generator supported calls for tug input */
void generator_hapticNormill(void);
void generator_intersection(void);

#endif /* VRT_GENERATOR_H */
