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
	session_t name;
	listed_t *last;
	unsigned int count;
};
typedef struct list_s list_t;

list_t *mk_list(session_t *name);
void rm_list(list_t *list);
listed_t *add_to_list(list_t *list, hmapf_t *map);
int subtract_from_list(list_t *list, hmapf_t *depreciated);
int select_listed(list_t *list, select_t *sel);

#endif /* VRT_PARTIAL_H */
