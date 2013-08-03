/* session.c: connect sessions for vrtater. no basic functionality yet.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include "session.h"
#include "progscope.h"

/* For now.  These will soon be allocated when running. */
session_desc_t all_sessions[VRT_MAX_CUED_SESSIONS];
prev_caller_sessions_t prev_caller_sessions[VRT_MAX_PREV_CALLER_SESSIONS];

int generate_session_name_from_files_in_dir(session_t *);
int get_session_name_from_file_in_dir(session_t *);
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
		(&prev_caller_sessions[i])->session = 0;
}

/* For now, produce a random session name/number 0-65536/0-65536 in session.
   Return status to caller.  notes: The session name produced/retrieved needs no
   connection.  Given session will however appear in a_all_sessions[].
   Currently session_t is of int type.  This will require expansion somehow.
   When it does change, there will be some adjustment needed for new struct
   details chosen.  A set of functions for converting and testing the eventual
   session name's may thus needs be implemented or found.  As it stands,
   vrt_hmaps_max must be <= 65536 and session uniqueness has a 1:65536 chance
   of being unique.  This is pretty sub-optimal, however it will be fine for
   testing sessions. */
int
set_node_orgin(session_t *session)
{
	/* If not provided generate a node-orgin session from
	   .vrtater/session/ln_seedfiles/.  If ln_seedfiles/ is empty generate
	   new session name returning VRT_VANILLA_SESSION. */
	if (!(*session))
		*session = (session_t) (rand() << 16); /* for now */

	/* Backup node-orgin session .vrtater/session/ln_orgin/uniqueness to
	   .vrtater/session/ln_orgin/, if one is running. */
	/* ... */

	/* Set current session for node-orgin in .vrtater/session/ln_session. */
	/* ... */

	return 0;
}

/* For now, produce a random session name/number 0-65536/0-65536 in session.
   Return status to caller. */
int
set_node_partial(session_t *session)
{
	/* If any, backup last node-partial to .vrtater/session/ln_partial/. */
	/* ... */

	/* If not provided generate a node-partial session from
	   .vrtater/session/ln_seedfiles/.  If ln_seedfiles/ is empty generate
	   new session name returning VRT_VANILLA_SESSION. */
	*session = (session_t) (rand() << 16);

	return 0;
}

/* Set session and value referenced by ~/.vrtater/session/ln_session/uniqueness,
   to value generated from .vrtater/session/ln_seedfiles if file(s) exist
   therein, then returning zero.  Otherwise, generate new session name returning
   VRT_VANILLA_SESSION or something else for any error. */
int
generate_session_name_from_files_in_dir(session_t *session)
{
	*session = (0 | (session_t) (rand() << 16)); /* for now */

	return 0;
}

/* Get value referenced by ~/.vrtater/session/ln_session/uniqueness.  Set
   session to this value.  Return zero on success. */
int
get_session_name_from_file_in_dir(session_t *session)
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
   of node-partial.  Return null pointer if none. */
prev_caller_sessions_t *
previous_caller_sessions(void)
{
	return prev_caller_sessions;
}

/* This may cue a session for given or list_nodes remote node ip address.
   Return zero on success.*/
int
call_session(char *address)
{
	/* Try to cue on remote with previous and current session numbers.
	   If successfull, cued session will appear in a_all_sessions[].
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
accept_called_partial_session(session_t *session_num, char *passwd)
{
	session_desc_t *session_desc;
	int rval = -1;

	if ((session_desc = match_vs_all_sessions(session_num)) !=  NULL)
		if((rval = complete_negotiation(session_desc)) == SUCCESS);
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
		if((rval = complete_negotiation(session_desc)) == SUCCESS);
			return rval;

	return rval;
}

/* Return reference to session description matching session_num or null if
   not found. */
session_desc_t *
match_vs_all_sessions(session_t *session)
{
	session_desc_t *p = all_sessions;
	int i;

	for (i = 0; i < VRT_MAX_CUED_SESSIONS; i++, p++)
		if ((p->session == *session) | (p->session == 0))
			return p;

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
   functions per incoming hmap.  Wrap functions provide int list input to and
   expect same output from buffers herein. */
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
	hmapf_t **p;
	int i;

	p = (hmapf_t **) (sel->seta);
	__builtin_printf(" session.c: pretending to buffer partial %x "
		"for transmit to another node,\n  buffering...\n",
		(int) *session);
	for (i = 0; i < sel->counta; i++, p++)
		__builtin_printf("  map %x\n", (int) ((*p)->name));
}

/* For any recieved hmap data unpacked thru hmapunwrapf and connected to
   session, write reference(s) in sel selectf_b setting sel countb.  Return
   reference to selectf_b or NULL if none.  notes:  As caller will be calling
   for each session seperately, int list input from session associated nodes
   could be kept in a form conducive to this instead of everything in one
   buffer.  It could thus follow that the hmaps would be best allocated and
   unwrapped as their data arrives, then keeping a linked list of references to
   those. */
hmapf_t *
recieve_maps_from_peer_partial(session_t *session, select_t *sel)
{
	hmapf_t *rval = NULL;
	return rval;
}

/* Close session referenced by session_desc.  Return zero on closed. */
int
close_session(session_desc_t *session_desc)
{
	return 0;
}
