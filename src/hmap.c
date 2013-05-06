/* hmap.c: calls to retreive and help build hmaps(vob attribute data struct).
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "progscope.h"
#include "vectors.h"
#include "attribs.h"

unsigned int vrt_hmaps_max; /* external */

/* selection buffer */
hmapf_t *selectf_a; /* external */
hmapf_t *selectf_b; /* external */

void
init_selection_buffers(void)
{
	/* allocate hmapf selection buffers
	   note: dont talk about double number calls :-| */
	if((selectf_a = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_a\n",
			__FILE__, __LINE__);
		abort();
	}
	if((selectf_b = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_b\n",
			__FILE__, __LINE__);
		abort();
	}
}

/* attach an empty hmap to given session */
hmapf_t *
hmapf(session_t *psession)
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
