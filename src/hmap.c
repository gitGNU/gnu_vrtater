/* hmap.c: calls to retreive and help build hmaps(vob attribute data struct).
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdio.h>
#include "hmap.h"
#include "attribs.h"

/* External. */
unsigned int vrt_hmaps_max;

/* Attach an empty hmap to given session. */
hmapf_t *
hmapf(session_t *session)
{
	hmapf_t *p;

	if ((p = attach_hmapf()) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: All hmaps are currently attached\n",
			__FILE__, __LINE__);
		/* dlg(VRT_VOBSPACE,
			"Error: Attempted to exceed hmap limit\n"); */
		return NULL;
	}
	p->name = *session | (session_t) p->index;
	__builtin_printf("generated hmap %x (index %i, free maps %u/%u)\n",
		(int) p->name, p->index, vrt_hmaps_max - attached_hmaps,
		vrt_hmaps_max);
	return p;
}

/* Return reference to hmap vs. it's index i.  note: This function, although
   quick, is unworkable with any partial connection.  It will be removed when
   alpha version is ready.  Function to replace it will match on session name
   instead. */
hmapf_t *
p_hmapf(int i)
{
	return &vohspace[i];
}

/* Return current number of hmaps in use out of vrt_hmaps_max available. */
unsigned int
hmap_count(void)
{
	return attached_hmaps;
}

/* Search vohspace for hmaps holding any of signs or modes given, and write
   references for those in listout.  Return count of matches referenced.  Until
   vohspace memory can be resized while running all node-partial's and
   node-orgin, this search may be rather suboptimal, as vrt_hmaps_max hmaps are
   always searched regardless of whether they are attached or not. */
unsigned int
search_vohspace(hmapf_t **listout, btoggles_t signs, btoggles_t modes)
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
		__builtin_printf("  %x ", (int)(*map)->name);
		if ((*map)->attribs.sign & VRT_MASK_HOLD)
			__builtin_printf("HOLD ");
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
		if ((*map)->attribs.mode & VRT_MASK_ATTACHED)
			__builtin_printf("attachd ");
		if ((*map)->attribs.mode & VRT_MASK_BALANCE_FILTER)
			__builtin_printf("balancef ");
		if ((*map)->attribs.mode & VRT_MASK_SESSION_FILTER)
			__builtin_printf("sessionf ");
		if ((*map)->attribs.mode & VRT_MASK_PUBLISHED)
			__builtin_printf("pub ");
		if ((*map)->attribs.mode & VRT_MASK_RENDER_FOLLOWS)
			__builtin_printf("rndf ");
		if ((*map)->attribs.mode & VRT_MASK_VOB_SELECTED)
			__builtin_printf("selectd ");
		if ((*map)->attribs.mode & VRT_MASK_LEFT_IN_TUNNEL)
			__builtin_printf("lbehind ");
		if ((*map)->attribs.mode & VRT_MASK_FLOW_OVER)
			__builtin_printf("flowovr ");
		if ((*map)->attribs.mode & VRT_MASK_FIXED_FORM)
			__builtin_printf("fixd ");
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
