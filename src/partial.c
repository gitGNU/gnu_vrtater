/* partial.c: Calls returning partial set for hmaps.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include "partial.h"
#include "global.h"

/* Return partial reference vs. partial_session or NULL if not found. */
partial_t *
find_partial(session_t *partial_session)
{
	partial_t *current, *passed;

	current = partial_list->last;
	passed = partial_list->last;
	while (1) {
		if (current != NULL) {
			if (current->session == *partial_session)
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

/* Create a linked list construct for partial_session returning reference to an
   empty list of hmaps residing therein. */
ptlmaps_list_t *
mk_ptlmaps_list(session_t *partial_session)
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

	list->session = partial_session;

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
			if (current->map->name == map->name)
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

/* Try to find the linked list element referencing keyname in list.  Search
   backup keys if srchbkp != 0.  Return reference to element or NULL if not
   found. */
ptlrep_t *
find_repute(ptlreps_list_t *list, session_t *keyname, int srchbkp)
{
	ptlrep_t *current, *passed;

	current = list->last;
	passed = list->last;
	if (srchbkp) {
		while (1) {
			if (current != NULL) {
				if (current->bkpkey == *keyname)
				{
					__builtin_printf(" previous local "
						"backup reputation %x was "
						"found\n",
						(int) current->bkpkey);
					return current;
				}
				passed = current;
				current = current->precursor;
			} else
				return NULL;
		}
	} else {
		while (1) {
			if (current != NULL) {
				if (current->key == *keyname)
				{
					__builtin_printf(" previous local "
						"reputation %x was found\n",
						(int) current->key);
					return current;
				}
				passed = current;
				current = current->precursor;
			} else
				return NULL;
		}
	}
}

/* For the purposes of illustration, push keyname loginkey down onto 2 deep
   reputation stack for list member, usually shifting list keyname lastkey
   over any validkey, and allowing any previous validkey in list to overflow.
   Update list locally for outbound keymap logging flexible node at url or for
   inbound keymap with no url.  If inbound and never having logged local
   flexible partial, start a new reputation.  In this case lastkey and validkey
   will be -1 were they an int.  If in the _extremely_ unusual case that given
   validkey does not match either of lastkey nor validkey listed, a reputation
   recovery option should be made avaiable in the ifnode interface when added.
   Any login with loginkey matching any currently listed key are rejected.  If
   node with flexible partial changes it's url, sync will also reflect new url.
   Return values given in partial.h else -1. */
int
sync_reputation(ptlreps_list_t *list, ptlrep_t *reputed, session_t *loginkey, session_t *lastkey, session_t *validkey, char *url, int keyuse)
{
	ptlrep_t *match = NULL;
	ptlrep_t *tmp = NULL;
	ptlrep_t *local = NULL;

#ifdef DIAG_FLEXIBLE_MAPKEY_REDUNDANT
	/* Place a key that will test as redundant in the reputation data */
	reputed->key = *loginkey;
#elif defined DIAG_FLEXIBLE_MAPBKPKEY_REDUNDANT
	reputed->bkpkey = *loginkey;
#endif

	/* Never write a new loginkey unless it is unique.  This only applies
	   to a flexible partial session as a continuing one will have an
	   empty ptlreps list. */
	if (((match = find_repute(list, loginkey, VRT_PARTIAL_SRCHMAPKEY)) != NULL) || ((match = find_repute(list, loginkey, VRT_PARTIAL_SRCHMAPBKPKEY)) != NULL)) {
		__builtin_printf("Flexible login failed, redundant login key "
			"given was unusable.\n");
		return VRT_PARTIAL_RETRY;
	}
	if (url) {
		/* Sync local reputations for remote continuing partial login.
		   Given a password, if any, reputation sync, has already
		   suceeded or failed on remote, returning keyuse. */
		if (keyuse == VRT_PARTIAL_LASTKEY) {
			__builtin_printf(" syncing local reputation vs. login "
				"with %x %x %x at:\n  %s\n", (int) *loginkey,
				(int) *lastkey, (int) *validkey, url);
			reputed->url = url;
			reputed->bkpkey = reputed->key;
			reputed->key = *loginkey;
			return keyuse;
		} else if (keyuse == VRT_PARTIAL_VALIDUSE) {
			/* An unknown lastkey has failed to match either on
			   remote or local reputation list. A disk write may
			   have failed while a partial login succeeded. */
			__builtin_printf(" syncing local reputation vs. "
				"continuing login with loginkey %x, unknown "
				"lastkey %x, and validkey %x, at:\n  %s\n",
				(int) *loginkey, (int) *lastkey,
				(int) *validkey, url);
			reputed->url = url;
			reputed->key = *loginkey;
			return keyuse;
		} else if (keyuse == VRT_PARTIAL_SYNCERR) {
			/* Given keys failed to match, requires recovery
			   option. */
			return keyuse;
		} else {
			/* Start a new reputation. */
			__builtin_printf("Adding new reputation for %x for "
				"remote continuing partial at:\n  %s\n",
				(int) *loginkey, url);
			tmp = add_ptlrep(list, loginkey, url);
			tmp->bkpkey = *loginkey;
			tmp->holdkey = 0;
			return VRT_PARTIAL_NEWREPUTED;
		}
	} else {
		/* Sync local reputations for local flexible partial login. */
		if ((*lastkey == 0xffffffff) && (*validkey == 0xffffffff)) {
			/* Start new reputation. */
			__builtin_printf("Adding new reputation on local "
				"flexible partial for %x\n", (int) *loginkey);
			tmp = add_ptlrep(list, loginkey, url);
			tmp->bkpkey = *loginkey;
			tmp->holdkey = 0;
			return VRT_PARTIAL_NEWREPUTED;
		} else if ((local = find_repute(list, lastkey, VRT_PARTIAL_SRCHMAPKEY))) {
			/* Continue. */
			local->bkpkey = *lastkey;
			local->key = *loginkey;
			return VRT_PARTIAL_SRCHMAPKEY;
		} else if ((local = find_repute(list, validkey, VRT_PARTIAL_SRCHMAPKEY))) {
			/* Sync issue.  Key was never shifted locally.  Remote
			   lastkey not in list.  list still references remote
			   validkey as local lastkey. */
			local->bkpkey = *validkey;
			local->key = *loginkey;
			return VRT_PARTIAL_VALIDUSE;
		} else if ((local = find_repute(list, validkey, VRT_PARTIAL_SRCHMAPBKPKEY))) {
			/* Sync issue.  lastkey was never written successfully
			   on one or both puters.  Remote lastkey is thus not
			   in list.  list still references remote validkey as
			   local validkey. */
			local->key = *loginkey;
			return VRT_PARTIAL_VALIDUSE;
		} else 
			/* Sync error.  validkey nor lastkey found in list. */
			return VRT_PARTIAL_SYNCERR;
	}
}

/* Create a linked list construct for partial_session returning reference to an
   empty list of reputations residing therein. */
ptlreps_list_t *
mk_ptlreps_list(session_t *partial_session)
{
	ptlreps_list_t *list = NULL;
	if ((list = (ptlreps_list_t *) malloc(sizeof(ptlreps_list_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_reps_list\n",
			__FILE__, __LINE__);
		abort();
	}
	list->last = NULL;
	list->count = 0;

	list->session = partial_session;

	return list;
}

/* Remove list and all of it's elements and element references from memory. */
void
rm_ptlreps_list(ptlreps_list_t *list)
{
	while (list->last != NULL)
		subtract_ptlrep(list, list->last);
	free(list);
}

/* Add an element reference to partial repute in the linked list construct list.
   Return reference to element listed. */
ptlrep_t *
add_ptlrep(ptlreps_list_t *list, session_t *key, char *url)
{
	ptlrep_t *listed = NULL;

	if ((listed = (ptlrep_t *) malloc(sizeof(ptlrep_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_reps_list entry\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->precursor = list->last;
	list->last = listed;
	list->count++;

	listed->url = url;
	listed->key = *key;
	listed->bkpkey = *key;
	listed->holdkey = 0x00000000;
	listed->logged_on = 0;

	return listed;
}

/* Subtract linked list element referenced by reputed. */
void
subtract_ptlrep(ptlreps_list_t *list, ptlrep_t *reputed)
{
	ptlrep_t *current, *passed;

	__builtin_printf(" removing reputation from %x 1/%i\n",
		*(list->session), list->count);

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (current->key == reputed->key)
				break;
			else if (current->bkpkey == reputed->bkpkey)
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	__builtin_printf("  repute %x dissolves in memory...\n",
		(int) current->key);
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
			if (current->sign_in == *sign_in)
			{
				__builtin_printf("  member with sign-in %x was "
					"found\n", (int) current->sign_in);
				return current;
			}
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Create a linked list construct for partial_session returning reference to an
   empty list of members residing therein. */
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
	__builtin_printf("  member with sign_in %x added...\n",
		(int) *sign_in);
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
			if (current->sign_in == member->sign_in)
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	__builtin_printf("  member with sign-in %x left group...\n",
		(int) current->sign_in);
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
			if (current->map_name == *grpmap_name)
			{
				__builtin_printf(" group with map %x was "
					"added/found\n",
					(int) current->map_name);
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
   login, when a keymap's reputation key in a partial is shifted becoming
   bkpkey, any ptlmbr_t member sign_in for given partial, must be re-referenced
   vs. that bkpkey to the newly generated key so that dialog based groups become
   maintained with most recent keynames for keymaps.  This also applies to each
   entry from node_orgin into any partial. */
void
sync_groups(ptlgrps_list_t *list, session_t *last, session_t *new)
{
	;
}

/* Create a linked list construct for partial_session returning reference to an
   empty list of groups residing therein. */
ptlgrps_list_t *
mk_ptlgrps_list(session_t *partial_session)
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

	list->session = partial_session;

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

	listed->map_name = *groupmap_name;
	listed->members = mk_ptlmbrs_list(listed);
	__builtin_printf("  group vs. %x added...\n",
		(int) *groupmap_name);

	return listed;
}

/* Subtract linked list element referenced by group. */
void
subtract_ptlgrp(ptlgrps_list_t *list, ptlgrp_t *group)
{
	ptlgrp_t *current, *passed;

	__builtin_printf(" message for group with groupmap %x\n",
		group->map_name);

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (current->map_name == group->map_name)
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	rm_ptlmbrs_list(current->members);
	__builtin_printf("  group .vs %x abandon this partial...\n",
		(int) current->map_name);
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
