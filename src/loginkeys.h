/* loginkeys.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_LOGINKEYS_H
#define VRT_LOGINKEYS_H

#include "hmap.h"

struct ptlrepute {
	char *url; /* used by continuing node */
	session_t lastkey; /* most recent */
	session_t contingentkey; /* 2nd most recent */
	session_t holdkey; /* continuing, last holdmap at url */
	session_t holdbkp; /* continuing, previous to last holdmap at url */
	int vrtlogin; /* true where key is within flexible session */
	char *passwd; /* optional for vrtlogin if supported, see: session.c */
	struct ptlrepute *precursor;
};

struct ptlrepute_list {
	session_t *session;
	struct ptlrepute *last;
	unsigned int count;
};

/* Keyuse values for sync_loginkeys. */
enum {
	VRT_LOGIN_NEWREPUTED = 0,
	VRT_LOGIN_RETRY,
	VRT_LOGIN_LAST,
	VRT_LOGIN_CONTINGENT,
	VRT_LOGIN_SYNCERR,
	VRT_LOGIN_PRECONTEXT
};

struct ptlrepute *find_lastkey(struct ptlrepute_list *list, session_t *keyname);
struct ptlrepute *find_contingentkey(struct ptlrepute_list *list, session_t *keyname);
struct ptlrepute *find_holdkey(struct ptlrepute_list *list, session_t *keyname);
struct ptlrepute *find_zero_mapname(struct ptlrepute_list *list);

int sync_loginkeys(char *url, struct ptlrepute_list *, struct ptlrepute *, session_t *loginkey, session_t *holdkey, session_t *lastkey, session_t *contingentkey, int keyuse);
void flow_from_hold(session_t *key, session_t *holdkey, session_t *holdbkp);
struct ptlrepute_list *mk_ptlrepute_list(session_t *session);
void rm_ptlrepute_list(struct ptlrepute_list *);
struct ptlrepute *add_ptlrepute(struct ptlrepute_list *, session_t *keyname, session_t *holdkey, char *url);
void subtract_ptlrepute(struct ptlrepute_list *, struct ptlrepute *);

#endif /* VRT_LOGINKEYS_H */
