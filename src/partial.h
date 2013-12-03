/* partial.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef PARTIAL_H
#define PARTIAL_H

#include "hmap.h"
#include "loginkeys.h"

struct ptlmap {
	hmapf_t *map;
	struct ptlmap *precursor;
};

struct ptlmap_list {
	session_t *session;
	struct ptlmap *last;
	unsigned int count;
};

struct modlg {
	session_t sign_in;
	struct modlg *precursor;
};
typedef struct modlg modlg_t;

struct modlg_list {
	struct modlg *last;
	unsigned int count;
};

struct ptlgrp {
	session_t groupmap;
	struct modlg_list *holdmaps; /* linked list of sign-in's from holdbkp */
	char *passwd; /* for admin of ptlgrp. */
	struct ptlgrp *precursor;
};
typedef struct ptlgrp ptlgrp_t;

struct ptlgrps_list {
	session_t *session;
	struct ptlgrp *last;
	unsigned int count;
};

struct partial {
	session_t session; /* maps from node-orgin are masked into session */
	struct ptlmap_list *ptlmaps;
	struct ptlgrps_list *ptlgrps;
	struct ptlrepute_list *reputed;
	hmapf_t *nodemap; /* hmap describing partial volumetrically */
	int ptlbits;
	struct partial *precursor;
};

/* partial_t ptlbits. */
enum {
	VRT_MASK_UNFORMED,
#define VRT_MASK_UNFORMED (1 << VRT_ORDINAL_UNFORMED)
	VRT_ORDINAL_FLEXIBLE,
#define VRT_MASK_FLEXIBLE (1 << VRT_ORDINAL_FLEXIBLE)
	VRT_ORDINAL_REQUIRE_PASSWD
#define VRT_MASK_REQUIRE_PASSWD (1 << VRT_ORDINAL_REQUIRE_PASSWD)
};

struct partial_list {
	struct partial *last;
	unsigned int count;
};

struct partial_list *partials; /* list of all partials */

struct partial *find_partial(session_t *session);
int select_partial_set(struct ptlmap_list *list, hmapf_t **maps);
struct ptlmap_list *mk_ptlmap_list(session_t *session);
void rm_ptlmap_list(struct ptlmap_list *);
struct ptlmap *add_ptlmap(struct ptlmap_list *, hmapf_t *);
void subtract_ptlmap(struct ptlmap_list *, hmapf_t *);

struct modlg *find_member(struct modlg_list *list, session_t *sign_in);
struct modlg_list *mk_modlg_list(struct ptlgrp *group);
void rm_modlg_list(struct modlg_list *);
struct modlg *add_modlg(struct modlg_list *, session_t *sign_in);
void subtract_modlg(struct modlg_list *, struct modlg *);

struct ptlgrp *find_group(struct ptlgrps_list *list, session_t *mapname);
void sync_groups(struct ptlgrps_list *list, session_t *last, session_t *sign_in);
struct ptlgrps_list *mk_ptlgrps_list(session_t *session);
void rm_ptlgrps_list(struct ptlgrps_list *);
struct ptlgrp *add_ptlgrp(struct ptlgrps_list *, session_t *mapname);
void subtract_ptlgrp(struct ptlgrps_list *, struct ptlgrp *);

#endif /* PARTIAL_H */
