/* hmap.c: calls to retreive and help build hmaps(vob attribute data struct).
   Copyright (C) 2012 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <unistd.h>
#include "progscope.h"
#include "hmap.h" /* !! */
#include "vectors.h"
#include "attribs.h"

/* hmapf selection buffers: definition for extern */
hmapf_t *selectf_a[VRT_HMAPS_MAX];
hmapf_t *selectf_b[VRT_HMAPS_MAX];

/* attach an empty hmap to given session */
hmapf_t *
hmapf(session_t *psession)
{
	hmapf_t *p;

	/* attach, set hmap session name */
	if((p = attach_hmapf()) == NULL) {
		__builtin_printf("hmap(): already have HMAPS_MAX attached\n");
		return NULL;
	}
	/* append hmap index to session name */
	p->name = *psession | (session_t)p->index;
	__builtin_printf("hmap(%llu): generated vob %i\n", \
		p->name, p->index);
	return p;
}

/* add a triangle to hmap data */
void
add_tri_to_hmapf(vf_t *av, vf_t **ppd)
{
	int i;
	vf_t *p;

	p = av;
	for(i=0; i<3; i++, p++, (*ppd)++) {
		(*ppd)->x = p->x;
		(*ppd)->y = p->y;
		(*ppd)->z = p->z;
		(*ppd)->m = p->m;
	}
}

void
add_vf_to_hmap(vf_t *p, vf_t **ppd)
{
	(*ppd)->x = p->x;
	(*ppd)->y = p->y;
	(*ppd)->z = p->z;
	(*ppd)->m = p->m;
	(*ppd)++;
}

hmapf_t *
p_hmapf(int i)
{
	return &a_hmaps[i];
}

/*  */
void
adjust_positionf(hmapf_t *p, float x, float y, float z, float m)
{
	;
}

/*  */
void
adjust_dir_momentf(hmapf_t *p, float x, float y, float z, float m)
{
	;
}

/*  */
void
adjust_pos_momentf(hmapf_t *p, float x, float y, float z, float m)
{
	;
}

/*  */
void
adjust_rad_momentf(hmapf_t *p, float x, float y, float z, float m)
{
	;
}

/* pan view -=l, +=r, relative, in radians */
void
panf(vf_t *p, float lr)
{
	;
}

/* tilt view -=d, +=u, relative, in radians */
void
tiltf(vf_t *p, float fb)
{
	;
}

/* roll view -=l, +=r, relative, in radians */
void
rollf(vf_t *p, float lr)
{
	;
}

/* zoom view -=b, +=f, relative, in radians */
void
zoomf(vf_t *p, float nf)
{
	;
}

/* try to warp to position x, y, z */
void warpf(vf_t *p, float x, float y, float z)
{
	;
}

/* test warp to position x, y, z */
int
seekposf(vf_t *p, float x, float y, float z)
{
	return 0;
}
