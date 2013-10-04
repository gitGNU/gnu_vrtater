/* partial.c: Calls returning partial set for hmaps.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include "partial.h"

/* Copy any refrences for partial in list to allocation referenced by maps. */
int
select_partial_set(list_t *list, hmapf_t **maps)
{
	int i;
	listed_t *listed;
	hmapf_t **map;

	map = maps;
	listed = list->last;
	for (i = 0; i < list->count; i++, map++) {
		*map = listed->map;
		listed = listed->precursor;
	}
	return i;
}

/* Return list of hmaps for partial refrenced by partial_session. */
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

/* Update keymap reputation list locally for keymap /w session logging url. */
int
update_reputation(session_t *session, hmapf_t *keymap, char *url)
{
	return 0;;
}
