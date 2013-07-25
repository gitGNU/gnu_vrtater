/* partial.c: linked list mechanism to group partials.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include "partial.h"

int
select_partial_set(list_t *list, select_t *sel)
{
	int i;
	listed_t *listed;
	hmapf_t **map;

	listed = list->last;
	map = (hmapf_t **) (sel->seta);
	for (i = 0; i < list->count; i++, map++) {
		*map = listed->map;
		listed = listed->precursor;
	}
	return sel->counta = i;
}

list_t *
find_partial(session_t *partial_session)
{
	int i;
	partial_t **partial;

	partial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, partial++) {
		if (*partial_session == (*partial)->session) {
			return (*partial)->list;
		}
	}
	return NULL;
}
