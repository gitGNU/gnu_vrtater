/* hmap.c: calls to retreive and help build hmaps(vob attribute data struct).
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdio.h>
#include "hmap.h"
#include "attribs.h"

unsigned int vrt_hmaps_max; /* external */

/* attach an empty hmap to given session */
hmapf_t *
hmapf(session_t *session)
{
	hmapf_t *p;

	/* attach, set hmap session name */
	if((p = attach_hmapf()) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: All hmaps are currently attached\n",
			__FILE__, __LINE__);
		/* dlg(VRT_VOBSPACE,
			"Error: Attempted to exceed hmap limit\n"); */
		return NULL;
	}
	/* append hmap index to session name */
	p->name = *session | (session_t)p->index;
	__builtin_printf("generated hmap %x (index %i, free maps %u/%u)\n",
		(int)p->name, p->index, vrt_hmaps_max - attached_hmaps, vrt_hmaps_max);
	return p;
}

/* note: this function will depreciate(be removed) at alpha version release */
hmapf_t *
p_hmapf(int i)
{
	return &a_hmaps[i];
}

unsigned int
hmap_count(void)
{
	return attached_hmaps;
}

/* diagnostic: printout diagnostic info for any hmaps referenced in selectf_a or
   alternately selectf_b if VRT_MASK_HAS_SETB is in specbits */
void
diag_selection(select_t *sel)
{
	int i;
	hmapf_t *map;

	map = (sel->specbits & VRT_MASK_HAS_SETB) ? *(sel->setb) : *(sel->seta);
	if(map == *(sel->setb))
		__builtin_printf(" diag info for setb:\n");
	else
		__builtin_printf(" diag info for seta:\n");

	for(i = 0; i < sel->counta; i++, map++) {
		__builtin_printf("%x ", (int)map->name);
		if(map->attribs.sign & VRT_MASK_HOLD)
			__builtin_printf("HOLD ");
		if(map->attribs.sign & VRT_MASK_RECYCLE)
			__builtin_printf("RECYC ");
		if(map->attribs.sign & VRT_MASK_PARTIAL)
			__builtin_printf("PARTIAL ");
		if(map->attribs.sign & VRT_MASK_DIALOG)
			__builtin_printf("DIALOG ");
		if(map->attribs.sign & VRT_MASK_BUFFER)
			__builtin_printf("BUFR ");
		if(map->attribs.sign & VRT_MASK_VERTICE_MODS)
			__builtin_printf("VMODS ");
		if(map->attribs.sign & VRT_MASK_DIALOG_MODS)
			__builtin_printf("DMODS ");
		if(map->attribs.sign & VRT_MASK_DETACH)
			__builtin_printf("DETACH ");
		if(map->attribs.mode & VRT_MASK_ATTACHED)
			__builtin_printf("attachd ");
		if(map->attribs.mode & VRT_MASK_BALANCE_FILTER)
			__builtin_printf("balancef ");
		if(map->attribs.mode & VRT_MASK_SESSION_FILTER)
			__builtin_printf("sessionf ");
		if(map->attribs.mode & VRT_MASK_PUBLISHED)
			__builtin_printf("pub ");
		if(map->attribs.mode & VRT_MASK_RENDER_FOLLOWS)
			__builtin_printf("rndf ");
		if(map->attribs.mode & VRT_MASK_VOB_SELECTED)
			__builtin_printf("selectd ");
		if(map->attribs.mode & VRT_MASK_LEFT_IN_TUNNEL)
			__builtin_printf("lbehind ");
		if(map->attribs.mode & VRT_MASK_FLOW_OVER)
			__builtin_printf("flowovr ");
		if(map->attribs.mode & VRT_MASK_FIXED_FORM)
			__builtin_printf("fixd ");
		if(map->attribs.mode & VRT_MASK_EXTEND_BY_FILTER)
			__builtin_printf("xbfilt ");
		if(map->attribs.mode & VRT_MASK_EXTEND_ANY)
			__builtin_printf("xany ");
		if(map->attribs.mode & VRT_MASK_EXTEND_SEAMLESS)
			__builtin_printf("xseam ");
		if(map->attribs.mode & VRT_MASK_WALL_TYPEA)
			__builtin_printf("wtypea ");
		__builtin_printf("\n");
	}
}

/* set lod envelopes */
void
set_lod_envelopef(float near, float perif, unsigned int sp_ratio, unsigned int sf_ratio)
{
	near_threshf = near;
	perif_threshf = perif;
	sort_perif_ratio = sp_ratio;
	sort_far_ratio = sf_ratio;
}
