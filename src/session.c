/* session.c: Connect sessions.  Simulated.  Still lacks most critical parts.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "session.h"
#include "global.h"
#include "transform.h"

int keyuse_feedback(session_desc_t *, session_t *loginkey, int keyuse);
void read_from_network(void);
void rm_session_desc_list(void);
void subtract_session_desc(session_desc_t *desc);

#ifdef DIAG_FLEXIBLE_SESSION
hmapf_t *diag_receive_nodemap(session_t *, select_t *);
#endif

/* Called after node-orgin is set up.  all_sessions list construct is already
   present and contains node-orgin and any flexible nodes read from config. */
void
init_sessions(void)
{
	;
}

/* Using seedfiles produce a 96 bit hash value, writing it to name->hash.h,
   name->hash.m and name->hash.l, also writing it's value to file
   seedfiles/uniqueness.  Using search_vohspace, very soon to be extended for
   this purpose, if name->hash.l is not unique in vohspace vs. any other
   name->hash.l, try again after first modding the contents of seedfiles/,
   perhaps adding the just generated name->hash.l as a file of name
   uniqueness_artifact to seedfiles/ (if not already present).  Return 0 if
   seedfiles/ had ordinary files therein or 1 if it is empty, otherwise < 0. */
int
hash_session_name(session_t *name, char *seedfiles)
{
	/* Simulate. */
	name->hash.h = 0;
	name->hash.m = 0;
	name->hash.l = rand();

	/* Write session name to seedfiles/uniqueness. */
	/* ... */

	return 0;
}

/* Read session name in seedfiles/uniqueness into session.  Return 0, else < 0
   on error. */
int
read_hash(session_t *session, char *seedfiles)
{
	return 0;
}

/* For phase-a of continuing partial, send request to url for continuity in
   flexible node at url.  Request contains url, oneliner if available,
   session_here, and possibly session_thru if continuing is being forwarded.
   flexible at url may then provide continuity represented by returned,
   possibly updated oneliner describing node, any updated url, flexible's
   session number, complextimate representing arrival metaloc, and a nodemap
   referenced through maps.  Return 0 on success else < 0.  note: If
   successfull the following occurs.  level bit for continuity in session_here
   becomes set.  If the url has changed, a new session description for the
   continuing node is added to all_sessions while the unlinked level bit on
   the previous one is set.  Around this time a single .vrtater data element
   is received and hmapunwrapf'd referenced as nodemap.  After calling
   successfully and waiting for nodemap, caller then calls mk_partial using
   session_here and given nodemap in continuing context.  During a configured
   lengthly timeout flexible session continuity normally remains valid awaiting
   call to continue_node for phase-b syncronization.  During this time the
   person running vrtater may evaluate the nodemap sent, then possibly
   continuing partial with the flexible bringing any hmaps selected.  This may
   fail yet the flexible session providing continuity will by default renew the
   timeout if any, allowing successive tries with lower complextimate or
   regenerated session_here hash.  further notes: Flexible session hash will
   usually be subject to uptime, the url may also change, so to the 79 chars
   max oneliner.  The session description created with received data is used
   by the ifnode**.c interface part to select flexible's.  The url must be
   unique in all_sessions and is used to partial.  Parts or the whole of the
   oneliner may be present in the url possibly allowing extraction of the
   oneliner from there optionally, however the oneliner always represent's the
   specific flexible node.  By default allowing multiple nodes at same url. */
int
node_continuity(char *url, char *oneliner, session_t *session_here, session_t *session_thru, hmapf_t **maps)
{
	session_desc_t *desc_here = NULL;
	session_t z = { { 0, 0, 0 }, 0 };
	session_t session_peer = { { 0, 0, 0 }, 0 };
	complextimate_t cmplxt_peer, cmplxt_here = { 0, 0, 0 };
	hmapf_t *nodemap;
	hmapf_t **sela = (hmapf_t **) selectf_a;
	hmapf_t **selb = (hmapf_t **) selectf_b;
	select_t receiver = { 0, 1, sela, 0, selb };

	*maps = NULL;

	__builtin_printf(" calling for continuity on flexible node (%x %x %x) "
		"%s\n", (&session_peer)->hash.h, (&session_peer)->hash.m,
		(&session_peer)->hash.l, url);

	/* Send data for continuity attempt.  Resume instructions following
	   if continuity succeeds. */
	/* ... */

	/* Simulating retreival of continuity, here is data that might be
	   returned from flexible node nothernode.  This is then written to
	   all_sessions as a session description, added or changed reflecting
	   latest details.  all_sessions is accessable from a menu in the
	   ifnode**.c interface part for reading arguments passed to
	   continue_node. */
	char newurl[] = ""; /* usually */
	char newoneliner[] = "nothernode";
	(&session_peer)->hash.l = 0xca11ed;
	(&cmplxt_peer)->hmap_count = 8;
	(&cmplxt_peer)->tl_vdata = 288;
	(&cmplxt_peer)->tl_dialog = 314;

	__builtin_printf("  ...flexible node description data received.\n");

	/* Check if too complex. */
	/* ... */

	/* For simulation desc_here needs to be created while normally it
	   will already exist.  This type of all_sessions description exists
	   as a continuing session with no implied node yet in node_orgin,
	   either read from disk or created by continue_repute.  This is
	   not the same as the description of a peer of a flexible, but
	   similar.  For now forego updating descriptions.   Later, set
	   unlinked level bit in an old description before adding updated one.
	   Person running vrtater will be able to restore an old description
	   in an unusual case, provided they are not logged in to node
	   represented by updated. */
	/* ... */
	if ((desc_here = add_session_desc(session_here, &session_peer, &z, VRT_MASK_SESSION_CALLED, url, newoneliner, &cmplxt_here, NULL, NULL)) == NULL)
		return -1;
	diag_ls_all_sessions(1);

	/* Retreive nodemap now or in a later pass. */
	__builtin_printf(" waiting for nodemap...\n\n");
	/* ... */

	/* Save state.  Continue checking resuming instructions following on
	   nodemap per this session. */
	/* ... */

	/* Simulate reception of continuity. */
	nodemap = diag_receive_nodemap(session_here, &receiver);
	__builtin_printf(" received .vrtater nodemap data from flexible node "
		"with session (%x %x %x)\n", (&session_peer)->hash.h,
		(&session_peer)->hash.m, (&session_peer)->hash.l);
	cp_session(&session_peer, &(nodemap->name));

	/* Set continuity. */
	if (match_session(&(nodemap->name), &session_peer)) {
		desc_here->level |= VRT_MASK_SESSION_CONTINUITY;
		__builtin_printf(" continuity on flexible node at url %s\n",
			url);
		*maps = nodemap;
		diag_ls_all_sessions(1);
	} else {
		__builtin_fprintf(stderr, "Error: nodemap vs session "
			"mismatch.\n");
		return -2;
	}

	return 0;
}

/* For phase-b of continuing partial, send syncronization key data in repute
   to url for loginkey in session_here along with any further details for
   session_here, then requesting login authentication if added.  A value
   is then fed back from flexible and used to syncronize reputatations in list.
   If successfull, continuing node with session_here becomes partial with a
   flexible node that retreives continuity.  Return value fed back else < 0.
   note: Optional password may automatically succeed based on the sense of
   the usepasswd level bit maintained flexible session description.  Reputation
   data is treated in loginkeys.c.  Reputations data (repute) is composed of
   session numbers with sequence indicies of hmap keymaps that were previously
   partial with a flexible, or are currently vrtlogging, these maintained on
   both sides of partial.  complextimate data is always set in advance of
   calling this function.  Therein is derived size of data that will transfer
   if implied vrtlogin succeeds.  A keymap is an hmap that represents all the
   hmaps from a continuing node in a given partial.  It's mapname is used as
   loginkey. */
int
continue_node(char *url, struct ptlrepute_list *list, struct ptlrepute *repute, session_t *session_here, session_t *loginkey, session_t *holdkey, unsigned int tl_cmplxt)
{
	session_desc_t *desc = NULL;
	struct ptlrepute *unique_repute;
	int keyuse, rval = -1;

	/* Looking at all_sessions, the nodemap, and any dialog therein, person
	   running vrtater decides to accept session desc->peer indexed by
	   session_here, as VRT_MASK_SESSION_CONTINUITY remains true. */
	__builtin_printf(" person running vrtater decides to log node\n");

	/* Likely passed in as arg. */
	if ((desc = find_session(session_here)) == NULL) {
		__builtin_printf("continue_node cant find_session\n");
		abort();
	}
	if (!(desc->level & VRT_MASK_SESSION_CONTINUITY))
		return -1;

	/* Send sync_vrtlogin keydata in repute to flexible. Implicit are url
	   and session, to reference earlier exchange.  Inclusive are loginkey,
	   holdkey, repute->lastkey, repute->contingentkey, and tl_cmplxt, . */
	/* ... */

	/* If password added, fork initiating password authentication with url
	   that determines flexible node connection.  Resume instructions
	   following if successfull and keyuse value is returned. */
	/* ... */
#ifdef DIAG_CONTINUING_SESSION
	__builtin_printf(" continuing loginkeys sent to flexible\n\tloginkey "
		"(%x %x %x) %i\n\tlastkey (%x %x %x) %i  if error assume it's "
		"on flexible side\n\tcontingentkey (%x %x %x) %i  if error "
		"assume it's on flexible side\n\tholdkey (%x %x %x) %i  "
		"update if not zero_mapname\n\tholdbkp (%x %x %x) %i  for now "
		"zero_mapname during diagnostic\n", loginkey->hash.h,
		loginkey->hash.m, loginkey->hash.l, loginkey->seq,
		repute->lastkey.hash.h, repute->lastkey.hash.m,
		repute->lastkey.hash.l, repute->lastkey.seq,
		repute->contingentkey.hash.h, repute->contingentkey.hash.m,
		repute->contingentkey.hash.l, repute->contingentkey.seq,
		holdkey->hash.h, holdkey->hash.m, holdkey->hash.l,
		holdkey->seq, repute->holdbkp.hash.h, repute->holdbkp.hash.m,
		repute->holdbkp.hash.l, repute->holdbkp.seq);
#endif /* DIAG_CONTINUING_SESSION */

	/* Simulate a keyuse value fed back from flexible and related to this
	   session arriving, describing how reputation data on flexible was or
	   was not modified. */
	keyuse = VRT_LOGIN_LAST; /* lastkey succeeded, usual case */

#ifdef DIAG_CONTINUING_FLEXIBLE_SENDS_RETRY
	keyuse = VRT_LOGIN_RETRY; /* newly given login or holdkey redundant */
#elif defined DIAG_CONTINUING_FLEXIBLE_SENDS_LASTKEYERR
	keyuse = VRT_LOGIN_CONTINGENT; /* lastkey failed, contingent used */
#elif defined DIAG_CONTINUING_FLEXIBLE_SENDS_BOTHKEYERR
	keyuse = VRT_LOGIN_SYNCERR; /* both keys failed, recover */
#elif defined DIAG_CONTINUING_FLEXIBLE_SENDS_NEWREPUTED
	keyuse = VRT_LOGIN_NEWREPUTED; /* both sync keys zero_mapname */
#endif

	/* Mapping keyuse determinacy, try to make session partial. */
	if ((rval = sync_loginkeys(url, list, repute, loginkey, holdkey, &(repute->lastkey), &(repute->contingentkey), keyuse))) {
		if ((rval == VRT_LOGIN_RETRY) || (rval == VRT_LOGIN_SYNCERR)) {
			__builtin_printf("given keyuse %i sync_loginkeys "
				"returns %i\n", keyuse, rval); /* diag */
			return -2;
		}
	}

#ifdef DIAG_CONTINUING_SESSION
	__builtin_printf(" keyuse given %i sync returned %i\n", keyuse, rval);
	if ((unique_repute = find_lastkey(list, loginkey)) == NULL) {
		__builtin_printf("continue_node cant find_lastkey\n");
		abort();
	}
	__builtin_printf(" continuing loginkey data after sync\n\tlastkey "
		"(%x %x %x) %i\n\tcontingentkey (%x %x %x) %i  "
		"(key = ovrflo ? match : !!mismatch)\n"
		"\tholdkey (%x %x %x) %i  empty previous or pushed down\n"
		"\tholdbkp (%x %x %x) %i  pushed down if any\n",
		unique_repute->lastkey.hash.h, unique_repute->lastkey.hash.m,
		unique_repute->lastkey.hash.l, unique_repute->lastkey.seq,
		unique_repute->contingentkey.hash.h,
		unique_repute->contingentkey.hash.m,
		unique_repute->contingentkey.hash.l,
		unique_repute->contingentkey.seq,
		unique_repute->holdkey.hash.h, unique_repute->holdkey.hash.m,
		unique_repute->holdkey.hash.l, unique_repute->holdkey.seq,
		repute->holdbkp.hash.h, unique_repute->holdbkp.hash.m,
		unique_repute->holdbkp.hash.l, unique_repute->holdbkp.seq);
	__builtin_printf(" continuing session (%x %x %x) now partial with "
		"\"%s\".\n", desc->session.hash.h, desc->session.hash.m,
		desc->session.hash.l, desc->oneliner);
#endif /* DIAG_CONTINUING_SESSION */

	desc->level &= ((VRT_MASK_SESSION_CALLED ^ 0xffffffff));
	desc->level |= VRT_MASK_SESSION_PARTIAL;

	return 0;
}

/* For phase-a of flexible partial with continuing nodes, having received
   vrtlogin data components url, possibly oneliner, session_peer, and possibly
   session_thru, search all_sessions for a description with url.  This
   indicates that the receiving flexible node exists.  If found test for
   session enabled, login's enabled, and flexible not overloaded.  Passing all
   this, send description details oneliner, url if changed, flexible session
   number, a size total derived from complextimate at the arrival point, and
   the nodemap.  Return 0 on success.  note:  After this function completes,
   tend to session_peer restoring continuation of partial if requested by
   the receiving of phase-b (syncronization) parts of the vrtlogin data then
   calling sync_vrtlogin (see: hacking notes below for more).  Any description
   of a peer in all_sessions only ever applies as a continuing session
   description on a flexible, those having no implied node in node_orgin and
   having a vrtlogin underway or partial with an adjunct described flexible
   node. */
int
answer_vrtlogin(session_t *session_peer, session_t *session_thru, char *url)
{
	session_desc_t *desc_here = NULL;
	session_desc_t *desc_peer = NULL;
	complextimate_t cmplxt_peer = { 0, 0, 0 }; /* unknown until sync */

	/* Simulate receiving vrtlogin phase-a. */
	__builtin_printf("\n ...it seems like nanoseconds ago\nRequest from "
		"\"(%x %x %x) %i continuing\" partial to \"%s\"\n\n",
		session_peer->hash.h, session_peer->hash.m,
		session_peer->hash.l, session_peer->seq, url);

	/* Search all_sessions for url. */
	if ((desc_here = find_url(url)) == NULL) {
		__builtin_printf("Node \"%s\" currently non-existant.  "
			"Disconnect sequence...\n", url);
		/* ... */
		return -1;
	}
	desc_here->level |= VRT_MASK_SESSION_INBOUND | VRT_MASK_SESSION_CALLED;

	__builtin_printf("Node \"%s\" found\n attempting to provide "
		"continuity...\n", desc_here->oneliner);
	diag_ls_all_sessions(1);

	/* note: Disconnect sequence should also tend to desc_here. */
	if (!(desc_here->level & VRT_MASK_SESSION_ENABLE)) {
		__builtin_printf("Node \"%s\" currently unreachable.  "
			"Disconnect sequence...\n", url);
		/* ... */
		return -1;
	} else if (!(session_nodemask & VRT_MASK_ACCEPT_VRTLOGIN)) {
		__builtin_printf("Node(s) currently unreachable.  Disconnect "
			"sequence...\n");
		/* ... */
		return -1;
	} else if (session_nodemask & VRT_MASK_OVERLOADED) {
		__builtin_printf("Node \"%s\" currently at maximum "
			"complextimate state.  Disconnect sequence...\n", url);
		/* ... */
		return -1;
	}

	/* Send latest description including desc_here->oneliner,
	   desc_here->session, and desc_here->cmplxt, followed by
	   desc_here->nodemap. */
	/* ... */
	desc_here->level |= VRT_MASK_SESSION_CONTINUITY;

	__builtin_printf(" sending nodemap to \"%s\"...\n\n oops: vrtater cant "
		"send nodemap.  Most significant components of\n session.c "
		"still missing.  Well, lets say someone had written those\n\n",			url);
	/* ... */

	/* Add phase-a parts for desc_peer. */
	if ((desc_peer = add_session_desc(session_peer, &(desc_here->session), session_thru, VRT_MASK_SESSION_CALLED | VRT_MASK_SESSION_CONTINUITY, NULL, NULL, &cmplxt_peer, NULL, NULL)) == NULL) {
		__builtin_printf("answer_vrtlogin cant find desc_peer\n");
		abort();
	}
	diag_ls_all_sessions(1);
	__builtin_printf("\n nodemap sent.  Checking every so often...\n\n");

	/* hacking notes:  At this point vrtlogin completion is determinate
	   based on a configured lengthly timeout.  Person running vrtater on
	   continuing then has ample time to decide (Some timing configured in
	   ifnode**.c so far).  Further refrence to desc_peer->session are now
	   assumed to be phase-b parts for desc_peer arriving, compounded with
	   loginkey data.  This data always sent immediately before peer
	   request for authentication login.  If the usepasswd level bit is
	   unset in desc_here, automaticaly authenticate if possible.  After
	   vrtlogin data passes preliminarily, call sync_vrtlogin therewith.
	   A preferably GNU Free Software, or GNU freindly login lib mechanism,
	   to be added at some point, would perhaps require uname and password.
	   This would do well as default.  Another way that might be workable
	   would be: retrieve a uname equaling contingentkey with a password.
	   Follow by resetting uname to new contingentkey retaining password.
	   Pondering...  Very non-standard...  Might be way far from secure...
	   Hmm...  Considering eventuality that continuing nodes may share the
	   same url continuously or initially assumes that all nodes behind
	   url should be allowed to be connected simutaniously if a flexible
	   session is accepted by continuing node attempting vrtlogin.  This
	   means that the exchange of a list of n session_desc_t data should be
	   supported so that block of sessions would then first all appear
	   in all_sessions on the continuing, or in the sent nodemap dialog.
	   These then would all be connected when accepting the flexible
	   session.  session_desc_t therefore includes session_t thru, that
	   specifies a node that peer session described eminates out of.
	   For a flexible node, any sessions calling for a vrtlogin out of node
	   at url after a continuing partial session exists there are then
	   automatically thru continuing. */

	return 0;
}

/* For phase-b of flexible partial with continuing nodes, having already
   authenticated with password if added and having received (see: hacking note
   above) phase-b vrtlogin data loginkey, holdkey, lastkey, contingentkey, and
   tl_cmplxt for peer, try to syncronize session_peer with flexible node's
   reputation list.  Allow for successive tries of this phase of vrtlogin at
   the accepting stage with different data, by default renewing any timeout
   then calling this.  If the vrtlogin completes here successfully, the session
   description is already completed and a reputation for passed loginkeys will
   either be added or updated.  Weather successfull or not keyuse_feedback is
   called and when parts for sending data are added therein, a keyuse value
   defined in loginkeys.h is sent to the continuing node reflecting contextual
   loginkey data state on flexible.  Return keyuse value sent to continuing or
   < 0 apon any error, meanwhile trying to maintain continuity where there is
   no partial yet.  note: When removing flexible nodes it is assumed associated
   session descriptions will be removed first through close_session. */
int
sync_vrtlogin(session_t *session_peer, session_t *session_thru, session_t *loginkey, session_t *lastkey, session_t *contingentkey, session_t *holdkey, unsigned int tl_cmplxt, char *url)
{
	session_desc_t *desc_here, *desc_peer;
	struct ptlrepute *repute_here, *unique_repute_here;
	int keyuse, rval = -1;

	/* Likely passed in as args. */
	if ((desc_here = find_url(url)) == NULL) {
		__builtin_printf("sync_vrtlogin cant find_url\n");
		abort();
	}
	if ((desc_peer = find_session(session_peer)) == NULL) {
		__builtin_printf("sync_vrtlogin cant find_session\n");
		abort();
	}

	/* For simulation, create an already active partial reputation in
	   desc_here->ptlrepute for vrtlogin of avatar555, enabling a
	   continuing session.  note: url is always NULL for repute in a
	   flexible partial. */
	session_t continuelastkey = { { 0, 0, 0xb0de }, 8 };
	session_t continuecontingentkey = { { 0, 0, 0xface }, 8 };
	session_t continueholdkey = { { 0, 0, 0xface }, 555 };
	repute_here = add_ptlrepute(desc_here->reputed, &continuelastkey, &continueholdkey, NULL);
	cp_mapname(&continuecontingentkey, &(repute_here->contingentkey));

	__builtin_printf(" authenticated vrtlogin.\n partial to \"%s\" "
		"requested\n continuing (%x %x %x) nport to \"%s\" arrival\n",
		url, session_peer->hash.h, session_peer->hash.m,
		session_peer->hash.l, desc_here->oneliner);

	/* For received cmplxt that is assumed to exclude the nodemap sent,
	   test if login_cmplxt_max is exceeded here. */
	if (login_cmplxt_max < tl_cmplxt) {
		__builtin_printf("Sending message...\n Continuing session "
			"maintained on \"%s\"\n requires lower complexitmate "
			"to log in.  session then retains continuity...\n",
			url);
		return -1;
	}
#ifdef DIAG_FLEXIBLE_SESSION
	if ((unique_repute_here = find_lastkey(desc_here->reputed, &continuelastkey)) == NULL) {
		__builtin_printf("sync_vrtlogin cant find_lastkey\n");
		abort();
	}

	__builtin_printf(" continuing loginkey data transfered\n\tloginkey "
		"(%x %x %x) %i\n\tlastkey (%x %x %x) %i\n\tcontingentkey "
		"(%x %x %x) %i\n\tholdkey (%x %x %x) %i\n\tholdbkp (%x %x %x) "
		"%i\n", loginkey->hash.h, loginkey->hash.m, loginkey->hash.l,
		loginkey->seq, lastkey->hash.h, lastkey->hash.m,
		lastkey->hash.l, lastkey->seq, contingentkey->hash.h,
		contingentkey->hash.m, contingentkey->hash.l,
		contingentkey->seq, holdkey->hash.h, holdkey->hash.m,
		holdkey->hash.l, holdkey->seq,
		unique_repute_here->holdbkp.hash.h,
		unique_repute_here->holdbkp.hash.m,
		unique_repute_here->holdbkp.hash.l,
		unique_repute_here->holdbkp.seq);
#endif /* DIAG_FLEXIBLE_SESSION */

	/* Syncronize loginkey reputation data using repute_peer vs. ptlrepute
	   list for flexible.  There will either be a reputation already,
	   or a new one will be added.  holdmap reference stack will be pushed
	   if holdkey is not of zero_mapname and found to be unique in
	   ptlrepute.  */
	keyuse = sync_loginkeys(NULL, desc_here->reputed, NULL, loginkey, holdkey, lastkey, contingentkey, VRT_LOGIN_PRECONTEXT);

	/* Send a keyuse value to continuing node so that loginkey data can be
	   syncronized there. */
	if ((rval = keyuse_feedback(desc_peer, loginkey, keyuse)) != 0) {
		__builtin_printf(" sync_loginkeys returns %i  keyuse_feedback "
			"returns %i\n",
			keyuse, rval); /* diag */
		subtract_session_desc(desc_peer);
		return rval;
	} else {

#ifdef DIAG_FLEXIBLE_SESSION
		__builtin_printf(" sync_loginkeys returns %i  keyuse_feedback "
			"returns %i\n", keyuse, rval);
		if ((unique_repute_here = find_lastkey(desc_here->reputed, loginkey)) == NULL) {
			__builtin_printf("sync_vrtlogin cant find_lastkey\n");
			abort();
		}

		__builtin_printf(" flexible loginkey data after sync\n"
			"\tlastkey (%x %x %x) %i\n"
			"\tcontingentkey (%x %x %x) %i\n"
			"\tholdkey (%x %x %x) %i  active on flexible\n"
			"\tholdbkp (%x %x %x) %i  active on flexible\n",
			unique_repute_here->lastkey.hash.h,
			unique_repute_here->lastkey.hash.m,
			unique_repute_here->lastkey.hash.l,
			unique_repute_here->lastkey.seq,
			unique_repute_here->contingentkey.hash.h,
			unique_repute_here->contingentkey.hash.m,
			unique_repute_here->contingentkey.hash.l,
			unique_repute_here->contingentkey.seq,
			unique_repute_here->holdkey.hash.h,
			unique_repute_here->holdkey.hash.m,
			unique_repute_here->holdkey.hash.l,
			unique_repute_here->holdkey.seq,
			unique_repute_here->holdbkp.hash.h,
			unique_repute_here->holdbkp.hash.m,
			unique_repute_here->holdbkp.hash.l,
			unique_repute_here->holdbkp.seq);
		__builtin_printf(" flexible session (%x %x %x) now partial "
			"with \"%s\".\n", desc_here->session.hash.h,
			desc_here->session.hash.m, desc_here->session.hash.l,
			desc_here->oneliner);
#endif /* DIAG_FLEXIBLE_SESSION */

		/* Add partial session.  Connect session sequence if any. */
		/* ... */
		desc_here->level &= ((VRT_MASK_SESSION_DETACHED ^ 0xffffffff));
		desc_here->level &= ((VRT_MASK_SESSION_ENABLE ^ 0xffffffff));
		desc_here->level &= ((VRT_MASK_SESSION_CALLED ^ 0xffffffff));
		desc_here->level &= ((VRT_MASK_SESSION_CONTINUITY ^ 0xffffffff));
		desc_here->level |= VRT_MASK_SESSION_PARTIAL;
		desc_peer->level ^= (VRT_MASK_SESSION_CALLED | VRT_MASK_SESSION_CONTINUITY | VRT_MASK_SESSION_PARTIAL);
		strcpy(desc_peer->oneliner, desc_here->oneliner);
		desc_peer->cmplxt.tl_dialog = tl_cmplxt; /* cheating for now */
		diag_ls_all_sessions(1);
	}

	return rval;
}

/* Send syncronization values described by keyuse to continuing node indicating
   success or failure of sync.  Return 0 on success else keyuse, or < 0 on any
   error. */
int
keyuse_feedback(session_desc_t *desc, session_t *loginkey, int keyuse)
{
	/* Messages to remote.  Forward keyuse value to peer and ... */
	if (keyuse == VRT_LOGIN_RETRY) {
		__builtin_printf("Sending keyuse message...\n Redundant "
			"loginkey or holdkey.  Try again with something any "
			"bit different.\n");
		/* Send VRT_LOGIN_RETRY. ... */
		return keyuse;
	} else if (keyuse == VRT_LOGIN_NEWREPUTED) {
		__builtin_printf("Sending keyuse message...\n New reputation "
			"on (%x %x %x).\n", desc->peer.hash.h,
			desc->peer.hash.m, desc->peer.hash.l);
		/* Send VRT_LOGIN_NEWREPUTED. ... */
	} else if (keyuse == VRT_LOGIN_LAST) {
		__builtin_printf("Sending keyuse message...\n Continuing "
			"session.\n");
		/* Send VRT_LOGIN_LAST. ... */
	} else if (keyuse == VRT_LOGIN_CONTINGENT) {
		__builtin_printf("Sending keyuse message...\n Continuing "
			"session.  note: !!contingentkey usage.\n");
		/* Send VRT_LOGIN_CONTINGENT. ... */
	} else if (keyuse == VRT_LOGIN_SYNCERR) {
		__builtin_printf("Sending keyuse message...\n Oops! A highly "
			"unlikely pair of exclusive errors has occured on one "
			"or both\n node systems.  Recovery option (if added) "
			"is required to continue session.\n See ifnode "
			"interface for more.\n");
		/* Send VRT_LOGIN_SYNCERR. ... */
		return keyuse;
	}

#ifdef DIAG_FLEXIBLE_SESSION
	/* Login succeeded. */
	__builtin_printf(" successfull vrtlogin by (%x %x %x) %i on "
		"(%x %x %x)\n", loginkey->hash.h, loginkey->hash.m,
		loginkey->hash.l, loginkey->seq, desc->peer.hash.h,
		desc->peer.hash.m, desc->peer.hash.l);
#endif

	return 0;
}

/* Tend to remote node sessions.  Called once per state increment.  This
   function will likely call read_from_network or something.  A send_maps call
   or freely chosen callname name might then follow. */
void
sync_sessions(void)
{
	;
}

/* Read .vrtater, other data, associated with remote node sessions running on
   puters on the internet, organizing int type .vrtater data implied to be
   unwrapped for each seperate session in successive calls to receive_map. */
void
read_from_network(void)
{
	;
}

/* Send to each node connected to session, counta hmaps in seta both referenced
   thru sel.  note: A set of outbound hmaps within session are referenced per
   call, partial session description having it's level bit true all_sessions.
   These happen to include only non node-orgin hmaps with relevant changes.
   These then become delivered for each url implied.  Return values perhaps
   determined in sync_sessions describing latency or disconnect for given
   session allowing caller to conditionally call close_session.  A last call
   with NULL reference will be added. */
int
send_maps(session_t *session, select_t *sel)
{
	hmapf_t **map;
	int i;

	map = sel->seta;
	__builtin_printf(" session.c: pretending to syncronize hmaps within "
		"partial (%x %x %x),\n  sending...\n",
		session->hash.h, session->hash.m, session->hash.l);
	for (i = 0; i < sel->counta; i++, map++)
		__builtin_printf("  map (%x %x %x) %i\n", (*map)->name.hash.h,
		(*map)->name.hash.m, (*map)->name.hash.l, (*map)->name.seq);

	return 0;
}

/* By this or another name chosen, and per each singular .vrtater data parcel
   received and connected to session then returned, hmapunwrapf parcel,
   returning NULL when there are zero sessions with .vrtater data available,
   otherwise returning counta hmap references in seta both referenced thru sel.
   note: hmapwrapf wraps from selectf_a, while hmapunwrapf unwraps to b. */
session_t *
receive_map(select_t *sel)
{
	session_t *session;

	return session;
}

/* For now here for testing in progress */
hmapf_t *
diag_receive_nodemap(session_t *session, select_t *sel)
{
	hmapf_t *map = NULL;
	int buf_len, *int_buf, **buf = &int_buf;

	/* Normally read .vrtater data then refrenced by int_buf from ... */
	/* ... */

	/* For diagnostic, call hmapwrap to create some int data.  int_buf
	   then becomes allocated and contains the data.  First the hmap to be
	   wrapped is referenced in selectf_a.  note: 2nd arg is opt's mask. */
	map = p_hmapf(0);
	*(sel->seta) = map;
	buf_len = hmapwrapf(sel, 0, NULL, buf);

	/* Now that int data is available call hmapunwrap to place it in
	   vohspace.  Afterwards sel->countb holds a map count of 1 for now.
	   The mapname of hmap is changed for now to avoid a conflict, however
	   soon hmapunwrap will update any map of same mapname. */
	if (hmapunwrapf(sel, session, NULL, (int *) int_buf) < 0)
		__builtin_fprintf(stderr, "Error: .vrtater data received did "
			"not hmapunwrapf as expected.");
	else
		__builtin_printf("%i hmaps unwrapped for node (%x %x %x)\n",
			sel->countb, session->hash.h, session->hash.m,
			session->hash.l);

	/* For diagnostic, hmapwrap allocated int_buf.  This way of allocating
	   is still under review, as malloc really should  be called where data
	   is introduced rethinking it... yet caller would then need to
	   calculate the correct filesize.   Perhaps calling first for that.
	   Hmm...  That should work... See next commit for more... */
	free(int_buf); /* for now free after hmapwrap allocates for caller. */

	/* Adjust map for diagnostic, !!not working for some reason but... */
	map = *(sel->setb);
	map->vpos.z -= 5.; /* adjust map for visibility (very tiny map) */

	return *(sel->setb);
}

/* Return reference to session description matching session or NULL if none. */
session_desc_t *
find_session(session_t *session)
{
	session_desc_t *current, *passed;

	current = all_sessions->last;
	passed = all_sessions->last;
	while (1) {
		if (current != NULL) {
			if (match_session(&(current->session), session))
				return current;
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Return reference to session description matching url or NULL if none. */
session_desc_t *
find_url(char *url)
{
	session_desc_t *current, *passed;

	current = all_sessions->last;
	passed = all_sessions->last;
	while (1) {
		if (current != NULL) {
			if (current->url) {
				if (!(strcmp(current->url, url))) {
					if (!(current->level & VRT_MASK_SESSION_UNLINKED))
					return current;
				}
			}
			passed = current;
			current = current->precursor;
		} else
			return NULL;
	}
}

/* Create linked list construct for all_sessions. */
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

/* Add a session description reference to all_sessions providing session
   description details peer, thru (if forwarding present), level describing
   connection state, oneliner describing session or session's peer, cmplxt
   describing size of data transfered during vrtlogin (might be used thereafter
   to maintain tl_cmplxt for session), and nodemap describing node
   volumetrically.  ptlrepute reference supplied will be non NULL if
   description is of a flexible node, allowing for calls to sync_loginkeys in
   filescope.  Reference to url is added only where description is of a
   continuing session eminating, not for a continuing session description in
   all_sessions on a flexible, those having no implied node in node_orgin.
   Return reference. */
session_desc_t *
add_session_desc(session_t *session, session_t *peer, session_t *thru, int level, char *url, char *oneliner, complextimate_t *cmplxt, hmapf_t *nodemap, struct ptlrepute_list *reputed)
{
	session_desc_t *listed = NULL;

	if (find_session(session)) {
		__builtin_printf("Error: Can not create session description "
			"as session (%x %x %x) already exists\n",
			session->hash.h, session->hash.m, session->hash.l);
		return NULL;
	}
	if (url) {
		if (find_url(url)) {
			__builtin_printf("Error: Can not create session "
			"description as url %s is already taken in "
			"all_sessions\n", url);
			return NULL;
		}
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
	cp_session(session, &(listed->session));
	cp_session(peer, &(listed->peer));
	cp_session(thru, &(listed->thru));
	listed->level = level;
	if (url) {
		if ((listed->url = (char *) malloc(strlen(url))) == NULL) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: Could not malloc for all_sessions "
				"list member url\n",
				__FILE__, __LINE__);
			abort();
		}
		strcpy(listed->url, url);
	} else
		listed->url = NULL;
	if (oneliner)
		strcpy(listed->oneliner, oneliner);
	listed->nodemap = nodemap;
	listed->reputed = reputed;
	listed->cmplxt.hmap_count = cmplxt->hmap_count;
	listed->cmplxt.tl_vdata = cmplxt->tl_vdata;
	listed->cmplxt.tl_dialog = cmplxt->tl_dialog;

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
			if (match_session(&(current->session), &(desc->session)))
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	__builtin_printf(" removing session description (%x %x %x)\n",
		current->session.hash.h, current->session.hash.m,
		current->session.hash.l);
	if (current == passed) {
		if (!current->precursor)
			all_sessions->last = NULL;
		else
			all_sessions->last = current->precursor;
	} else
		passed->precursor = current->precursor;
	if (current->url)
		free(current->url);
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

/* Return 0 after closing all connected sessions. */
int
close_all_sessions(void)
{
	while (all_sessions->last != NULL)
		close_session(all_sessions->last);

	return 0;
}

/* Called when program exits or otherwise, this should leave the program code
   herein in an initial state with all connections dropped.  When this function
   is called, caller will then always create a new session description list.
   init_sessions may follow therafter.  When call_session is called, or when
   VRT_MASK_ACCEPT_VRTLOGIN bit becomes true, calls to sync_session will
   always become periodic per state increment.  This is the same sequence used
   when the program is first run.  node-orgin and possibly some defined
   flexible nodes that are not yet enabled will be in the session_desc_list.
   Non-zero could be returned for now if a peer node seems broken, otherwise
   returning 0 indicates an initial state. */
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
diag_ls_all_sessions(int full)
{
	session_desc_t *current, *passed;
	char *tmp, other[] = "";
	int i;

	if (all_sessions->count == 0) {
		__builtin_printf("--no partial sessions listed--\n");
		return;
	}

	current = all_sessions->last;
	passed = all_sessions->last;
	for (i = 0 ; ;i++) {
		if (current != NULL) {
			__builtin_printf("(%x %x %x) ", current->session.hash.h,
				current->session.hash.m,
				current->session.hash.l);
			__builtin_printf("(%x %x %x) ", current->peer.hash.h,
				current->peer.hash.m, current->peer.hash.l);
			__builtin_printf("(%x %x %x) ", current->thru.hash.h,
				current->thru.hash.m, current->thru.hash.l);
			if (full) {
				if (current->level & VRT_MASK_SESSION_DETACHED)
					__builtin_printf("DETACHED ");
				if (current->level & VRT_MASK_SESSION_ENABLE)
					__builtin_printf("VOBSPACE ");
				if (current->level & VRT_MASK_SESSION_CALLED)
					__builtin_printf("CALLED ");
				if (current->level & VRT_MASK_SESSION_CONTINUITY)
					__builtin_printf("NODEMAP ");
				if (current->level & VRT_MASK_SESSION_USEPASSWD)
					__builtin_printf("AUTH ");
				if (current->level & VRT_MASK_SESSION_PARTIAL)
					__builtin_printf("PARTIAL ");
				if (current->level & VRT_MASK_SESSION_INBOUND)
					__builtin_printf("FLEXIBLE ");
				else if ((current->level & VRT_MASK_SESSION_PARTIAL))
					__builtin_printf("CONTINUING ");
				__builtin_printf("\"%s\" ", current->oneliner);
				__builtin_printf("%i ", (sizeof(hmapf_t) * current->cmplxt.hmap_count) + current->cmplxt.tl_vdata + current->cmplxt.tl_dialog);

				__builtin_printf("%s", tmp = current->url ? current->url : other);
			}
			__builtin_printf("\n");
			passed = current;
			current = current->precursor;
		} else {
			__builtin_printf("--%i descriptions listed--\n", i);
			return;
		}
	}
}
