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
	session_t peer;
	session_t from; /* for forwarding when supported, else proxy */
	btoggles_t level;
	char oneliner[81];
	complextimate_t complexity;
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
int call_session(char *url, complextimate_t *, hmapf_t **maps, session_t *lsession);
int form_continuing_session(char *url, partial_t *forming, session_t *loginkey, ptlrep_t *repute);
int answer_session(session_t *session); /* here for testing */ 
void sync_sessions(void);
int buffer_maps_to_peer_partial(session_t *, select_t *sel);
hmapf_t *receive_map_from_peer_partial(session_t *, select_t *sel); /* here for testing */
session_desc_t *find_in_all_sessions(session_t *);
void mk_session_desc_list(void);
session_desc_t *add_session_desc(session_t *local, session_t *peer, session_t *from, btoggles_t *level, char *briefdesc, complextimate_t *);
int close_session(session_desc_t *);
int close_all_sessions(void);
int reset_sessions(void);
/* Diagnostics. */
void diag_ls_partial_sessions(int full);

#endif /* VRT_SESSION_H */
