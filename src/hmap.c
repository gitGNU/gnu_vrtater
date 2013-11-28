/* hmap.c: Calls associated with retreival of hmaps and their data in vohspace.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdio.h>
#include "hmap.h"
#include "global.h"
#include "attribs.h"

/* External. */
unsigned int vrt_hmaps_max;

/* Set session hash referenced to all zero's.  (0 0 0)' ?. */
void
zero_session(session_t *session)
{
	int i, *p = &(session->hash.h);

	for (i = 0; i < 3; i++)
		*p++ = 0;
}

/* Set session name referenced to all zero's.  (0 0 0)' 0'. */
void
zero_fullname(session_t *session)
{
	int i, *p = &(session->hash.h);

	for (i = 0; i < 4; i++)
		*p++ = 0;
}

/* Set nodename to session.  (hash)' 0' <-- (hash) ?. */
void
set_nodename(session_t *nodename, session_t *session)
{
	int i, *c = &(nodename->hash.h);
	int *d = &(session->hash.h);

	for (i = 0; i < 3; i++)
		*c++ = *d++;
	*c = 0;
}

/* Copy session hash a to session hash b.  (hash) ? --> (hash)' ?. */
void
cp_session(session_t *a, session_t *b)
{
	int i, *c = &(a->hash.h);
	int *d = &(b->hash.h);

	for (i = 0; i < 3; i++)
		*d++ = *c++;
}

/* Copy map name a to map name b.  (hash) seq --> (hash)' seq' */
void
cp_mapname(session_t *a, session_t *b)
{
	int i, *c = &(a->hash.h);
	int *d = &(b->hash.h);

	for (i = 0; i < 4; i++)
		*d++ = *c++;
}

/* Return true if mapnames a and b match else 0.  (hash) seq == (hash) seq */
int
match_mapname(session_t *a, session_t *b)
{
	int i, *c = &(a->hash.h);
	int *d = &(b->hash.h);

	for (i = 0; i < 4; i++)
		if (*c++ != *d++)
			return 0;
	return 1;
}

/* Return true if session hashes a and b match else 0.  (hash) ? == (hash) ? */
int
match_session(session_t *a, session_t *b)
{
	int i, *c = &(a->hash.h);
	int *d = &(b->hash.h);

	for (i = 0; i < 3; i++)
		if (*c++ != *d++)
			return 0;
	return 1;
}

/* True if mapname a and b hash.l and seq match.  (? ? l) seq == (? ? l) seq */
int
match_mapped(session_t *a, session_t *b)
{
	int i, *c = &(a->hash.l);
	int *d = &(b->hash.l);

	for (i = 0; i < 2; i++)
		if (*c++ != *d++)
			return 0;
	return 1;
}

/* Return true if name member hash.l matches p, else 0.  (? ? l) ? == *p
   note: If a hash.l is non unique arriving in vohspace over ip, the map
   already in vohspace will simply take a new hash yet retain it's hash.l part. 
   This will only happen once every 2^32 times for any new map,,, well
   hopefully. */
int
match_little(session_t *name, int *p)
{
	if (name->hash.l != *p)
		return 0;
	return 1;
}

/* Attach empty hmap to given session.  Applied index reflects hmaps ordinal
   position in vohspace for node-orgin maps.  note: When resizing while
   connected is added, call for more vohspace when it becomes full. */
hmapf_t *
hmapf(session_t *session)
{
	hmapf_t *map;

	if ((map = attach_hmapf()) == NULL) {
		__builtin_fprintf(stderr, "Error: All hmaps are currently "
			"attached\n");
		return NULL;
	}
	cp_session(session, &(map->name));

#ifdef DIAG_HMAP_MESSAGES
	__builtin_printf(" generated hmap (%x %x %x) %i (free maps %u/%u)\n",
		map->name.hash.h, map->name.hash.m, map->name.hash.l,
		 map->name.seq, vrt_hmaps_max - attached_hmaps, vrt_hmaps_max);
#endif

	return map;
}

/* Return reference to hmap vs. it's index i.  note: This function, although
   quick, is unworkable with any partial connection.  It will be removed when
   alpha version is ready.  Function mapref will replace it, matching on
   session name instead. */
hmapf_t *
p_hmapf(int i)
{
	return &vohspace[i];
}

/* Return reference to hmap vs. session, and NULL if no match.  note: This
   function, is undesireably slow, yet will be usefull for now.  It will be
   replaced as soon as issue's of storing vohspace are resolved. */
hmapf_t *
mapref(session_t *session)
{
	int i;
	hmapf_t *map;

	map = vohspace;
	for (i = 0; i < vrt_hmaps_max; i++, map++)
		if (match_mapname(&(map->name), session))
			return map;
	return NULL;
}

/* Return current number of hmaps in use out of vrt_hmaps_max available. */
unsigned int
hmap_count(void)
{
	return attached_hmaps;
}

/* Search vohspace for hmaps holding any of signs or modes given, and write
   references for those in listout.  Return count of matches referenced.
   note: Until vohspace memory can be resized while running all node-partial's
   and node-orgin, this search may be rather suboptimal, as vrt_hmaps_max hmaps
   are always searched regardless of whether they are attached or not. */
unsigned int
search_vohspace(hmapf_t **listout, int signs, int modes)
{
	int i, count = 0;
	hmapf_t *p = &vohspace[0];
	hmapf_t **map = listout;
	for (i = 0; i < vrt_hmaps_max; i++, p++) {
		if ((p->attribs.sign & signs) || (p->attribs.mode & modes)) {
			*(map++) = p;
			count++;
		}
	}
	return count;
}

/* Temporary diagnostic to printout info for any hmaps referenced in selectf_a
   or alternately selectf_b if VRT_MASK_HAS_SETB is in specbits.  select_t
   member counta or countb must be set respectively. */
void
diag_selection(select_t *sel)
{
	int i, count;
	hmapf_t **map;

	if (sel->specbits & VRT_MASK_HAS_SETB) {
		map = sel->setb;
		count = sel->countb;
		__builtin_printf(" diag_selection: setb (%i hmaps)\n",
			sel->countb);
	} else {
		map = sel->seta;
		count = sel->counta;
		__builtin_printf(" diag_selection: seta (%i hmaps)\n",
			sel->counta);
	}

	for (i = 0; i < count; i++, map++) {
		__builtin_printf("  (%x %x %x) %i", (*map)->name.hash.h,
			(*map)->name.hash.m, (*map)->name.hash.l,
			(*map)->name.seq);
		if ((*map)->attribs.sign & VRT_MASK_RECYCLE)
			__builtin_printf("RECYC ");
		if ((*map)->attribs.sign & VRT_MASK_BUFFER)
			__builtin_printf("BUFR ");
		if ((*map)->attribs.sign & VRT_MASK_DETACH)
			__builtin_printf("DETACH ");
		if ((*map)->attribs.sign & VRT_MASK_DIALOG)
			__builtin_printf("DIALOG ");
		if ((*map)->attribs.sign & VRT_MASK_PARTIAL)
			__builtin_printf("PARTIAL ");
		if ((*map)->attribs.sign & VRT_MASK_VERTICE_MODS)
			__builtin_printf("VTCMODS ");
		if ((*map)->attribs.sign & VRT_MASK_DIALOG_MODS)
			__builtin_printf("DLGMODS ");
		if ((*map)->attribs.sign & VRT_MASK_PARTIAL_MODS)
			__builtin_printf("PTLMODS ");
		if ((*map)->attribs.sign & VRT_MASK_RENDER_DIALOG)
			__builtin_printf("RNDRDLG ");
		if ((*map)->attribs.sign & VRT_MASK_RESEND)
			__builtin_printf("RESEND ");
		if ((*map)->attribs.sign & VRT_MASK_TRUNKMAP)
			__builtin_printf("TRUNKMAP ");
		if ((*map)->attribs.sign & VRT_MASK_TREEMAP)
			__builtin_printf("TREEMAP ");
		if ((*map)->attribs.sign & VRT_MASK_KEYMAP)
			__builtin_printf("KEYMAP ");
		if ((*map)->attribs.mode & VRT_MASK_ATTACHED)
			__builtin_printf("attachd ");
		if ((*map)->attribs.mode & VRT_MASK_BALANCE_FILTER)
			__builtin_printf("balancef ");
		if ((*map)->attribs.mode & VRT_MASK_SESSION_FILTER)
			__builtin_printf("sessionf ");
		if ((*map)->attribs.mode & VRT_MASK_IP)
			__builtin_printf("ip ");
		if ((*map)->attribs.mode & VRT_MASK_FLOW)
			__builtin_printf("flow ");
		if ((*map)->attribs.mode & VRT_MASK_RENDER_FOLLOWS)
			__builtin_printf("rndf ");
		if ((*map)->attribs.mode & VRT_MASK_VOB_SELECTED)
			__builtin_printf("selectd ");
		if ((*map)->attribs.mode & VRT_MASK_LEFT_IN_TUNNEL)
			__builtin_printf("lbehind ");
		if ((*map)->attribs.mode & VRT_MASK_FLOW_OVER)
			__builtin_printf("flowovr ");
		if ((*map)->attribs.mode & VRT_MASK_FIXED_FORM)
			__builtin_printf("fxdform ");
		if ((*map)->attribs.mode & VRT_MASK_EXTEND_BY_FILTER)
			__builtin_printf("xbfilt ");
		if ((*map)->attribs.mode & VRT_MASK_EXTEND_ANY)
			__builtin_printf("xany ");
		if ((*map)->attribs.mode & VRT_MASK_EXTEND_SEAMLESS)
			__builtin_printf("xseam ");
		if ((*map)->attribs.mode & VRT_MASK_WALL_TYPEA)
			__builtin_printf("wtypea ");
		if ((*map)->attribs.mode & VRT_MASK_NODEMAP)
			__builtin_printf("nodemap ");
		if ((*map)->attribs.mode & VRT_MASK_GROUPMAP)
			__builtin_printf("groupmap ");
		if ((*map)->attribs.mode & VRT_MASK_SYNC_VERTICES)
			__builtin_printf("vxsync ");
		if ((*map)->attribs.mode & VRT_MASK_HOLD)
			__builtin_printf("HOLD ");
		__builtin_printf("\n");
	}
}

/* Set lod envelopes. */
void
set_lod_envelopef(float near, float perif, unsigned int sp_ratio, unsigned int sf_ratio)
{
	near_threshf = near;
	perif_threshf = perif;
	sort_perif_ratio = sp_ratio;
	sort_far_ratio = sf_ratio;
}
