/* partial.c: Calls returning partial set for hmaps.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include "partial.h"
#include "progscope.h"

/* Return partial reference vs. session or NULL if not found. */
struct partial *
find_partial(session_t *session)
{
	struct partial *current, *passed;

	current = partials->last;
	passed = partials->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->session), session))
				return current;
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Copy any refrences for partial in list to allocation referenced by maps. */
int
select_partial_set(ptlmaps_list_t *list, hmapf_t **maps)
{
	int i;
	ptlmap_t *listed;
	hmapf_t **map;

	map = maps;
	listed = list->last;
	for (i = 0; i < list->count; i++, map++) {
		*map = listed->map;
		listed = listed->precursor;
	}
	return i;
}

/* Create a linked list construct for session returning reference to an empty
   list of hmaps residing therein. */
ptlmaps_list_t *
mk_ptlmaps_list(session_t *session)
{
	ptlmaps_list_t *list = NULL;
	if ((list = (ptlmaps_list_t *) malloc(sizeof(ptlmaps_list_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_maps_list\n",
			__FILE__, __LINE__);
		abort();
	}
	list->last = NULL; /* first in list will have a NULL precursor */
	list->count = 0;

	list->session = session;

	return list;
}

/* Remove list and all of it's element references. */
void
rm_ptlmaps_list(ptlmaps_list_t *list)
{
	while (list->last != NULL)
		subtract_ptlmap(list, list->last->map);
	free(list);
}

/* Add an element reference to partial hmap map in the linked list construct
   list.  Set VRT_MASK_PARTIAL.  Also set VRT_MASK_PARTIAL_MODS informing code
   in session.c that this map should be sync'd with remote nodes.  Return
   reference to element listed. */
ptlmap_t *
add_ptlmap(ptlmaps_list_t *list, hmapf_t *map)
{
	ptlmap_t *listed = NULL;

	if ((listed = (ptlmap_t *) malloc(sizeof(ptlmap_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_maps_list entry\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->precursor = list->last;
	list->last = listed;
	list->count++;

	listed->map = map;
	listed->map->attribs.sign |= (VRT_MASK_PARTIAL | VRT_MASK_PARTIAL_MODS);

	return listed;
}

/* Subtract linked list element referencing hmap map from list.  Unset
   VRT_MASK_PARTIAL.  */
void
subtract_ptlmap(ptlmaps_list_t *list, hmapf_t *map)
{
	ptlmap_t *current, *passed;

	current = list->last; /* start at end */
	passed = list->last; /* keep a back reference */
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->map->name), &(map->name)))
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	if (current == passed) {
		if (!current->precursor)
			list->last = NULL;
		else
			list->last = current->precursor; /* reset */
	} else
		passed->precursor = current->precursor; /* relink */
	map->attribs.sign &= (0xffffffff ^ VRT_MASK_PARTIAL);
	free(current);
	list->count--;
}

/* Try to find the linked list element for sign_in in list.  Return reference to
   element or NULL if not found. */
ptlmbr_t *
find_member(ptlmbrs_list_t *list, session_t *sign_in)
{
	ptlmbr_t *current, *passed;

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->sign_in), sign_in))
			{
				__builtin_printf("  member with sign-in "
					"(%x %x %x) %i was found\n",
					current->sign_in.hash.h,
					current->sign_in.hash.m,
					current->sign_in.hash.l,
					current->sign_in.seq);
				return current;
			}
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Create a linked list construct for group returning reference to an empty list
   of members residing therein. */
ptlmbrs_list_t *
mk_ptlmbrs_list(ptlgrp_t *group)
{
	ptlmbrs_list_t *list = NULL;
	if ((list = (ptlmbrs_list_t *) malloc(sizeof(ptlmbrs_list_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_mbrs_list\n",
			__FILE__, __LINE__);
		abort();
	}
	list->last = NULL;
	list->count = 0;

	group->members = list;

	return list;
}

/* Remove list and all of it's elements and element references from memory. */
void
rm_ptlmbrs_list(ptlmbrs_list_t *list)
{
	while (list->last != NULL)
		subtract_ptlmbr(list, list->last);
	free(list);
}

/* Add an element reference to member in the linked list construct list.  Return
   reference to element listed. */
ptlmbr_t *
add_ptlmbr(ptlmbrs_list_t *list, session_t *sign_in)
{
	ptlmbr_t *listed = NULL;

	if ((listed = (ptlmbr_t *) malloc(sizeof(ptlmbr_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_mbrs_list entry\n",
			__FILE__, __LINE__);
		abort();
	}
	__builtin_printf("  member with sign_in (%x %x %x) %i added...\n",
		sign_in->hash.h, sign_in->hash.m, sign_in->hash.l,
		sign_in->seq);
	listed->precursor = list->last;
	list->last = listed;
	list->count++;

	listed->sign_in = *sign_in;

	return listed;
}

/* Subtract linked list element referenced by member. */
void
subtract_ptlmbr(ptlmbrs_list_t *list, ptlmbr_t *member)
{
	ptlmbr_t *current, *passed;

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->sign_in), &(member->sign_in)))
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	__builtin_printf("  member with sign-in (%x %x %x) %i left group...\n",
		current->sign_in.hash.h, current->sign_in.hash.m,
		current->sign_in.hash.l, current->sign_in.seq);
	if (current == passed) {
		if (!current->precursor)
			list->last = NULL;
		else
			list->last = current->precursor;
	} else
		passed->precursor = current->precursor;
	free(current);
	list->count--;
}

/* Try to find the linked list element for group with grpmap_name in list.
   Return reference to element or NULL if not found. */
ptlgrp_t *
find_group(ptlgrps_list_t *list, session_t *grpmap_name)
{
	ptlgrp_t *current, *passed;

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->map_name), grpmap_name)) {
				__builtin_printf(" group with map (%x %x %x) "
					"%i was added/found\n",
					current->map_name.hash.h,
					current->map_name.hash.m,
					current->map_name.hash.l,
					current->map_name.seq);
				return current;
			}
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* For partial referencing list, set each ptlmbr_t member sign_in equaling
   lastsign_in to given argument sign_in for all groups therein.  note: On
   vrtlogin, when a keymap's reputation key in a partial is shifted becoming
   bkpkey, any ptlmbr_t member sign_in for given partial, must be re-referenced
   vs. that bkpkey to the newly generated key so that dialog based groups
   become maintained with most recent keynames for keymaps.  This also applies
   to each entry from node_orgin into any partial. */
void
sync_groups(ptlgrps_list_t *list, session_t *last, session_t *new)
{
	;
}

/* Create a linked list construct for session returning reference to an empty
   list of groups residing therein. */
ptlgrps_list_t *
mk_ptlgrps_list(session_t *session)
{
	ptlgrps_list_t *list = NULL;
	if ((list = (ptlgrps_list_t *) malloc(sizeof(ptlgrps_list_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_grps_list\n",
			__FILE__, __LINE__);
		abort();
	}
	list->last = NULL;
	list->count = 0;

	list->session = session;

	return list;
}

/* Remove list and all of it's elements and element references from memory. */
void
rm_ptlgrps_list(ptlgrps_list_t *list)
{
	if (list) {
		while (list->last != NULL) {
			subtract_ptlgrp(list, list->last);
		}
		free(list);
	}
}

/* Add an element reference to partial group in the linked list construct list.
   Return reference to element listed. */
ptlgrp_t *
add_ptlgrp(ptlgrps_list_t *list, session_t *groupmap_name)
{
	ptlgrp_t *listed = NULL;

	if ((listed = (ptlgrp_t *) malloc(sizeof(ptlgrp_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_grps_list entry\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->precursor = list->last;
	list->last = listed;
	list->count++;

	cp_mapname(groupmap_name, &(listed->map_name));
	listed->members = mk_ptlmbrs_list(listed);
	__builtin_printf("  group with groupmap (%x %x %x) %i added...\n",
		groupmap_name->hash.h, groupmap_name->hash.m,
		groupmap_name->hash.l, groupmap_name->seq);

	return listed;
}

/* Subtract linked list element referenced by group. */
void
subtract_ptlgrp(ptlgrps_list_t *list, ptlgrp_t *group)
{
	ptlgrp_t *current, *passed;

	__builtin_printf(" message for group with groupmap (%x %x %x) %i\n",
		group->map_name.hash.h, group->map_name.hash.m,
		group->map_name.hash.l, group->map_name.seq);

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->map_name), &(group->map_name)))
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	rm_ptlmbrs_list(current->members);
	__builtin_printf("  group with groupmap (%x %x %x) %i abandon this "
		"partial...\n", current->map_name.hash.h,
		current->map_name.hash.m, current->map_name.hash.l,
		current->map_name.seq);
	if (current == passed) {
		if (!current->precursor)
			list->last = NULL;
		else
			list->last = current->precursor;
	} else
		passed->precursor = current->precursor;
	free(current);
	list->count--;
}
