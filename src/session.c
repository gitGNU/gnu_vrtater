/* session.c: connect sessions for vrtater. no basic functionality yet.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include "session.h"

/* for now. these will soon be allocated when running */
session_desc_t a_all_sessions[VRT_MAX_CUED_SESSIONS];
prev_caller_sessions_t a_prev_caller_sessions[VRT_MAX_PREV_CALLER_SESSIONS];

int complete_negotiation(session_desc_t *);
session_desc_t *match_vs_all_sessions(session_t *);
int close_session(session_desc_t *);
void generate_session_name_from_files_in_dir(session_t *);
int get_session_name_from_file_in_dir(session_t *);

void
init_sessions(void)
{
	int i;
	for(i = 0; i < VRT_MAX_CUED_SESSIONS; i++)
		(&a_all_sessions[i])->session = 0;
	for(i = 0; i < VRT_MAX_PREV_CALLER_SESSIONS; i++)
		(&a_prev_caller_sessions[i])->session = 0;
}

/* read to buffers from ip addr's associated with remote node sessions */
void
read_from_network(void)
{
	;
}

/* send to from buffers and otherwise tend to any remote node sessions. */
void
session_sync(void)
{
	;
}

session_desc_t *
p_session_desc(void)
{
	return a_all_sessions;
}

prev_caller_sessions_t *
p_prev_caller_sessions(void)
{
	return a_prev_caller_sessions;
}

/* for now, produce a random session name/number 0-65536/0-65536 in session.
   notes: the session name produced/retrieved needs no connection; it will however
   appear in a_all_sessions[].  currently session_t is a a long long unsigned int.
   for now however, it is used as an int for simplicity.  when it does change
   there will be some adjustment needed.  a set of functions for converting and
   testing the eventual session name's may needs be implemented to ease this.  as
   it stands, vrt_hmaps_max must be <= 65536 and session uniqueness has a 1:65536
   chance of being unique.  this is pretty sub-optimal, however it will be fine
   for testing sessions. */
int
set_node_orgin(session_t *session)
{
	/* if not provided generate a node-orgin session from
	   .vrtater/session/ln_seedfiles/ */
	if (!(*session))
		*session = (session_t)(rand() << 16); /* for now */
	/* if any, backup session for node-orgin to .vrtater/session/ln_orgin/ */
	/* set current session for node-orgin in .vrtater/session/ln_session */
	return 0; /* for now */
}

int
set_node_partial(session_t *session)
{
	/* if any, backup last node-partial to .vrtater/session/ln_partial/ */
	/* if not provided generate a node-partial session from
	   .vrtater/session/ln_seedfiles/ */
	*session = (session_t)(rand() << 16);
	return 0; /* for now */
}

/* search for available nodes matching desc */
void
list_nodes(char *desc)
{
	;
}

/* this may cue a session for given list_nodes() remote node */
int
call_session(char *address)
{
	/* try to cue on remote with previous and current session numbers
	   if successfull, cued session will appear in a_all_sessions[]
	   statefull maintainance function will need to poll input for
	   responce */
	/* ... */
	return 0;
}

/* consideration for eventuality that remote nodes may share the same ip address
   assumes that all associated nodes may be allowed to be included to the running
   set if session_desc is accepted by the person running node-orgin.  this means
   that the exchange of a list of composite session_t data with it's
   session_desc_t data should be supported.  these then would all be included in
   the running set associated with ip, if the calling node session is accepted by
   node-orgin */
int
complete_negotiation(session_desc_t *session_desc)
{
	return 1;
}

session_desc_t *
match_vs_all_sessions(session_t *session_num)
{
	session_desc_t *p = a_all_sessions;
	int i;
	for(i = 0; i < VRT_MAX_CUED_SESSIONS; i++, p++)
		if((p->session == *session_num) | (p->session == 0))
			return p;
	__builtin_printf("session.c: non 0 terminated a_all_sessions[]");
	return 0;
}

/* finish log in (an optional password may be implemented) and then add
   session_num to the running set.  success is assumed while session_num remains
   in session_desc data */
int
accept_called_partial_session(session_t *session_num, char *passwd)
{
	session_desc_t *session_desc;
	int rval;
	/* return null on no match */
	if((session_desc = match_vs_all_sessions(session_num)) !=  NULL)
		rval = complete_negotiation(session_desc);
	return rval;
}

/* add session_num to the running set.  success is assumed while session_num
   remains in session_desc data */
int
accept_caller_partial_session(session_t *session_num)
{
	session_desc_t *session_desc;
	int rval;
	/* return null on no match */
	if((session_desc = match_vs_all_sessions(session_num)) !=  NULL)
		rval = complete_negotiation(session_desc);
	return rval;
}

/* close session */
int
close_session(session_desc_t *p)
{
	return 0;
}

int
close_sessions(session_desc_t *p)
{
	session_desc_t *q;
	q = p;

	while(q != NULL) {
		close_session(q);
		q++;
	}
	return 0;
}

/* set both ~/.vrtater/session/ln_session, pointer to, and ../related/uniqueness
   related file(s) may exist therein, else generate vanilla session name */
void
generate_session_name_from_files_in_dir(session_t *s)
{
	/* for now */
	*s = (0 | (session_t)(rand() << 16));
}

int
get_session_name_from_file_in_dir(session_t *s)
{
	int rval = 1;
	return rval;
}

/* partial sync
   from the callers perspective, outbound hmaps are referenced in selectf_a, and
   inbound one's are referenced out of selectf_b.  as caller will be calling for
   each partial seperately, input from associated partials is kept in a form
   conducive to this.  outbound hmaps include all partial(non-node-orgin) hmaps
   with any relevant changes.  these may be sent in reduced form and restored
   apon reception through a set of functions (transform.c) affecting network
   transfer and file formatting of hmap data, those being hmapwrapf and
   hmapunwrapf, currently scheduled to be extended for these purposes.
   sync_session assures that all peers in a session are using the same options
   to the wrap functions.  wrap functions provide int list input to and expect
   same output from buffers herein.  examples of hmaps with data dropable before
   send are: hmaps with positional data changes and no data changes(see:
   hmap->attribs bits VRT_MASK_VERTICE_MODS and VRT_MASK_DIALOG_MODS), magnitudes
   of vectors, hmap index and pointers to data(useless outside of node) ...

   currently planned are 2 buffers for each connected partial, one inbound and
   one out.

   outbound hmaps accumulate for a partial in callers list when relevant changes
   are generated locally per partial.  once per state increment the full set of
   these changed hmaps for each connected partial is delivered by code herein to
   each network address connected to that partial.  caller supplys these set's
   in succession per partial in the selection buffer as a NULL terminated list,
   then calls for each with associated session_t.  each set is then transformed
   by hmapwrapf into a format suitable for writing to the buffer then available
   through int pointer in transform.c.

   inbound buffer per partial (at implicit intervals) collects any changes
   originating on running peer nodes.  when complete_negotiation considers the
   premise of n nodes at same ip address no special consideration need be taken
   thereafter regards this issue save for assuring that all hmaps arrive in and
   leave from associated partials.  partial inbound buffers for completed frames
   of hmap data are converted to hmaps and read locally once per state increment
   apon caller supplying associated session_t.  after transform those hmaps
   reside in the selection buffer.  caller recieves NULL there if there are no
   relevant remotely generated changes for that state increment. */

/* send to each node connected to s, hmaps referenced by NULL terminated maps */
void
buffer_maps_to_peer_partial(session_t *s, select_t *sel)
{
	hmapf_t **p;
	int i;

	p = (hmapf_t **) (sel->seta);
	__builtin_printf(" session.c: pretending to buffer partial %x "
		"for transmit to another node,\n  buffering...\n",
		(int)*s);
	for (i = 0; i < sel->counta; i++, p++)
		__builtin_printf("  map %x\n", (int)((*p)->name));
		 
}

/* for any recieved hmap data unpacked thru transform and connected to s,
   write reference(s) in NULL terminated reference list setb.  return reference
   to setb or NULL if none */
hmapf_t *
recieve_maps_from_peer_partial(session_t *s, hmapf_t **setb)
{
	hmapf_t *rval = NULL;
	return rval;
}
