/* session.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_SESSION_H
#define VRT_SESSION_H

#include "hmap.h"
#include "partial.h"

struct session_desc_s {
	session_t session;
	btoggles_t level;
	char oneliner[81];
	complextimate_t complexity;
	session_t behind;
	struct session_desc_s *precursor;
};
typedef struct session_desc_s session_desc_t;

enum { /* session_desc_t level */
	VRT_ORDINAL_SESSION_CALLED,
#define VRT_MASK_SESSION_CALLED (1 << VRT_ORDINAL_SESSION_CALLED)
	VRT_ORDINAL_SESSION_CUED,
#define VRT_MASK_SESSION_CUED (1 << VRT_ORDINAL_SESSION_CUED)
	VRT_ORDINAL_SESSION_PARTIAL,
#define VRT_MASK_SESSION_PARTIAL (1 << VRT_ORDINAL_SESSION_PARTIAL)
	VRT_ORDINAL_SESSION_INBOUND
#define VRT_MASK_SESSION_INBOUND (1 << VRT_ORDINAL_SESSION_INBOUND)
};

struct session_desc_list_s {
	session_desc_t *last;
	unsigned int count;
};
typedef struct session_desc_list_s session_desc_list_t;

session_desc_list_t *all_sessions;
int answer_accept;

void init_sessions(void);
int hash_session_name(session_t *, char *seedfiles);
void list_nodes(char *);
int call_session(char *url, complextimate_t *, hmapf_t **maps);
int form_determined_session(partial_t *, session_t *last, session_t *new, char *url);
int form_flexible_session(session_t *);
void sync_sessions(void);
int buffer_maps_to_peer_partial(session_t *, select_t *sel);
hmapf_t *recieve_maps_from_peer_partial(session_t *, select_t *sel);
session_desc_t *find_in_all_sessions(session_t *);
void mk_session_desc_list(void);
int close_session(session_desc_t *);
int reset_sessions(void);
/* Diagnostics. */
void diag_ls_partial_sessions(int full);

#endif /* VRT_SESSION_H */
