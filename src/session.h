/* session.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_SESSION_H
#define VRT_SESSION_H

#include "hmap.h"
#include "loginkeys.h"

struct session_desc_s {
	session_t session;
	session_t peer;
	session_t thru; /* session is passing thru here before any redirect */
	btoggles_t level;
	char *url; /* set for a continuing */
	char oneliner[80];
	hmapf_t *nodemap; /* for flexible partial_t, copy of nodemap */
	ptlrepute_list_t *ptlrepute; /* for flexible, ptlrepute pointer copy */
	complextimate_t cmplxt; /* at arrival metaloc */
	struct session_desc_s *precursor;
};
typedef struct session_desc_s session_desc_t;

enum { /* session_desc_t level */
	VRT_ORDINAL_SESSION_DETACHED,
#define VRT_MASK_SESSION_DETACHED (1 << VRT_ORDINAL_SESSION_DETACHED)
	VRT_ORDINAL_SESSION_INBOUND,
#define VRT_MASK_SESSION_INBOUND (1 << VRT_ORDINAL_SESSION_INBOUND)
	VRT_ORDINAL_SESSION_ENABLE,
#define VRT_MASK_SESSION_ENABLE (1 << VRT_ORDINAL_SESSION_ENABLE)
	VRT_ORDINAL_SESSION_CALLED,
#define VRT_MASK_SESSION_CALLED (1 << VRT_ORDINAL_SESSION_CALLED)
	VRT_ORDINAL_SESSION_CONTINUITY,
#define VRT_MASK_SESSION_CONTINUITY (1 << VRT_ORDINAL_SESSION_CONTINUITY)
	VRT_ORDINAL_SESSION_USEPASSWD, /* when added sometime */
#define VRT_MASK_SESSION_USEPASSWD (1 << VRT_ORDINAL_SESSION_USEPASSWD)
	VRT_ORDINAL_SESSION_PARTIAL,
#define VRT_MASK_SESSION_PARTIAL (1 << VRT_ORDINAL_SESSION_PARTIAL)
	VRT_ORDINAL_SESSION_UNLINKED
#define VRT_MASK_SESSION_UNLINKED (1 << VRT_ORDINAL_SESSION_UNLINKED)
};

struct session_desc_list_s {
	session_desc_t *last;
	unsigned int count;
};
typedef struct session_desc_list_s session_desc_list_t;

btoggles_t session_nodemask;
enum { /* bits for session_nodemask */
	VRT_ORDINAL_ACCEPT_VRTLOGIN, /* all nodes */
#define VRT_MASK_ACCEPT_VRTLOGIN (1 << VRT_ORDINAL_ACCEPT_VRTLOGIN)
	VRT_ORDINAL_OVERLOADED
#define VRT_MASK_OVERLOADED (1 << VRT_ORDINAL_OVERLOADED)
};

session_desc_list_t *all_sessions;
int login_cmplxt_max;

void init_sessions(void);
int hash_session_name(session_t *, char *seedfiles);
int read_hash(session_t *, char *seedfiles);
void list_nodes(char *);
int node_continuity(char *url, char *oneliner, session_t *session_here, session_t *session_thru, hmapf_t **maps);
int continue_node(char *url, ptlrepute_list_t *new, ptlrepute_t *repute, session_t *session_here, session_t *loginkey, session_t *holdkey, unsigned int tl_cmplxt);
int answer_vrtlogin(session_t *session, session_t *thru, char *url); /* here for testing */ 
int sync_vrtlogin(session_t *peer, session_t *thru, session_t *loginkey, session_t *lastkey, session_t *contingentkey, session_t *holdkey, unsigned int tl_cmplxt, char *url); /* here for testing */ 
void sync_sessions(void);
int send_maps(session_t *, select_t *sel);
session_t *receive_map(select_t *sel);
session_desc_t *find_session(session_t *);
session_desc_t *find_url(char *oneliner);
void mk_session_desc_list(void);
session_desc_t *add_session_desc(session_t *, session_t *peer, session_t *thru, btoggles_t level, char *url, char *oneliner, complextimate_t *, hmapf_t *nodemap, ptlrepute_list_t *list);
int close_session(session_desc_t *);
int close_all_sessions(void);
int reset_sessions(void);
/* Diagnostics. */
void diag_ls_all_sessions(int full);

#endif /* VRT_SESSION_H */
