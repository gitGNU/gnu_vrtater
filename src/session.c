/* session.c: connect sessions for vrtater. no basic functionality yet.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include "session.h"
#include "progscope.h"

/* For now.  These will soon be allocated when running. */
session_desc_t all_sessions[VRT_MAX_CUED_SESSIONS];
caller_sessions_t caller_sessions[VRT_MAX_PREV_CALLER_SESSIONS];

int generate_session_name_from_files_in_dir(session_t *, char *seedfiles);
int get_session_name_from_file_in_dir(session_t *, char *seedfiles);
session_desc_t *match_vs_all_sessions(session_t *);
int complete_negotiation(session_desc_t *);
void read_from_network(void);

/* Called last when generating node-orgin. */
void
init_sessions(void)
{
	int i;
	for (i = 0; i < VRT_MAX_CUED_SESSIONS; i++)
		(&all_sessions[i])->session = 0;
	for (i = 0; i < VRT_MAX_PREV_CALLER_SESSIONS; i++)
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

/* Return reference to all currently called, cued, and running sessions. */
session_desc_t *
session_descriptions(void)
{
	return all_sessions;
}

/* Return reference to any previous caller sessions given in configured backup
   of node-partial.  Return NULL if none. */
caller_sessions_t *
previous_caller_sessions(void)
{
	return caller_sessions;
}

/* This may cue a session for given or list_nodes remote node ip address.
   Return zero on success.*/
int
call_session(char *address)
{
	/* Try to cue on remote with previous and current session numbers.
	   If successfull, cued session will appear in all_sessions.
	   Statefull maintainance function will need to poll input for
	   responce. */
	/* ... */

	return 0;
}

/* Finish log in (an optional password may be implemented) and then add
   session_num to the running set.  Return zero on success.  Connection
   is assumed while session_num remains in session_desc data.  If a cued
   session can not be connected return nonzero. */
int
accept_called_partial_session(session_t *session, char *passwd)
{
	session_desc_t *session_desc;
	int rval = -1;

	if ((session_desc = match_vs_all_sessions(session)) !=  NULL)
		if((rval = complete_negotiation(session_desc)) == 0);
			return rval;

	return rval;
}

/* Add session session_num to the running set.  Return zero on success.
   Connection is assumed while session_num remains in session_desc data.
   If a cued session can not be connected return nonzero. */
int
accept_caller_partial_session(session_t *session_num)
{
	session_desc_t *session_desc;
	int rval = -1;

	if ((session_desc = match_vs_all_sessions(session_num)) !=  NULL)
		if((rval = complete_negotiation(session_desc)) == 0);
			return rval;

	return rval;
}

/* Return reference to session description matching session or NULL if none. */
session_desc_t *
match_vs_all_sessions(session_t *session)
{
	session_desc_t *session_desc = all_sessions;
	int i;

	for (i = 0; i < VRT_MAX_CUED_SESSIONS; i++, session_desc++)
		if ((session_desc->session == *session))
			return session_desc;

	return NULL;
}

/* Connect session described in session_desc.  Return zero on success.
   notes: Consideration for the eventuality that remote nodes may share the same
   ip address assumes that all associated nodes may be allowed to be included in
   the running set if session_desc is accepted by the person running node-orgin.
   This means that the exchange of a list of composite session_t data with it's
   session_desc_t data should be supported.  These then would all be included in
   the running set associated with ip. */
int
complete_negotiation(session_desc_t *session_desc)
{
	return 0;
}

/* Send and recieve hmaps, and otherwise tend to any remote node sessions.
   notes: Peers in a partial need to apply the correct options to the wrap
   functions per incoming hmap.  This is achieved and used by the wrap functions
   with an options field after the header(count of bytes) in an hmap file.
   Wrap functions provide an hmap file as a list of integers and should receive
   input identical to what they would produce. */
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

/* Send to each node connected to session, hmaps referenced in sel selectf_a
   given sel counta.  notes: Outbound hmaps are referenced per call per given
   session in the running set, and include only non-node-orgin hmaps with
   relevant changes.  These may be sent in reduced form and restored apon
   reception through a set of functions (transform.c) affecting network
   transfer and file formatting of hmap data.  Functions hmapwrapf and
   hmapunwrapf are currently scheduled to be extended for this purpose.  Each
   set would then be transformed by hmapwrapf into a format suitable for writing
   to a buffer through int pointer passed.  These then become delivered for each
   ip address implied. */
void
buffer_maps_to_peer_partial(session_t *session, select_t *sel)
{
	hmapf_t **map;
	int i;

	map = sel->seta;
	__builtin_printf(" session.c: pretending to buffer partial %x "
		"for transmit to another node,\n  buffering...\n",
		*session);
	for (i = 0; i < sel->counta; i++, map++)
		__builtin_printf("  map %x\n", ((*map)->name));
}

/* For any recieved hmap data unpacked thru hmapunwrapf and connected to
   session, write reference(s) in sel selectf_b setting sel countb.  Return
   reference to selectf_b or NULL if none.  The data begins with it's size in
   bytes, followed by options and then session name.  notes:  As caller will be
   calling for each session seperately, int list input from session associated
   nodes could be kept in a form conducive to this instead of everything in one
   buffer.  It could thus follow that the hmaps would be best allocated and
   unwrapped as their data arrives, then keeping a linked list of references to
   those stacked in vohspace. */
hmapf_t *
recieve_maps_from_peer_partial(session_t *session, select_t *sel)
{
	hmapf_t *map = NULL;
	return map;
}

/* Close session referenced by session_desc.  Return zero on closed. */
int
close_session(session_desc_t *session_desc)
{
	return 0;
}
