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
	char adesc[81]; /* if called, cued */
	complextimate_t complexity;
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

struct caller_sessions_s {
	session_t session;
};
typedef struct caller_sessions_s caller_sessions_t;

session_desc_t *all_sessions;
caller_sessions_t *caller_sessions;

void init_sessions(void);
int set_node_orgin(session_t *, char *seedfiles);
int set_node_partial(session_t *, char *seedfiles);
void list_nodes(char *);
session_desc_t *session_descriptions(void);
caller_sessions_t *previous_caller_sessions(void);
hmapf_t *call_session(char *url, complextimate_t *);
int accept_called_session(session_t *, session_t *last, session_t *new);
int accept_caller_session(session_t *);
void sync_sessions(void);
int buffer_maps_to_peer_partial(session_t *s, select_t *sel);
hmapf_t *recieve_maps_from_peer_partial(session_t *s, select_t *sel);
int close_session(session_desc_t *);
int close_sessions(void);

#endif /* VRT_SESSION_H */
