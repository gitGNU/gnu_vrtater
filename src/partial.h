/* partial.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_PARTIAL_H
#define VRT_PARTIAL_H

#include "hmap.h"
#include "loginkeys.h"

struct ptlmap_s {
	hmapf_t *map;
	struct ptlmap_s *precursor;
};
typedef struct ptlmap_s ptlmap_t;

struct ptlmaps_list_s {
	session_t *session;
	ptlmap_t *last;
	unsigned int count;
};
typedef struct ptlmaps_list_s ptlmaps_list_t;

struct ptlmbr_s {
	session_t sign_in;
	struct ptlmbr_s *precursor;
};
typedef struct ptlmbr_s ptlmbr_t;

struct ptlmbrs_list_s {
	ptlmbr_t *last;
	unsigned int count;
};
typedef struct ptlmbrs_list_s ptlmbrs_list_t;

struct ptlgrp_s {
	session_t map_name; /* name of groupmap for this group */
	ptlmbrs_list_t *members; /* linked list of */
	char *passwd; /* if non NULL password is supported */
	struct ptlgrp_s *precursor;
};
typedef struct ptlgrp_s ptlgrp_t;

struct ptlgrps_list_s {
	session_t *session;
	ptlgrp_t *last;
	unsigned int count;
};
typedef struct ptlgrps_list_s ptlgrps_list_t;

struct partial {
	session_t session; /* maps from node-orgin are masked into session */
	ptlmaps_list_t *ptlmaps;
	ptlgrps_list_t *ptlgrps;
	struct ptlrepute_list *reputed;
	hmapf_t *nodemap; /* hmap describing the volume of given partial */
	btoggles_t ptlbits;
	struct partial *precursor;
};

/* partial_t ptlbits. */
enum {
	VRT_ORDINAL_UNFORMED, /* not in session code's formed set */
#define VRT_MASK_UNFORMED (1 << VRT_ORDINAL_UNFORMED)
	VRT_ORDINAL_FLEXIBLE,
#define VRT_MASK_FLEXIBLE (1 << VRT_ORDINAL_FLEXIBLE)
	VRT_ORDINAL_REQUIRE_PASSWD,
#define VRT_MASK_REQUIRE_PASSWD (1 << VRT_ORDINAL_REQUIRE_PASSWD)
};

struct partial_list {
	struct partial *last;
	unsigned int count;
};

struct partial_list *partials; /* list of all partials */

struct partial *find_partial(session_t *session);
int select_partial_set(ptlmaps_list_t *list, hmapf_t **maps);
ptlmaps_list_t *mk_ptlmaps_list(session_t *session);
void rm_ptlmaps_list(ptlmaps_list_t *);
ptlmap_t *add_ptlmap(ptlmaps_list_t *, hmapf_t *);
void subtract_ptlmap(ptlmaps_list_t *, hmapf_t *);

ptlmbr_t *find_member(ptlmbrs_list_t *list, session_t *sign_in);
ptlmbrs_list_t *mk_ptlmbrs_list(ptlgrp_t *group);
void rm_ptlmbrs_list(ptlmbrs_list_t *);
ptlmbr_t *add_ptlmbr(ptlmbrs_list_t *, session_t *sign_in);
void subtract_ptlmbr(ptlmbrs_list_t *, ptlmbr_t *);

ptlgrp_t *find_group(ptlgrps_list_t *list, session_t *grpmap_name);
void sync_groups(ptlgrps_list_t *list, session_t *last, session_t *sign_in);
ptlgrps_list_t *mk_ptlgrps_list(session_t *session);
void rm_ptlgrps_list(ptlgrps_list_t *);
ptlgrp_t *add_ptlgrp(ptlgrps_list_t *, session_t *groupmap_name);
void subtract_ptlgrp(ptlgrps_list_t *, ptlgrp_t *);

#endif /* VRT_PARTIAL_H */
