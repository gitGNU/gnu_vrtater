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

/* partial */
struct partial_space {
	session_t session; /* maps from node-orgin are masked into session */
	list_t *list; /* session name and list of all hmaps in this partial */
	char *desc; /* set when calling mk_partial */
	hmapf_t *nodemap; /* hmap describing the volume of given partial */
	complextimate_t complexity;
	int running; /* is in session code's running set */
};
typedef struct partial_space partial_t;

partial_t *partial_generator_list;
int partials_count;

int select_partial_set(list_t *list, hmapf_t **maps);
list_t *find_partial(session_t *partial_session);

#endif /* VRT_PARTIAL_H */
