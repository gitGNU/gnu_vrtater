/* session.c: connect sessions for vrtater. no basic functionality yet.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include "progscope.h"
#include "hmap.h"

/* for now */
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
	for(i=0;i<VRT_MAX_CUED_SESSIONS;i++)
		(&a_all_sessions[i])->session = 0;
	for(i=0;i<VRT_MAX_PREV_CALLER_SESSIONS;i++)
		(&a_prev_caller_sessions[i])->session = 0;
}

/* statefully send to, retrieve from, and proc remote node session responces.
   outbound hmaps are referenced in selectf_a, and inbound one's are referenced
   out of selectf_b by caller.  outbound hmaps include all partial(non-in-node)
   hmaps with any changes.  these could be sent in reduced form and restored
   apon reception.  examples of hmaps with data dropable before send are: hmaps
   with positional data changes and no data changes(see: hmap->attribs bits
   VRT_MASK_VERTICE_MODS and VRT_MASK_DIALOG_MODS), magnitudes of vectors,
   hmap index and pointers to data(useless outside of node) ... */
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

/* for now, produce a random session name/number 0-65536/0-65536.  session_t
   is a a long long unsigned int.  for now however, it is used as an int for
   simplicity.  therefore vrt_hmaps_max must be <= 65536 and session uniqueness
   has a 1:65536 chance of being unique.  this is pretty sub-optimal, however
   it will be fine for testing in node sessions.  to call another node, one
   needs to have an in-node session.  these local sessions need no connection.
   they will however appear in a_all_sessions[] */
int
in_node_session(session_t *session)
{
	/* backup previous session for local node .vrtater/session/ln_session */
	/* generate session number for local node */
	*session = (session_t)(rand() << 16);
	/* set current session for local node .vrtater/session/ln_session */
	return 0; /* for now */
}

/* search for available nodes matching desc */
void
list_nodes(char *desc)
{
	;
}

/* this may cue a session for given list_nodes() remote node */
void
call_session(char *address)
{
	/* try to cue on remote with previous and current session numbers
	   if successfull, cued session will appear in a_all_sessions[]
	   statefull maintainance function will need to poll input for
	   responce */
	/* ... */
}

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
	for(i=0;i<VRT_MAX_CUED_SESSIONS;i++, p++)
		if((p->session == *session_num) | (p->session == 0))
			return p;
	__builtin_printf("session.c: non 0 terminated a_all_sessions[]");
	return 0;
}

/* finish log in, make running, given session.
   since inbound, disallow any in_node_session from having another, incl 0...
   success is assumed while implied session_t remains in session_desc data.
   reads from remote node will succeed with no data until sync or closed */
int
on_node_session(session_t *session_num)
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

/* note: first in node 'session 0' _can not_ be locked out(closed from here) */
int
close_sessions(session_desc_t *p)
{
	session_desc_t *q;
	q = p;
	/* examp */
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
