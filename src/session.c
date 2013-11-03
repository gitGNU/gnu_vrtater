/* session.c: Connect sessions.  Simulated.  Lacks the most critical parts.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "session.h"
#include "global.h"
#include "transform.h"

int generate_session_name_from_files_in_dir(session_t *, char *seedfiles);
int get_session_name_from_file_in_dir(session_t *, char *seedfiles);
int complete_negotiation(session_desc_t *, session_t *loginkey, int keyuse);
void read_from_network(void);
void rm_session_desc_list(void);
void subtract_session_desc(session_desc_t *desc);

/* Called last when generating node-orgin, all_sessions list construct is
   already set up when this function is called. */
void
init_sessions(void)
{
	;
}

/* Using seedfiles produce session name referenced by name, also clobbering
   it's value to file seedfiles/uniqueness.  Return 0 if seedfiles/ had
   ordinary files therein, otherwise return 1.  If seedfiles/ is not a
   directory fail returning -1.  note: For now, use a pseudorandom session name
   of 16 bits with 16 bits of trailing zero's, otherwise, same contents in
   seedfiles/ must always produce same session name.  Currently session_t is of
   int type and has somewhat wimpy namespace leverage.  Expansion of the
   session name then desires a set of functions for setting and testing
   expanded to ease adjustment needed.  These then would need to be implemented
   or found.  The current situation will hopefully be adequate for testing. */
int
hash_session_name(session_t *name, char *seedfiles)
{
	*name = rand() << 16; /* for now */

	/* Clobber session name to seedfiles/uniqueness. */
	/* ... */

	return 0; /* for now */
}

/* Generate session name referenced by session from files in seedfiles/.
   Return 0, else -1 on error. */
int
generate_session_name_from_files_in_dir(session_t *session, char *seedfiles)
{
	return 0;
}

/* Copy session name in seedfiles/uniqueness to struct referenced by session.
   Return 0, else -1 on error. */
int
get_session_name_from_file_in_dir(session_t *session, char *seedfiles)
{
	return 0;
}

/* Search for available nodes matching desc.  note: This would be nice... */
void
list_nodes(char *desc)
{
	;
}

/* Try to cue 'continuing' session lsession, with node at url that accords with
   local complexitimate lcomplex.  If successfull, cued session should then
   appear in all_sessions and .vrtater data is hmapunwrapf'd with produced hmap
   referenced through nodemap.  note: As a partial session number may change,
   for now the oneliner could be used to attain the nodemap.  Statefull
   maintainance function sync_session, will need to read in any incoming ip
   network transfers.  A healthy sign will be exchange of relevant
   session_desc_s data followed by an inbound nodemap.  note: There are several
   reasons why this function might fail returning non-zero.  examples: nodemap
   name offered by remote node has hash part that conflicts in local session
   namespace.  Timeout waiting for nodemap.  lcomplex or nodemap data exceeds
   continuing partial maximum.  Return 0 on success else less than 0. */

int
call_session(char *url, complextimate_t *lcomplex, hmapf_t **nodemap, session_t *lsession)
{
	btoggles_t level;
	session_desc_t *desc;
	*nodemap = NULL;

	/* Send data for call attempt. */
	/* ... */

#ifdef DIAG_CONTINUING_SESSION
	/* Attempting to cue, here is the data that might be returned by node
	   _hw_node_ vs. local complextimate lcomplex through sync_sessions and
	   read_from_network at url. */
	session_t flexible = 0xd1610000;
	char oneliner[] = "hw node"; /* 80 chars max */
	complextimate_t rcomplex; /* guessing */
	(&rcomplex)->hmap_count = 8;
	(&rcomplex)->tl_vdata = 288;
	(&rcomplex)->tl_dialog = 314;

	__builtin_printf(" flexible node %x at url %s was called, responded\n",
		(int) flexible, url);
	level = VRT_MASK_SESSION_CALLED;

	/* Simulate a session description for a continuing partial.
	   Description will have session lsession on this side of connection
	   uniquely identifying continuing in partial.  Node called will be
	   flexible node hw node will be described with session 0xd1610000. */
	session_t from = 0; /* applies only in a flexible description */
	if ((desc = add_session_desc(lsession, &flexible, &from, &level, oneliner, lcomplex)) == NULL)
		return -1;
	diag_ls_partial_sessions(1);

	/* Save state, continue checking starting on next frame */
	/* ... */
	__builtin_printf(" waiting for nodemap\n");

	/* Simulate remote sends nodemap for non-cued session. */
	hmapf_t *map;
	session_t session = 0;
	hmapf_t **sela = (hmapf_t **) selectf_a;
	hmapf_t **selb = (hmapf_t **) selectf_b;
	select_t receiver = { 0, 1, sela, 0, selb };

	map = receive_map_from_peer_partial(&session, &receiver);
	__builtin_printf(" received .vrtater nodemap data from flexible with "
		"session %x\n", (int) flexible);

	map->name = flexible | (map->name & 0x0000ffff);

	/* Test if nodemap is within remote flexible. */
	if ((map->name & 0xffff0000) != flexible) {
		__builtin_fprintf(stderr, "Error: nodemap vs session "
			"mismatch.\n");
		return -2;
	}

	/* Now that the nodemap is available, the session is considered cued
	   while VRT_MASK_SESSION_CUED remains in the session description. */
	desc->level |= VRT_MASK_SESSION_CUED;
	__builtin_printf(" in cue on flexible node at url %s\n", url);
	diag_ls_partial_sessions(1);
	*nodemap = map;
#endif /* DIAG_CONTINUING_SESSION */

	return 0;
}

/* Try to log in to cued flexible session at url(an optional password may be
   implemented).  partial forming becomes 'partial' with the continued or
   new continued session and the flexible.  partial's are selected from
   descriptions appearing in all_sessions.  If successfull, continued session
   is then added to the set of sessions with VRT_MASK_SESSION_PARTIAL true
   locally in all_sessions.  Representational data described by hmap with
   session name loginkey will then become partial to forming partial using
   repute refrences if any.  If sync can not be established, no data is sent
   and any of data for loginkey present in disconnected partial has it's
   VRT_MASK_PARTIAL bit set false, any associated maps receiving selection and
   a session name within node-orgin.  If successfull update outbound keyname
   reputation locally vs. forming.  Return 0 on success.  Connection is then
   assumed while session name of forming remains in all_sessions data.  If
   forming can not be connected return non-zero. */
int
form_continuing_session(char *url, partial_t *forming, session_t *loginkey, ptlrep_t *repute)
{
	session_desc_t *desc;
	int keyuse, rval = -1;

#ifdef DIAG_CONTINUING_SESSION
	/* Looking at all_sessions, the nodemap, and any dialog therein, one
	   whom runs vrtater accepts the cued session offered.  This would
	   normally be done using accept_called_session. */
	__builtin_printf(" person running vrtater decides to log node\n");
#endif

	desc = find_in_all_sessions(&(forming->session));
	/* Retrieve password locally, authenticate on remote. */
	/* ... */

	/* Retreive from remote a value describing how the remote reputation
	   data was or was not modified, assigning this value to keyuse. */
	/* ... */
	keyuse = VRT_PARTIAL_LASTKEY; /* lastkey succeeded, usual case */

#ifdef DIAG_CONTINUING_REMOTE_SENDS_RETRY
	keyuse = VRT_PARTIAL_RETRY; /* loginkey given redundant, unusable */
#elif defined DIAG_CONTINUING_REMOTE_SENDS_KEYERR_LAST
	keyuse = VRT_PARTIAL_VALIDUSE; /* lastkey failed, validkey succeeded */
#elif defined DIAG_CONTINUING_REMOTE_SENDS_KEYERR_BOTH
	keyuse = VRT_PARTIAL_SYNCERR; /* both keys failed, recover */
#endif

	if (keyuse == VRT_PARTIAL_RETRY) {
		/* Login to remote flexible failed.  Retry.*/
		__builtin_printf("Redundant login key remotely.  Try again "
			"with something any bit different.\n");
		return -1;
	}
	/* Login to flexible succeeded.  Syncronize reputations locally. */
	if ((rval = sync_reputation(forming->ptlreps, repute, loginkey, &(repute->key), &(repute->bkpkey), url, keyuse)) != 0) {
		if (rval == VRT_PARTIAL_LASTKEY)
			__builtin_printf("Reputation found\n");
		else if (rval == VRT_PARTIAL_VALIDUSE)
			__builtin_printf("Reputation found\nnote: "
				"lastkey mismatch.\n");
		else if (rval == VRT_PARTIAL_SYNCERR) {
			__builtin_printf("Error: Continuing requires "
				"recovery.\nOption to simplify this, in "
				"ifnode interface, not yet added\n");
			return -1;
		} else
			__builtin_printf("New reputation.\n");
	}
	if ((rval = complete_negotiation(desc, loginkey, 0)) != 0)
		return -1;

#ifdef DIAG_CONTINUING_SESSION
	__builtin_printf("  reputation data: %x %x %x\n", repute->key,
		repute->bkpkey, repute->holdkey);

	__builtin_printf("Forming a continuing session %x now partial with "
		"\"%s\".\n", (int) desc->session, desc->oneliner);

	/* Add partial session. */
	/* ... */
	desc->level |= VRT_MASK_SESSION_PARTIAL;
#endif

	return 0;
}

/* If answer_accept is true, try to cue a flexible session locally for a
   calling peer node, providing a login for peer_session.  session will be
   refrenceable through all_sessions if not already, peer_session becoming
   likewise apon successfully returning.  If session is not already in the set
   of sessions with VRT_MASK_SESSION_PARTIAL set high, add it as such returning
   0, connection then being assumed thereafter while it remains true.  If
   peer_session can not be connected return non-zero. */
int
answer_session(session_t *session)
{
	partial_t *partial;
	session_t peer_session, loginkey, lastkey, validkey;
	session_desc_t *desc = NULL;
	session_desc_t *peer_desc = NULL;
	ptlreps_list_t *list;
	ptlrep_t *rrepute;
	int keyuse, rval = -1;

	partial = find_partial(session);
	list = partial->ptlreps;
	answer_accept = 1; /* for now */
	partial->ptlbits |= VRT_MASK_ANSWER_ACCEPT; /* for now */
	if (!answer_accept) {
		/* Nobody home.  Disconnect. */
		/* ... */
		return -1;
	} else if (!(partial->ptlbits & VRT_MASK_ANSWER_ACCEPT)) {
		/* Nobody home in that partial.  Disconnect. */
		/* ... */
		return -1;
	}

#ifdef DIAG_FLEXIBLE_SESSION
	/* Simulate session description values that would be retrieved or set
	   here. */
	peer_session = 0x330d0000; /* nicenode calling */
	session_t null = 0;
	loginkey = 0xc0de0008;
	lastkey = 0xb0de0008;
	validkey = 0xface0008;
	btoggles_t level = VRT_MASK_SESSION_CALLED;
	complextimate_t rcomplex; /* guessing */
	(&rcomplex)->hmap_count = 345;
	(&rcomplex)->tl_vdata = 7071;
	(&rcomplex)->tl_dialog = 51213;
	if ((peer_desc = add_session_desc(&peer_session, session, &null, &level, NULL, &rcomplex)) == NULL)
		return -1;
	/* Now the session is cued, but a session description is needed for the
	   simulation to set the cued bit on hw node flexible. */
	__builtin_printf(" accepting logins on flexible partial %x\n",
		(int) *session);
	peer_desc->level |= VRT_MASK_SESSION_CUED; /* nicenode assumed cued */
	__builtin_printf(" it seems like nanoseconds ago ...\n");
	/* Create a session description for flexible local node hwnode. */
	char oneliner[] = "hw node"; /* 80 chars max */
	level = VRT_MASK_SESSION_CALLED | VRT_MASK_SESSION_INBOUND;
	complextimate_t complex;
	(&complex)->hmap_count = 8;
	(&complex)->tl_vdata = 288;
	(&complex)->tl_dialog = 314;
	if ((desc = add_session_desc(session, &peer_session, &null, &level, oneliner, &complex)) == NULL)
		return -1;
	desc->level |= VRT_MASK_SESSION_CUED; /* hw node */
	if ((desc = find_in_all_sessions(session)) == NULL)
		return -1;
	diag_ls_partial_sessions(1);
#endif

	if ((peer_desc = find_in_all_sessions(&peer_session)) !=  NULL) {
		__builtin_printf(" \"%s\" authenticating login\n",
			desc->oneliner);
		/* Optional password authentication with remote, if added.
		   For given partial, option is specified in partial_t member
		   ptlbits, masked by VRT_MASK_REQUIRE_PASSWD. */
		/* ... */

#ifdef DIAG_FLEXIBLE_SESSION
		/* Assuming that this is running on hw node, create simulated
		   and already active local keymap info for avatar8, placed in
		   a local reputation. */
		session_t synclastkey, syncvalidkey;
		synclastkey = 0xb0de0008;
		syncvalidkey = 0xface0008;
		hmapf_t *holdmap = p_hmapf(26);
		rrepute = add_ptlrep(list, &synclastkey, NULL);
		rrepute->bkpkey = syncvalidkey;
		rrepute->holdkey = holdmap->name;
#endif
#ifdef DIAG_FLEXIBLE_KEYERR
		/* Simulate keysync error from a disk or memory related event.
		   This key (non-bkpkey) was successfully used by remote,
		   however it has an error either remotely or locally somehow
		   after likely write to disk or memory, or having never
		   been written to disk before a reset or power-off. */
		rrepute->key = 0xfaded000; /* inverse test also valid */
#endif
#ifdef DIAG_FLEXIBLE_BKPKEYERR
		/* Simulate keysync error from a disk or memory related event.
		   This key was successfully used possibly becoming shifted
		   (system may have gone down during shift).  When possibly
		   written to disk then, or in memory either remotely or
		   locally, it somehow was in error. */
		rrepute->bkpkey = 0xfaded000; /* inverse test also valid */
#endif
#ifdef DIAG_FLEXIBLE_NEWREPUTE
		lastkey = 0xffffffff;
		validkey = 0xffffffff;
#endif
		/* Some keyuse values for sync_reputation, see header.
		   VRT_PARTIAL_RETRY.  As a flexible partial receiving logins,
		   loginkey given may be used by another reputation.
		   VRT_PARTIAL_SYNC_*.  When any login arrives, it should be
		   rejected if it's lastkey or validkey matches on any lastkey
		   or respective validkey of any keymap that is in the
		   reputation data read from disk or generated while running.
		   This protect's the sync for all further login's.  Also,
		   logins that are archived due to disuse will need to be
		   exclusive or they will require interactive recovery. */

		/* Syncronize reputations locally vs. login from remote
		   continuing partial.  Tend to any issues. */
		rval = sync_reputation(list, rrepute, &loginkey, &lastkey, &validkey, NULL, keyuse);
		if ((rval = complete_negotiation(peer_desc, &loginkey, rval)) != 0)
			return rval;

		/* Add partial session. */
		/* ... */
		desc->level |= VRT_MASK_SESSION_PARTIAL;
		peer_desc->level |= VRT_MASK_SESSION_PARTIAL; /* assumed cued */
		diag_ls_partial_sessions(1);
	}

	return 0;
}

/* Using last and new keynames connect session described in desc.  Return 0 on
   success else nonzero.  notes: Consideration for the eventuality that remote
   continuing nodes may share the same url assumes that all nodes behind url
   may be allowed to be connected simutaniously if desc is accepted by one whom
   runs vrtater locally.  This means that the exchange of a list of n
   session_desc_t data should be supported so that block of sessions then would
   all be connected.  session_desc_t therefore now includes session_t from,
   that specifies the node that session described is from.  For a flexible
   node, any sessions calling for a login out of node at url after a continuing
   partial session exists there are then automatically behind continuing. */
int
complete_negotiation(session_desc_t *desc, session_t *loginkey, int keyuse)
{
#ifdef DIAG_FLEXIBLE_SESSION
	ptlrep_t *reputed;
	/* Messages to remote.  If this is a local flexible partial, forward
	   keyuse value to peer and ... */
	if (!(desc->level & VRT_MASK_SESSION_INBOUND)) {
		if (keyuse == VRT_PARTIAL_RETRY) {
			__builtin_printf("Sending message:\n Redundant "
			"loginkey.  Try again with something any bit "
			"different.\n");
			/* Send VRT_PARTIAL_RETRY. */
			return keyuse;
		} else if (keyuse == VRT_PARTIAL_NEWREPUTED) {
			__builtin_printf("Sending message:\n New "
				"reputation on %x.\n", desc->peer);
			/* Send VRT_PARTIAL_NEW_REPUTED. */
		} else if (keyuse == VRT_PARTIAL_SRCHMAPKEY) {
			__builtin_printf("Sending message:\n Continuing "
				"lastkey session.\n");
			/* Send VRT_PARTIAL_LASTKEY vs. ..._SRCHMAPKEY. */
		} else if (keyuse == VRT_PARTIAL_VALIDUSE) {
			__builtin_printf("Sending message:\n Continuing "
				"session. note: validkey usage.\n");
			/* Send VRT_PARTIAL_VALIDUSE. */
		} else if (keyuse == VRT_PARTIAL_SYNCERR) {
			__builtin_printf("Sending message:\n Oops! A highly "
				"unlikely pair of exclusive errors has "
				"occured on one or both\n node systems.  "
				"Recovery option (if added) is required to "
				"continue session.\n See ifnode interface for "
				"more.\n");
			/* Send VRT_PARTIAL_SYNCERR. */
			return keyuse;
		}
	}

	/* Login succeeded.  On the flexible node side, one line description of
	   the node now logged is the NULL string. */
	if (desc->oneliner) {
		__builtin_printf(" successfull login by %x on %x\n",
			(int) *loginkey, (int) desc->peer);
		/* Send nodemap and ... */
	} else
		__builtin_printf(" successfull login of %x at %x\n",
			(int) *loginkey, (int) desc->peer);
#endif

	return 0;
}

/* Send and receive .vrtater data while tending to any remote node sessions.
   notes: Peers in a partial need to apply the correct options to the wrap
   functions per inbound hmap.  This is automatically achieved by the wrap
   functions with an options field at the top of the .vrtater, after the count
   of bytes.  Wrap functions provide and receive .vrtater file's as a list of
   int data. */
void
sync_sessions(void)
{
	;
}

/* Read to buffers from ip addr's associated with remote node sessions.
   Organize inbound .vrtater data to be unwrapped per partial by successive
   calls to receive_maps_from_peer_partial. */
void
read_from_network(void)
{
	;
}

/* Send to each node connected to session, counta hmaps in seta both referenced
   thru sel.  notes: Outbound hmaps are referenced per call for given session
   assumed to have VRT_MASK_SESSION_PARTIAL true in all_sessions description.
   These happen to include only non-node-orgin hmaps with relevant changes.
   These then become delivered for each url implied.  Values perhaps determined
   in sync_sessions could be returned here and parsed by caller for reflecting
   latency or disconnect for given remote session allowing caller to
   conditionally close session. */
int
buffer_maps_to_peer_partial(session_t *session, select_t *sel)
{
	hmapf_t **map;
	int i;

	map = sel->seta;
	__builtin_printf(" session.c: pretending to buffer partial %x "
		"for transmit to another node,\n  buffering...\n", *session);
	for (i = 0; i < sel->counta; i++, map++)
		__builtin_printf("  map %x\n", ((*map)->name));

	return 0;
}

/* Per each singular .vrtater data parcel received and connected to a called
   or calling session thus represented by a node partial, hmapunwrapf
   parcel, returning NULL when there are zero available for session, otherwise
   returning counta hmap references in seta both referenced thru sel.  note:
   hmapwrapf wraps from selection buffer a, while hmapunwrapf unwraps to b. */
hmapf_t *
receive_map_from_peer_partial(session_t *session, select_t *sel)
{
	hmapf_t *selected, *map = NULL;
	int buf_len, *int_buf, **buf = &int_buf;

	/* read .vrtater data then refrenced by int_buf from ... */
	/* ... */

#ifdef DIAG_CONTINUING_SESSION
	/* Need a .vrtater to simulate receiving so for diagnostic, wrap
	   an hmap from selection buffer a, simulating received data that
	   is then referenced in int_buf. */
	map = p_hmapf(0);
	*(sel->seta) = map;
	buf_len = hmapwrapf(sel, 0, NULL, buf); /* buf passed, int_buf set */
	/* int_buf is now allocated by hmapwrapf */
	/* For test change the session name of the map so that it does
	   not conflict thus unwrapping seperately as a new map.  This
	   is required until the swap_hmaps function is written. */
#endif

	/* Using argued session, receive map(s).  sel->countb holds hmap count
	   received. */
	if (hmapunwrapf(sel, session, NULL, (int *) int_buf) < 0)
		__builtin_fprintf(stderr, "Error: .vrtater data received did "
			"not hmapunwrapf as expected.");
	else
		__builtin_printf("%i hmaps unwrapped from node %x\n",
			sel->countb, (int) *session);

#ifdef DIAG_CONTINUING_SESSION
	map = *(sel->setb);
	map->vpos.z -= 5.; /* adjust map for visibility (very tiny map) */
	free(int_buf); /* !!always free after hmapwrap wrap's .vrtater data. */
#endif

	return *(sel->setb);
}

/* Return reference to session description matching session or NULL if none. */
session_desc_t *
find_in_all_sessions(session_t *session)
{
	session_desc_t *current, *passed;

	current = all_sessions->last;
	passed = all_sessions->last;
	while (1) {
		if (current != NULL) {
			if (current->session == *session)
				return current;
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Create linked list construct for all_sessions returning reference to an
   empty list of session descriptions residing therein. */
void
mk_session_desc_list(void)
{
	all_sessions = NULL;
	if ((all_sessions = (session_desc_list_t *) malloc(sizeof(session_desc_list_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for all_sessions "
			"list construct\n",
			__FILE__, __LINE__);
		abort();
	}
	all_sessions->last = NULL;
	all_sessions->count = 0;

	__builtin_printf(" session description list added\n");
}

/* Remove linked list wrapper struct all_sessions from memory. */
void
rm_session_desc_list(void)
{
	free(all_sessions);
}

/* Add an element reference and provided session description details to the
   linked list construct all_sessions.  Return reference to element listed. */
session_desc_t *
add_session_desc(session_t *session, session_t *peer, session_t *from, btoggles_t *level, char *oneliner, complextimate_t *complexity)
{
	session_desc_t *listed = NULL;

	if (find_in_all_sessions(session)) {
		__builtin_printf("Error: Can not create session "
			"description as session %x already exists\n",
			(int) *session);
		return NULL;
	}

	if ((listed = (session_desc_t *) malloc(sizeof(session_desc_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for all_sessions entry\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->precursor = all_sessions->last;
	all_sessions->last = listed;
	all_sessions->count++;

	/* Description. */
	listed->session = *session;
	listed->peer = *peer;
	listed->from = *from;
	listed->level = *level;
	if (oneliner)
		strcpy(listed->oneliner, oneliner);
	listed->complexity.hmap_count = complexity->hmap_count;
	listed->complexity.tl_vdata = complexity->tl_vdata;
	listed->complexity.tl_dialog = complexity->tl_dialog;

	if (oneliner)
		__builtin_printf(" session description added for "
			"\"%s\"\n", oneliner);
	else
		__builtin_printf(" session description added\n");

	return listed;
}

/* Subtract linked list element referenced by desc. */
void
subtract_session_desc(session_desc_t *desc)
{
	session_desc_t *current, *passed;

	current = all_sessions->last;
	passed = all_sessions->last;
	while (1) {
		if (current != NULL) {
			if (current->session == desc->session)
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	__builtin_printf(" removing session description %x from all_sessions\n",
		(int) current->session);
	if (current == passed) {
		if (!current->precursor)
			all_sessions->last = NULL;
		else
			all_sessions->last = current->precursor;
	} else
		passed->precursor = current->precursor;
	free(current);
	all_sessions->count--;
}

/* Close session referenced by desc.  Return 0 on closed. */
int
close_session(session_desc_t *desc)
{
	/* Disconnect sequence. */
	/* ... */
	subtract_session_desc(desc);

	return 0;
}

/* Remove all element refrences of all_sessions.  Return 0 on closed. */
int
close_all_sessions(void)
{
	while (all_sessions->last != NULL)
		close_session(all_sessions->last);

	return 0;
}


/* Called when program exits or otherwise, this should leave everything in an
   initial state yet be incapable of sending nor receiving until call_session
   is called or answer_accept is equal to 1.  Caller will create a new session
   description list before either above case.  Return 0 on success else
   non-zero. */
int
reset_sessions(void)
{
	/* Reset sequence. */
	/* ... */
	close_all_sessions();
	rm_session_desc_list();

	return 0;
}

/* Temporary diagnostic to list partial sessions to stdout.  If full is
   nonzero, describe each session as well. */
void
diag_ls_partial_sessions(int full)
{
	session_desc_t *current, *passed;

	if (all_sessions->count == 0) {
		__builtin_printf("--no partial sessions listed--\n");
		return;
	}

	current = all_sessions->last;
	passed = all_sessions->last;
	while (1) {
		if (current != NULL) {
			__builtin_printf("%x ", (int) current->session);
			__builtin_printf("%x ", (int) current->peer);
			__builtin_printf("%x ", (int) current->from);
			if (full) {
				if (current->level & VRT_MASK_SESSION_CALLED)
					__builtin_printf("CALLED ");
				if (current->level & VRT_MASK_SESSION_CUED)
					__builtin_printf("CUED ");
				if (current->level & VRT_MASK_SESSION_PARTIAL)
					__builtin_printf("PARTIAL ");
				if (current->level & VRT_MASK_SESSION_INBOUND)
					__builtin_printf("FLEXIBLE ");
				else if ((current->level & VRT_MASK_SESSION_PARTIAL) && (!(current->level & VRT_MASK_SESSION_INBOUND)))
					__builtin_printf("CONTINUING ");
				__builtin_printf("\"%s\" ", current->oneliner);
				__builtin_printf("%i ", (sizeof(hmapf_t) * current->complexity.hmap_count) + current->complexity.tl_vdata + current->complexity.tl_dialog);
			}
			__builtin_printf("\n");
			passed = current;
			current = current->precursor;
		} else
			return;
	}
}
