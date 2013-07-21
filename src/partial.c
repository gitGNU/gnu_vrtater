/* partial.c: linked list mechanism to group partials.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include "partial.h"

list_t *
mk_list(session_t *name)
{
	list_t *list;
	if((list = (list_t *) malloc(sizeof(list_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial_list\n",
			__FILE__, __LINE__);
		abort();
	}
	list->name = *name;
	list->last = NULL; /* first in list will have a null precursor */
	list->count = 0;
	__builtin_printf("new partial list: %x\n", (int)*name);
	return list;
}

void
rm_list(list_t *list)
{
	while (list->last != NULL) {
		__builtin_printf("dissolving list...\n");
		subtract_from_list(list, list->last->map);
	}
	__builtin_printf("freeing allocation for list\n");
	free(list);
}

listed_t *
add_to_list(list_t *list, hmapf_t *map)
{
	listed_t *listed;

	if((listed = (listed_t *) malloc(sizeof(listed_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial_listed\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->precursor = list->last;
	listed->map = map;
	list->last = listed;
	list->count++;

if(listed->precursor) {
__builtin_printf("adding: %i precursor %i\n",
	list->last->map->index, listed->precursor->map->index);
} else {
__builtin_printf("adding: %i %x precursor\n",
	list->last->map->index, (int)listed->precursor);
}

	return listed;
}

int
subtract_from_list(list_t *list, hmapf_t *depreciated)
{
	listed_t *current;
	listed_t *passed;

	if(list->last == NULL)
		return -1;

	current = list->last; /* start at end */
	passed = list->last; /* keep a back reference */

	__builtin_printf("subtract_from_list: map %i, "
		"current %i, passed %i, last %i\n",
		depreciated->index, current->map->index,
		passed->map->index, list->last->map->index);

	while (1) {
		if (current != NULL) {
			if (current->map->name == depreciated->name) {
				break; /* match */
			}
			/* increment */
			passed = current;
			current = current->precursor;

			if(current) {
				__builtin_printf("adjusted: current %i, " 
					"passed %i, last %i\n",
					current->map->index,
					passed->map->index,
					list->last->map->index);
			}
		} else
			return -1;
	}

	/* subtract listed */
	if (current == passed) {

		/* last in list */
		if (!current->precursor) {

			/* one element list */
			list->last = NULL;
			__builtin_printf("subtracting: _last_, list now empty\n"
				"\tlast set to %x\n", (int)list->last);

		} else {

			/* last element where more than one */
			__builtin_printf("subtracting: _last_, resetting last, ");
			__builtin_printf("last was %i, ",
				list->last->map->index);

			list->last = current->precursor; /* last matched, reset */

			__builtin_printf("last ref now %x\n",
				(int)list->last);

		}
	} else {

		/* not last, no need to reset last, needs relink */
		__builtin_printf("subtracting: _not_last_\n"
			"\tpassed precursor ref %x (self)\n",
			(int)passed->precursor);

		passed->precursor = current->precursor; /* relink */

		__builtin_printf("\tpassing precursor ref %x to (previous)\n",
			(int)passed->precursor);
	}

	__builtin_printf("freeing allocation for listed\n");
	free(current);
	list->count--;

	return 0;
}

int
select_listed(list_t *list, select_t *sel)
{
	int i;
	listed_t *p;
	hmapf_t **map;

	p = list->last;
	map = (hmapf_t **) (sel->seta);
	for (i = 0; i < list->count; i++, map++) {
		*map = p->map;
		p = p->precursor;
	}
	return list->count;
}
