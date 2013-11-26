/* loginkeys.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_LOGINKEYS_H
#define VRT_LOGINKEYS_H

#include "hmap.h"

struct ptlrepute_s {
	char *url; /* used by continuing node */
	session_t lastkey; /* most recent */
	session_t contingentkey; /* 2nd most recent */
	session_t holdkey; /* continuing, last holdmap at url */
	session_t holdbkp; /* continuing, previous to last holdmap at url */
	int vrtlogin; /* true where key is within flexible session */
	char *passwd; /* optional for vrtlogin if supported, see: session.c */
	struct ptlrepute_s *precursor;
};
typedef struct ptlrepute_s ptlrepute_t;

struct ptlrepute_list_s {
	session_t *reputed;
	ptlrepute_t *last;
	unsigned int count;
};
typedef struct ptlrepute_list_s ptlrepute_list_t;

/* Keyuse values for sync_loginkeys. */
enum {
	VRT_LOGIN_NEWREPUTED = 0,
	VRT_LOGIN_RETRY,
	VRT_LOGIN_LAST,
	VRT_LOGIN_CONTINGENT,
	VRT_LOGIN_SYNCERR,
	VRT_LOGIN_PRECONTEXT
};

ptlrepute_t *find_lastkey(ptlrepute_list_t *list, session_t *keyname);
ptlrepute_t *find_contingentkey(ptlrepute_list_t *list, session_t *keyname);
ptlrepute_t *find_holdkey(ptlrepute_list_t *list, session_t *keyname);
ptlrepute_t *find_zero_mapname(ptlrepute_list_t *list);

int sync_loginkeys(char *url, ptlrepute_list_t *, ptlrepute_t *, session_t *loginkey, session_t *holdkey, session_t *lastkey, session_t *contingentkey, int keyuse);
void flow_from_hold(session_t *key, session_t *holdkey, session_t *holdbkp);
ptlrepute_list_t *mk_ptlrepute_list(session_t *partial_session);
void rm_ptlrepute_list(ptlrepute_list_t *);
ptlrepute_t *add_ptlrepute(ptlrepute_list_t *, session_t *keyname, session_t *holdkey, char *url);
void subtract_ptlrepute(ptlrepute_list_t *, ptlrepute_t *);

#endif /* VRT_LOGINKEYS_H */
