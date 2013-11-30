/* loginkeys.c: Maintain vrtlogin data.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include "loginkeys.h"
#include "progscope.h"

/* Try to find the linked list element referencing keyname as lastkey in list.
   Return reference to element or NULL if not found. */
struct ptlrepute *
find_lastkey(struct ptlrepute_list *list, session_t *keyname)
{
	struct ptlrepute *current, *passed;

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->lastkey), keyname))
				return current;
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Try to find the linked list element referencing keyname as contingentkey in
   list.  Return reference to element or NULL if not found. */
struct ptlrepute *
find_contingentkey(struct ptlrepute_list *list, session_t *keyname)
{
	struct ptlrepute *current, *passed;

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->contingentkey), keyname))
				return current;
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Try to find the linked list element referencing keyname as holdkey or
   holdbkp in list,  ignoring any zero_mapname sent vs. holdkeys and any row
   of zero_mapnames found in holdkeys.  Return reference to element or NULL. */
struct ptlrepute *
find_holdkey(struct ptlrepute_list *list, session_t *keyname)
{
	struct ptlrepute *current, *passed;
	session_t zero_mapname = { { 0, 0, 0}, 0 };

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if ((match_mapname(&(current->holdkey), keyname) || match_mapname(&(current->holdbkp), keyname)) && (!(match_mapname(&(current->holdkey), &zero_mapname) && match_mapname(&(current->holdbkp), &zero_mapname))) && (!(match_mapname(keyname, &zero_mapname))))
				return current;
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Try to find next linked list element referencing a zero_mapname as both
   holdkey and holdbkp in list.  Return reference to element or NULL if not
   found.  note: Any other searches of list ignore a zero_mapname in hold. */
struct ptlrepute *
find_zero_mapname(struct ptlrepute_list *list)
{
	struct ptlrepute *current, *passed;
	session_t zero_mapname = { { 0, 0, 0}, 0 };

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->holdkey), &zero_mapname) && match_mapname(&(current->holdbkp), &zero_mapname))
				return current;
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Update ptlrepute loginkey reputations.  On a continuing node do so for
   keymap with mapname loginkey logging url using reputed provided, or on a
   flexible for keymap with mapname loginkey, using list provided.  Fail to
   syncronize if loginkey or holdkey are non unique on their respective sides
   of ptlrepute_s session_t data.  Try to update holdkey if it has any value
   other than a zero mapname.  lastkey and contingentkey provide syncronization
   logic.  keyuse sets a context for syncronizing when a continuing node
   receives feedback, or when a flexible wants to send that feedback.  Return
   keyuse value's given in partial.h whether they were just used or otherwise
   are to be used by peer.  note: For the purposes of illustration, push
   keyname loginkey down onto 2 deep reputation stack for list member reputed,
   usually shifting reputed keyname lastkey over any contingentkey, and
   allowing any previous contingentkey to overflow.  If continuing and having
   never logged implied flexible node, a new reputation is setup on flexible
   by calling with lastkey and contingentkey both with a zero mapname.  If in
   the _extremely_ unusual case that given contingentkey does not match either
   of lastkey nor contingentkey, a reputation recovery option is to be made
   avaiable in the ifnode**.c interface part when added.  A feature to soon be
   added to this function will compare the url in any continuing repute with
   the received non NULL url from flexible and if changed, create a backup
   repute using the old url with a small edit after the final char therein,
   ideally a slash, that edit ending with a slash.  The backup repute will
   also be different in that it will use the overflowed contingentkey of the
   now current repute with new url.  The backup repute will fully preserve all
   other members of the now current one.  Since they can never be used
   together, their keys will never be redundant when logging.  This will work
   in tandem with reading reputations into all_sessions for partialing, using
   the unlinked level bit in partial session description with function
   continue_repute.  In an unusual case this should help. */
int
sync_loginkeys(char *url, struct ptlrepute_list *list, struct ptlrepute *reputed, session_t *loginkey, session_t *holdkey, session_t *lastkey, session_t *contingentkey, int keyuse)
{
	struct ptlrepute *match = NULL;
	int publicize = 0;

	/* Determine if code in dialog*.c should to try to publicize any new
	   held maps with VRT_MASK_FLOW set. */
	publicize = (holdkey->hash.h | holdkey->hash.m | holdkey->hash.l | holdkey->seq);

#ifdef DIAG_NODEKEYS
	struct ptlrepute *keys;
	if ((keys = find_lastkey(list, loginkey)))
		printf("loginkey (%x %x %x) %i  found in lastkey column of "
			"list\n", keys->lastkey.hash.h, keys->lastkey.hash.m,
			keys->lastkey.hash.l, keys->lastkey.seq);
	if ((keys = find_contingentkey(list, loginkey)))
		printf("loginkey (%x %x %x) %i -found in contingentkey column "
			"of list-\n", keys->contingentkey.hash.h,
			keys->contingentkey.hash.m, keys->contingentkey.hash.l,
			keys->contingentkey.seq);
	if ((keys = find_holdkey(list, holdkey)))
		printf("holdkey's (%x %x %x) %i (%x %x %x) %i  found in "
			"holdkey side of list\n",
			keys->holdkey.hash.h, keys->holdkey.hash.m,
			keys->holdkey.hash.l, keys->holdkey.seq,
			keys->holdbkp.hash.h, keys->holdbkp.hash.m,
			keys->holdbkp.hash.l, keys->holdbkp.seq);
	if ((keys = find_zero_mapname(list)))
		printf(" zero_mapname's (%x %x %x) %i (%x %x %x) %i  found in "
			"holdkey side of list\n",
			keys->holdkey.hash.h, keys->holdkey.hash.m,
			keys->holdkey.hash.l, keys->holdkey.seq,
			keys->holdbkp.hash.h, keys->holdbkp.hash.m,
			keys->holdbkp.hash.l, keys->holdbkp.seq);
#endif /* DIAG_NODEKEYS */

#ifdef DIAG_FLEXIBLE_HOLDKEY_REDUNDANT
	__builtin_printf("note: for diagnostic preset holdkey to "
		"\"(0 0 c0de) 555\" so that sync fail's\n");
	if ((match = find_lastkey(list, lastkey)))
		flow_from_hold(holdkey, &(match->holdkey), &(match->holdbkp));
#endif

	/* loginkey's and holdkey's are not written unless unique.  continuing
	   node's begin with a new ptlrepute list so therein, all keys will be
	   unique.  loginkey and holdkey lists are independant. */
	if (find_lastkey(list, loginkey) || find_contingentkey(list, loginkey) || find_holdkey(list, holdkey)) {
		return VRT_LOGIN_RETRY;
	} else if (url) {
		/* Syncronize reputations in continuing node apon logging a
		   flexible.  Given a password, if any, reputation sync, has
		   already suceeded or failed on flexible. Here, feedback value
		   keyuse is tested and feedback context is returned. */
		if (keyuse == VRT_LOGIN_LAST) {
			__builtin_printf("Syncronizing with node %s\n", url);
			reputed->url = url;
			cp_mapname(&(reputed->lastkey), &(reputed->contingentkey));
			cp_mapname(loginkey, &(reputed->lastkey));
			if (publicize)
				flow_from_hold(holdkey, &(reputed->holdkey), &(reputed->holdbkp));
			__builtin_printf(" ...syncronized.  lastkey match.\n");
		} else if (keyuse == VRT_LOGIN_CONTINGENT) {
			/* An unknown lastkey exists either on continuing node
			   or flexible node reputation list. A disk write may
			   have failed while a vrtlogin partial to a flexible
			   node succeeded. */
			__builtin_printf("Syncronizing with node %s\n", url);
			reputed->url = url;
			cp_mapname(loginkey, &(reputed->lastkey));
			if (publicize)
				flow_from_hold(holdkey, &(reputed->holdkey), &(reputed->holdbkp));
			__builtin_printf(" ...syncronized.  note: "
				"lastkey !!mismatch.\n");
		} else if (keyuse == VRT_LOGIN_NEWREPUTED) {
			/* Start a new reputation. */
			__builtin_printf("New reputation on node %s\n", url);
			match = add_ptlrepute(list, loginkey, holdkey, url);
			cp_mapname(loginkey, &(match->contingentkey));
			zero_fullname(&(match->holdbkp));
			if (publicize)
				flow_from_hold(holdkey, &(reputed->holdkey), &(reputed->holdbkp));
			__builtin_printf(" ...new reputation.\n");
		} else if (keyuse == VRT_LOGIN_RETRY) {
		__builtin_printf("Try again with something any bit "
			"different.\n ...a vrtlogin key was redundant on "
			"flexible.\n");
		} else if (keyuse == VRT_LOGIN_SYNCERR) {
			__builtin_printf("Error: Exclusive pair errors.  "
				"Continuing requires recovery.  Option to\n"
				"simplify this may have been added.  see: "
				"ifnode interface for more.\n");
			__builtin_printf(" ...holdkey recovery to continue.\n");
		} else
			__builtin_printf("Error: keyuse unrecognized.\n");
	} else {
		/* Syncronize reputations on a flexible node during a vrtlogin.
		   Return feedback value keyuse, so that it can be sent to
		   node logging in. */
		if (!((lastkey->hash.h | lastkey->hash.m | lastkey->hash.l | lastkey->seq | contingentkey->hash.h | contingentkey->hash.m  | contingentkey->hash.l | contingentkey->seq))) {
			/* Start new reputation. */
			__builtin_printf("Adding new reputation for "
				"(%x %x %x) %i\n", loginkey->hash.h,
				loginkey->hash.m, loginkey->hash.l,
				loginkey->seq);
			match = add_ptlrepute(list, loginkey, holdkey, url);
			cp_mapname(loginkey, &(match->contingentkey));
			zero_fullname(&(match->holdbkp));
			if (publicize)
				flow_from_hold(holdkey, &(match->holdkey), &(match->holdbkp));
			return VRT_LOGIN_NEWREPUTED;
		} else if ((match = find_lastkey(list, lastkey))) {
			/* Continue. */
			__builtin_printf(" lastkey found in lastkey column of list\n"); /* diag */
			cp_mapname(lastkey, &(match->contingentkey));
			cp_mapname(loginkey, &(match->lastkey));
			if (publicize)
				flow_from_hold(holdkey, &(match->holdkey), &(match->holdbkp));
			return VRT_LOGIN_LAST;
		} else if ((match = find_lastkey(list, contingentkey))) {
			/* Sync issue.  Key was never shifted locally.  Remote
			   lastkey not in list.  list still references remote
			   contingentkey as local lastkey. */
			__builtin_printf(" contingentkey found in lastkey column of list\n"); /* diag */
			cp_mapname(contingentkey, &(match->contingentkey));
			cp_mapname(loginkey, &(match->lastkey));
			if (publicize)
				flow_from_hold(holdkey, &(match->holdkey), &(match->holdbkp));
			return VRT_LOGIN_CONTINGENT;
		} else if ((match = find_contingentkey(list, contingentkey))) {
			/* Sync issue.  lastkey was never written successfully
			   on one or both puters.  continuing lastkey is thus
			   not in list.  list still references remote
			   contingentkey as local contingentkey. */
			__builtin_printf(" contingentkey found in contingentkey column of list\n"); /* diag */
			cp_mapname(loginkey, &(match->lastkey));
			if (publicize)
				flow_from_hold(holdkey, &(match->holdkey), &(match->holdbkp));
			return VRT_LOGIN_CONTINGENT;
		} else {
			/* Sync error.  contingentkey nor lastkey were found in
			   list. */
			__builtin_printf(" given keys did not match\n"); /* diag */
			return VRT_LOGIN_SYNCERR;
		}
	}

	return keyuse;
}

/* Add map with mapname key to holdmaps as holdkey, shifting holdkey to holdbkp
   and allowing previous holdbkp to overflow.  map is then treated as an hmap
   that groups other hmaps that may have VRT_MASK_FLOW set asking any flexible
   node to by default publicize those in an automated way.  Person running
   vrtater instance with flexible node implied may choose to attend to this
   publicity as they see fit.  An automatic timestamp feature is soon to be
   added supporting the authorship mechanism for this publicity mandated. */
void
flow_from_hold(session_t *key, session_t *holdkey, session_t *holdbkp)
{
	__builtin_printf("Flow from holdmaps, moderated.\n");
	cp_mapname(holdkey, holdbkp);
	cp_mapname(key, holdkey);
}

/* Create a linked list construct for node session returning reference to an
   empty list of loginkey reputations residing therein. */
struct ptlrepute_list *
mk_ptlrepute_list(session_t *session)
{
	struct ptlrepute_list *list = NULL;
	if ((list = (struct ptlrepute_list *) malloc(sizeof(struct ptlrepute_list))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_reps_list\n",
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
rm_ptlrepute_list(struct ptlrepute_list *list)
{
	while (list->last != NULL)
		subtract_ptlrepute(list, list->last);
	free(list);
}

/* Add an element reference to partial repute in the linked list construct
   list.  Return reference to element listed. */
struct ptlrepute *
add_ptlrepute(struct ptlrepute_list *list, session_t *keyname, session_t *holdkey, char *url)
{
	struct ptlrepute *listed = NULL;
	session_t *key, z = { { 0, 0, 0 }, 0 };

	if ((listed = (struct ptlrepute *) malloc(sizeof(struct ptlrepute))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_reps_list "
			"entry\n", __FILE__, __LINE__);
		abort();
	}
	listed->precursor = list->last;
	list->last = listed;
	list->count++;

	listed->url = url;
	cp_mapname(keyname, &(listed->lastkey));
	cp_mapname(keyname, &(listed->contingentkey));
	cp_mapname(key = holdkey ? holdkey : &z, &(listed->holdkey));
	cp_mapname(&z, &(listed->holdbkp));
	cp_mapname(holdkey, &(listed->holdkey));

	return listed;
}

/* Subtract linked list element referenced by reputed.  note: This relies on
   keys in ptlrepute uniqueness assertion, see: sync_loginkeys. */
void
subtract_ptlrepute(struct ptlrepute_list *list, struct ptlrepute *repute)
{
	struct ptlrepute *current, *passed;

	current = list->last;
	passed = list->last;
	while (1) {
		if (current != NULL) {
			if (match_mapname(&(current->lastkey), &(repute->lastkey)))
				break;
			else if (match_mapname(&(current->contingentkey), &(repute->contingentkey)))
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	__builtin_printf("  repute (%x %x %x) %i dissolves in memory...\n",
		current->lastkey.hash.h, current->lastkey.hash.m,
		current->lastkey.hash.l, current->lastkey.seq);
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
