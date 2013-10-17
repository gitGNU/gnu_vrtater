/* session.c: Connect sessions for vrtater.  No basic functionality yet.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include "session.h"
#include "global.h"

int vrt_max_cued_sessions;
int vrt_max_called_sessions;
int vrt_max_caller_sessions;

int generate_session_name_from_files_in_dir(session_t *, char *seedfiles);
int get_session_name_from_file_in_dir(session_t *, char *seedfiles);
session_desc_t *match_vs_all_sessions(session_t *);
int complete_negotiation(session_desc_t *, session_t *last, session_t *new);
void read_from_network(void);

/* Called last when generating node-orgin. */
void
init_sessions(void)
{
	int i;

	/* For now allocate for connection specific session data as fixed
	   parcels.  Linked list mechanism to store these already written may
	   be found in commit c489481 files partial.c/partial.h and modified
	   for this purpose if desired.  Scheduled next.  After this function
	   is run, all_sessions and caller_sessions must be present until
	   close_sessions is run. */

	vrt_max_cued_sessions = 10;
	vrt_max_called_sessions = 20;
	vrt_max_caller_sessions = 100; /* record of current/previous calls */

	if ((all_sessions = (session_desc_t *) malloc(vrt_max_cued_sessions * sizeof(session_desc_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for all_sessions\n",
			__FILE__, __LINE__);
		abort();
	}

	if ((caller_sessions = (caller_sessions_t *) malloc(vrt_max_caller_sessions * sizeof(caller_sessions_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for caller_sessions\n",
			__FILE__, __LINE__);
		abort();
	}

	for (i = 0; i < vrt_max_cued_sessions; i++)
		(&all_sessions[i])->session = 0;

	for (i = 0; i < vrt_max_caller_sessions; i++)
		(&caller_sessions[i])->session = 0;
}

/* Produce session name referenced by session also clobbering it's value to file
   seedfiles/uniqueness and setting up a struct for session referenced in
   all_sessions.  If session referenced 0, implement a random session name of
   for now 16 bits with 16 bits of trailing zero's, else apply session given.
   Return 0 if seedfiles/ had ordinary files therein, otherwise return 1.  If
   seedfiles/ is not a directory fail returning -1.  If node-orgin changes
   replace node-orgin in all_sessions.  notes: Same contents in seedfiles/ must
   always produce same session name.  session name derived needs no connection
   even though it appears in all_sessions.  Currently session_t is of int type
   and has somewhat wimpy namespace leverage.  Expansion of the session name
   then desires a set of functions for converting and testing expanded to
   ease adjustment needed so these then would need to be implemented or found.
   The current situation will hopefully be adequate for testing. */
int
set_node_orgin(session_t *session, char *seedfiles)
{
	/* If not provided, generate a node-orgin session from seedfiles/. */
	if (!(*session))
		*session = rand() << 16; /* for now */

	/* Clobber session name to seedfiles/uniqueness. */
	/* ... */

	/* Write session info to all_sessions. */
	/* ... */

	return 0; /* for now */
}

/* Produce session name referenced by session also clobbering it's value to file
   seedfiles/uniqueness and setting up a struct for session referenced in
   all_sessions.  If session referenced 0, implement a random session name of
   for now 16 bits with 16 bits of trailing zero's, else apply session given.
   Return 0 if seedfiles/ had ordinary files therein, otherwise return 1.  If
   seedfiles/ is not a directory fail returning -1. */
int
set_node_partial(session_t *session, char *seedfiles)
{
	/* If not provided, generate a node-partial session from seedfiles/. */
	if (!(*session))
		*session = rand() << 16; /* for now */

	/* Clobber session name to seedfiles/uniqueness. */
	/* ... */

	/* Write session info to all_sessions. */
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

/* This may cue a session for given or list_nodes remote node url.
   Return a nodemap on success else NULL. */
hmapf_t *
call_session(char *url, complextimate_t *complextimate)
{
	hmapf_t *nodemap;

	nodemap = NULL;
	/* Try to cue on remote.  If successfull, cued session should then
	   appear in all_sessions.  Statefull maintainance function
	   sync_session, will need to poll any ip network input for responce.
	   A healthy sign will be an inbound nodemap following exchange of
	   relevant session_desc_s data. */
	/* ... */

	return nodemap;
}

/* This may cue a session for calling node. */
int
answer_session(complextimate_t *complextimate)
{
	hmapf_t *nodemap;

	nodemap = p_hmapf(23); /* nicenode */
	/* Try to add remote to cue.  If successfull, cued session should then
	   appear in all_sessions.  Statefull maintainance function
	   sync_session, has polled ip network input finding call for cue.
	   Send nodemap following exchange of relevant session_desc_s data. */
	/* ... */

	return 0;
}

/* Try to finish log in (an optional password may be implemented) and then add
   session cued_partial, to the running set.  Caller has already recieved
   nodemap deriving cued_partial and implicitly has last.  keymap with keyname
   last will have entered partial (nodemap) in a pre-emptive way any will back
   out if login fails, however keyname new will have been produced.  If
   successfull update outbound keyname reputation locally vs. cued_partial.
   Return 0 on success.  Connection is then assumed to be in the running set
   while session cued_partial remains in all_sessions data.  If cued_partial
   can not be connected return non-zero. */
int
accept_called_session(session_t *cued_partial, session_t *last, session_t *new)
{
	session_desc_t *desc; /* for now */
	char url[] = "protocol://192.168.0.2"; /* aquired by call_session */
	partial_t *partial;
	ptlreps_list_t *list;
	int rval = -1;

	/* Some values that would be aquired by call_session.  The person
	   running vrtater will see these values in all_sessions. */
	desc->session = *cued_partial;
	desc->level = VRT_MASK_SESSION_CALLED | VRT_MASK_SESSION_CUED;

	partial = find_partial(cued_partial);
	list = partial->ptlreps;

	if ((desc = match_vs_all_sessions(cued_partial)) !=  NULL) {
		/* Retrieve password locally, authenticate on remote. */
		/* ... */
		if((rval = complete_negotiation(desc, last, new)) == 0) {
			sync_reputation(list, last, new, url);
			desc->level |= VRT_MASK_SESSION_PARTIAL;
		}
	}

	return rval;
}

/* Try to add session cued_partial to the running set.  Return 0 on success.
   Connection is then assumed while session cued_partial remains in
   all_sessions data.  If cued_partial can not be connected return non-zero. */
int
accept_caller_session(session_t *cued_partial)
{
	session_desc_t *desc; /* for now */
	session_t last, new;
	partial_t *partial;
	ptlreps_list_t *list;
	int rval = -1;

	/* Some values that would be aquired by answer_session.  The person
	   running vrtater will see these values in all_sessions. */
	desc->session = *cued_partial;
	desc->level = VRT_MASK_SESSION_CALLED | VRT_MASK_SESSION_CUED |
		VRT_MASK_SESSION_INBOUND;

	partial = find_partial(cued_partial);
	list = partial->ptlreps;

	if ((desc = match_vs_all_sessions(cued_partial)) !=  NULL) {
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

/* Return reference to session description matching session or NULL if none. */
session_desc_t *
match_vs_all_sessions(session_t *session)
{
	session_desc_t *desc = all_sessions;
	int i;

	for (i = 0; i < vrt_max_cued_sessions; i++, desc++)
		if ((desc->session == *session))
			return desc;

	return NULL;
}

/* Connect session described in desc.  Return 0 on success.
   notes: Consideration for the eventuality that remote nodes may share the same
   url assumes that all associated nodes may be allowed to be included in the
   running set if desc is accepted by the person running node-orgin.
   This means that the exchange of a list of composite session_t data with it's
   session_desc_t data should be supported.  These then would all be included in
   the running set associated with ip. */
int
complete_negotiation(session_desc_t *desc, session_t *last, session_t *new)
{
	/* ... */

	return 0;
}

/* Send and recieve hmaps, and otherwise tend to any remote node sessions.
   notes: Peers in a partial need to apply the correct options to the wrap
   functions per incoming hmap.  This is achieved and used by the wrap functions
   with an options field in the header, after the count of bytes in a .vrtater
   file.  Wrap functions provide and recieve .vrtater file's as a list of
   int data. */
void
sync_sessions(void)
{
	;
}

/* Read to buffers from ip addr's associated with remote node sessions. */
void
read_from_network(void)
{
	;
}

/* Send to each node connected to session, counta hmaps in seta both referenced
   thru sel.  notes: Outbound hmaps are referenced per call for given session
   assumed to be set running in all_sessions data.  These happen to include only
   non-node-orgin hmaps with relevant changes.  These then become delivered for
   each url implied.  Values perhaps determined in sync_sessions could be
   returned here and parsed by caller for reflecting latency or disconnect for
   given remote session allowing caller to conditionally close session. */
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
   parcel, returning 0 when there are zero available for session, otherwise
   returning 1.  notes: As caller will be calling for each session seperately,
   int list parcel input from session associated nodes could be kept in a form
   conducive to this instead of say everything in one buffer.  Possibly commit
   c489481 files partial.c/partial.h could also be used here as well. */
hmapf_t *
recieve_maps_from_peer_partial(session_t *session, select_t *sel)
{
	hmapf_t *map = NULL;
	return map;
}

/* Close session referenced by desc.  Return 0 on closed. */
int
close_session(session_desc_t *desc)
{
	return 0;
}

/* Called when program exits. */
int
close_sessions(void)
{
	/* For now. */
	free(all_sessions);
	free(caller_sessions);

	return 0;
}
