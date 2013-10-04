/* partial.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_PARTIAL_H
#define VRT_PARTIAL_H

#include "hmap.h"

struct listed_s {
	hmapf_t *map;
	struct listed_s *precursor;
};
typedef struct listed_s listed_t;

struct list_s {
	session_t *session;
	listed_t *last;
	unsigned int count;
};
typedef struct list_s list_t;

struct dialog_group_s {
	char *group;
	session_t key[2]; /* group hmap session name, group infrastructure */
	int total_members;
	session_t *members; /* current session name list of members */
	char *passwd; /* if non NULL password is implemented */
	struct dialog_group_s *next;
};
typedef struct dialog_group_s dialog_group_t;

struct reputation_s {
	char *url; /* called node-partial */
	session_t key[2]; /* 2 most recent session names */
	char *passwd; /* optional, if non NULL, password is implemented */
	struct reputation_s *next;
};
typedef struct reputation_s reputation_t;

struct partial_s {
	session_t session; /* maps from node-orgin are masked into session */
	list_t *list; /* session name and list of all hmaps in this partial */
	char *desc; /* set when calling mk_partial */
	hmapf_t *nodemap; /* hmap describing the volume of given partial */
	dialog_group_t *dialog_group; /* linked list of groups for partial */
	complextimate_t complexity;
	int running; /* is in session code's running set */
};
typedef struct partial_s partial_t;

partial_t *partial_generator_list;
int partials_count;

int select_partial_set(list_t *list, hmapf_t **maps);
list_t *find_partial(session_t *partial_session);
int update_reputation(session_t *session, hmapf_t *keymap, char *url);
#endif /* VRT_PARTIAL_H */
