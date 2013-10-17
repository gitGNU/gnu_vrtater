/* partial.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_PARTIAL_H
#define VRT_PARTIAL_H

#include "hmap.h"

#define VRT_MAPKEY 0
#define VRT_MAPKEYBKP 1

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

struct ptlrep_s {
	char *url; /* called node-partial */
	session_t key; /* most recent */
	session_t bkpkey; /* 2nd most recent */
	session_t holdkey; /* last held session */
	char *passwd; /* NULL till supported, see: session.c */
	struct ptlrep_s *precursor;
};
typedef struct ptlrep_s ptlrep_t;

struct ptlreps_list_s {
	session_t *session;
	ptlrep_t *last;
	unsigned int count;
};
typedef struct ptlreps_list_s ptlreps_list_t;

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

struct partial_s {
	session_t session; /* maps from node-orgin are masked into session */
	ptlmaps_list_t *ptlmaps;
	ptlgrps_list_t *ptlgrps;
	ptlreps_list_t *ptlreps;
	char *desc; /* set when calling mk_partial */
	hmapf_t *nodemap; /* hmap describing the volume of given partial */
	complextimate_t complexity;
	int running; /* is in session code's running set */
};
typedef struct partial_s partial_t;

partial_t *partial_list;
int partials_count;

partial_t *find_partial(session_t *partial_session);
int select_partial_set(ptlmaps_list_t *list, hmapf_t **maps);
ptlmaps_list_t *mk_ptlmaps_list(session_t *partial_session);
void rm_ptlmaps_list(ptlmaps_list_t *);
ptlmap_t *add_ptlmap(ptlmaps_list_t *, hmapf_t *);
void subtract_ptlmap(ptlmaps_list_t *, hmapf_t *);

ptlrep_t *find_repute(ptlreps_list_t *list, session_t *keyname, int flag);
int sync_reputation(ptlreps_list_t *list, session_t *last, session_t *new, char *url);
ptlreps_list_t *mk_ptlreps_list(session_t *partial_session);
void rm_ptlreps_list(ptlreps_list_t *);
ptlrep_t *add_ptlrep(ptlreps_list_t *, session_t *keyname, char *url);
void subtract_ptlrep(ptlreps_list_t *, ptlrep_t *);

ptlmbr_t *find_member(ptlmbrs_list_t *list, session_t *sign_in);
ptlmbrs_list_t *mk_ptlmbrs_list(ptlgrp_t *group);
void rm_ptlmbrs_list(ptlmbrs_list_t *);
ptlmbr_t *add_ptlmbr(ptlmbrs_list_t *, session_t *sign_in);
void subtract_ptlmbr(ptlmbrs_list_t *, ptlmbr_t *);

ptlgrp_t *find_group(ptlgrps_list_t *list, session_t *grpmap_name);
void sync_groups(ptlgrps_list_t *list, session_t *last, session_t *sign_in);
ptlgrps_list_t *mk_ptlgrps_list(session_t *partial_session);
void rm_ptlgrps_list(ptlgrps_list_t *);
ptlgrp_t *add_ptlgrp(ptlgrps_list_t *, session_t *groupmap_name);
void subtract_ptlgrp(ptlgrps_list_t *, ptlgrp_t *);

#endif /* VRT_PARTIAL_H */
