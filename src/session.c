/* session.c: Connect sessions for vrtater.  No basic functionality yet.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "session.h"
#include "global.h"
#include "stock.h" /* for now */

int generate_session_name_from_files_in_dir(session_t *, char *seedfiles);
int get_session_name_from_file_in_dir(session_t *, char *seedfiles);
int answer_session(void);
int complete_negotiation(session_desc_t *, session_t *last, session_t *new);
void read_from_network(void);
void rm_session_desc_list(void);
session_desc_t *add_session_desc(session_t *, btoggles_t *level, char *briefdesc, complextimate_t *);
void subtract_session_desc(session_desc_t *desc);

/* Called last when generating node-orgin, all_sessions list construct is
   already set up when this function is called. */
void
init_sessions(void)
{
	;
}

/* Using seedfiles produce session name referenced by name, also clobbering it's
   value to file seedfiles/uniqueness.  Return 0 if seedfiles/ had ordinary
   files therein, otherwise return 1.  If seedfiles/ is not a directory fail
   returning -1.  note: For now, use a pseudorandom session name of 16 bits with
   16 bits of trailing zero's, otherwise, same contents in seedfiles/ must 
   always produce same session name.  Currently session_t is of int type
   and has somewhat wimpy namespace leverage.  Expansion of the session name
   then desires a set of functions for setting and testing expanded to ease
   adjustment needed.  These then would need to be implemented or found.  The
   current situation will hopefully be adequate for testing. */
int
hash_session_name(session_t *name, char *seedfiles)
{
	*name = rand() << 16; /* for now */

	/* Clobber session name to seedfiles/uniqueness. */
	/* ... */

	return 0; /* for now */
}

/* Generate session name referenced by session from files in seedfiles/.  Return
   0, else -1 on error. */
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

/* Try to cue a determined session at url accepting local complexitimate
   lcomplex.  If successfull recieved .vrtater data is hmapunwrapf'd with
   produced hmap referenced through nodemap.  As a partial session number may
   change, for now the oneliner could be used to attain the nodemap.  Return 0
   on success else non-zero. */
int
call_session(char *url, complextimate_t *lcomplex, hmapf_t **nodemap)
{
	btoggles_t level;
	*nodemap = NULL;

	/* Try to cue on remote.  If successfull, cued session should then
	   appear in all_sessions.  Statefull maintainance function
	   sync_session, will need to poll any ip network input for responce.
	   A healthy sign will be an inbound nodemap following exchange of
	   relevant session_desc_s data.  note: There are several reasons why
	   this function might fail returning non-zero.  Some examples are:
	   nodemap name offered by remote node has hash part that conflicts in
	   local session namespace.  Timeout waiting for nodemap.  nodemap
	   goes on forever and a configured limit has been exceeded. etc... */
	/* ... */

#ifdef DIAG_DETERMINED_SESSION
	__builtin_printf(" node at url %s was called\n", url);
	level = VRT_MASK_SESSION_CALLED;

	/* Attempting to cue, here is the data that would be returned by node
	   _hw_node_ vs. local complextimate lcomplex through sync_sessions and
	   read_from_network at url. */
	session_t session = 0x80860000;
	char oneliner[] = "hw node"; /* 80 chars max */
	complextimate_t rcomplex; /* guessing */
	(&rcomplex)->hmap_count = 11;
	(&rcomplex)->tl_vdata = 8515;
	(&rcomplex)->tl_dialog = 18200;

	/* Cook up hmap result of would be hmapunwrapf'd .vrtater nodemap data.
	   note: stock.h, include above is needed until .vrtater data
	   available. */
	__builtin_printf(" recieved .vrtater nodemap data from %x\n",
		(int) session);
	hmapf_t *map;
	if ((map = hmapf_cylinder_c(&session, 80, 25, 111.5, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = .761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
		map->attribs.mode |= VRT_MASK_FIXED_FORM;
	} else
		return -2; /* no maps left */

	map->name = session | (map->name & 0x0000ffff); /* hmapunwrap'd */

	/* With all the data looking good, cue the session. */
	/* ... */
	level |= VRT_MASK_SESSION_CUED;
	__builtin_printf(" node at url %s was cued\n", url);
	if (session == (map->name & 0xffff0000))
		add_session_desc(&session, &level, oneliner, &rcomplex);
	else {
		__builtin_fprintf(stderr, "Error: nodemap vs session "
			"mismatch.\n");
		return -1;
	}
	diag_ls_partial_sessions(1);
	*nodemap = map;
#endif /* DIAG_DETERMINED_SESSION */

	return 0;
}

/* If answer_accept is true, try to cue a flexible session locally for a
   calling peer node.  Return 0 on success else non-zero. */
int
answer_session(void)
{
	answer_accept = 1; /* for now */
	/* ... */

	return 0;
}

/* Try to finish log in (an optional password may be implemented) and then add
   session forming to the active set of sessions with VRT_MASK_SESSION_PARTIAL
   true in all_sessions.  Local node has already recieved nodemap deriving
   partial space, and last will be either the last sent key or the backup key.
   keymap with keyname new will have entered disconnected partial in a
   pre-emptive way any will back out if login fails.  If successfull update
   outbound keyname reputation locally vs. cued_partial.  Return 0 on success.
   Connection is then assumed while session cued_partial remains in
   all_sessions data.  If cued_partial can not be connected return non-zero. */
int
form_determined_session(partial_t *forming, session_t *last, session_t *new, char *url)
{
	session_desc_t *desc;
	int rval = -1;

	if ((desc = find_in_all_sessions(&(forming->session))) !=  NULL) {
		/* Retrieve password locally, authenticate on remote. */
		/* ... */
		if((rval = complete_negotiation(desc, last, new)) == 0)
			sync_reputation(forming->ptlreps, last, new, url);
	}

#ifdef DIAG_DETERMINED_SESSION
	ptlrep_t *repute = find_repute(forming->ptlreps, new, 0);
	__builtin_printf("  repute data %x %x %x vs:\n   %s\n",
		repute->key, repute->bkpkey, repute->holdkey, repute->url);

	/* Looking at all_sessions and nodemap, one whom runs vrtater
	   accepts the cued session offered.  This would normally be done in
	   accept_called_session. */
	__builtin_printf(" Forming determined session %x \"%s\" with %i "
		"complexity\n",
		(int) desc->session, desc->oneliner,
		(sizeof(hmapf_t) * desc->complexity.hmap_count) +
		desc->complexity.tl_vdata + desc->complexity.tl_dialog);
#endif

	/* Add inbound partial session. */
	/* ... */
	desc->level |= VRT_MASK_SESSION_PARTIAL;

	return rval;
}

/* Try to add session cued_partial to the set of sessions having
   VRT_MASK_SESSION_PARTIAL true in all_sessions.  Return 0 on success.
   Connection is then assumed while session cued_partial remains in
   all_sessions data.  If cued_partial can not be connected return non-zero. */
int
form_flexible_session(session_t *cued_partial)
{
	session_desc_t *desc; /* for now */
	session_t last, new;
	partial_t *partial;
	ptlreps_list_t *list;
	int rval = -1;

	/* Some values that would be aquired by answer_session and be seen in
	   all_sessions by one whom is running vrtater. */
	desc->session = *cued_partial;
	desc->level = VRT_MASK_SESSION_CALLED | VRT_MASK_SESSION_CUED |
		VRT_MASK_SESSION_INBOUND;

	partial = find_partial(cued_partial);
	list = partial->ptlreps;

	if ((desc = find_in_all_sessions(cued_partial)) !=  NULL) {
		/* Retrieve password and last and new keyname's from remote.
		   for example: */
		last = (rand() << 16) | 8;
		new = (rand() << 16) | 8;
		/* Authenticate login with recieved password vs. partial.c
		   call find_repute. */
		/* ... */
		if((rval = complete_negotiation(desc, &last, &new)) == 0) {
			if(sync_reputation(list, &last, &new, NULL))
				desc->level |= VRT_MASK_SESSION_PARTIAL;
			else {
				/* Disconnect with: Im sorry, your...
				   because session hash is in use on node
				   partial->session.  Please regenerate hash
				   and try again. */
				/* ... */
				return -1;
			}
		}
	}

	return rval;
}

/* Using last and new keynames connect session described in desc.  Return 0 on success.  notes:
   Consideration for the eventuality that remote nodes may share the same url
   assumes that all nodes behind url may be allowed to be connected
   simutaniously if desc is accepted by one whom runs vrtater locally.  This
   means that the exchange of a list of session_desc_t data should be supported
   so that block of sessions then would all be connected.  session_desc_t
   therefore now includes session_t behind that specifies the node that session
   described is behind.  Any sessions calling from an ip after a connection
   exists to there are then automatically behind first existing one. */
int
complete_negotiation(session_desc_t *desc, session_t *last, session_t *new)
{
	/* ... */

#ifdef DIAG_DETERMINED_SESSION
	__builtin_printf(" sucessful login of %x at %x:\n%s\n",
		(int) *new, (int) desc->session, desc->oneliner);
#endif

	return 0;
}

/* Send and recieve .vrtater data while tending to any remote node sessions.
   notes: Peers in a partial need to apply the correct options to the wrap
   functions per inbound hmap.  This is automatically achieved by the wrap
   functions with an options field at the top of the .vrtater, after the count
   of bytes.  Wrap functions provide and recieve .vrtater file's as a list of
   int data. */
void
sync_sessions(void)
{
	;
}

/* Read to buffers from ip addr's associated with remote node sessions.
   Organize inbound .vrtater data to be unwrapped per partial by successive
   calls to recieve_maps_from_peer_partial. */
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

/* Per each singular .vrtater data parcel recieved and connected to a called
   or calling session thus represented by a node partial, hmapunwrapf
   parcel, returning NULL when there are zero available for session, otherwise
   returning counta hmap references in seta both referenced thru sel. */
hmapf_t *
recieve_maps_from_peer_partial(session_t *session, select_t *sel)
{
	hmapf_t *map = NULL;
	return map;
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
}

/* Remove all_sessions and all of it's elements and element references from
   memory. */
void
rm_session_desc_list(void)
{
	while (all_sessions->last != NULL)
		subtract_session_desc(all_sessions->last);
	free(all_sessions);
}

/* Add an element reference and provided session description details to the
   linked list construct all_sessions.  Return reference to element listed. */
session_desc_t *
add_session_desc(session_t *session, btoggles_t *level, char *oneliner, complextimate_t *complexity)
{
	session_desc_t *listed = NULL;

	if ((listed = (session_desc_t *) malloc(sizeof(session_desc_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for all_sessions entry\n",
			__FILE__, __LINE__);
		abort();
	}
	/* Description. */
	listed->session = *session;
	listed->level = *level;
	strcpy(listed->oneliner, oneliner);
	listed->complexity.hmap_count = complexity->hmap_count;
	listed->complexity.tl_vdata = complexity->tl_vdata;
	listed->complexity.tl_dialog = complexity->tl_dialog;

	listed->precursor = all_sessions->last;
	all_sessions->last = listed;
	all_sessions->count++;

	__builtin_printf(" session description added for \"%s\"\n", oneliner);

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

/* Called when program exits or otherwise, this should leave everything in an
   initial state yet be incapable of sending nor recieving outside of filescope
   until call_session is called or answer_session is called with answer_accept
   equal to 1. */
int
reset_sessions(void)
{
	/* Reset sequence. */
	/* ... */
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
			if (full) {
				if (current->level & VRT_MASK_SESSION_CALLED)
					__builtin_printf("CALLED ");
				if (current->level & VRT_MASK_SESSION_CUED)
					__builtin_printf("CUED ");
				if (current->level & VRT_MASK_SESSION_PARTIAL)
					__builtin_printf("PARTIAL ");
				if (current->level & VRT_MASK_SESSION_INBOUND)
					__builtin_printf("INBOUND ");
				else if ((current->level & VRT_MASK_SESSION_PARTIAL) && (!(current->level & VRT_MASK_SESSION_INBOUND)))
					__builtin_printf("OUTBOUND ");
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
