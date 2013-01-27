/* transform.c: hmap transforms.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "progscope.h"
#include "hmap.h"
#include "vectors.h"

/*
   hapticNormill: ... 
*/
void
hapticNormill(select_t *sel)
{
	/* j: i will beta test this */
}

/*
   transform path on hmap vob intersection.
   where bound vol geom's intersected, calculate intersection of draw geom's
   for 2 hmap vob's.  adjust attribs of both accordingly
*/
int
intersection(select_t *sel)
{
	/* this really would like some hashing. for now tho, it will be simple
	   and require the caller's tending: given 2 hmaps, in sel a and b
	   respectively, determine bound intersection.  if intersecting, set
	   new trajectories for both */
	/* hmapf_t **a, **b;
	   btoggles_t t;
	   a = sel->seta; b = sel->setb;
	   t = sel->specbits;
	   __builtin_printf("transform: %i\n", (*(sel->seta))->index); */
	return 0;
}

/*
   allow hmaps to share intersection, attach etc...  requires tending to
   session_filter and data via perhaps rname_hmapf(), enter_group(),
   leave_group(), and group_groups(), all based on hmap session_filter and name.
*/
int
group_hmaps(select_t *sel)
{
	return 0;
}

/*
   join selected vobs.
*/
void
join (select_t *sel)
{
	;
}

/*
   send vobs to recycler.
   the recycler could be written to function as undo stack per vob
   dependant on session name, and perhaps via future supporting data.c.
*/
void
recycler(select_t *sel)
{
	;
}

/*
   write hmap in selection buffer to network or file, as an hmap vob file
   for now assume file/network is meta_u a_file_net_io[10000]
*/
meta_u a_file_net_io[10000]; /* for now */
void
hmapwrapf(select_t *sel)
{
	int *pb = (int *)a_file_net_io;
	int *d, *pi = (int *)&a_file_net_io[1];
	float *pf;
	int i = 0;
	int j = 0;
	hmapf_t *h;
	vf_t *v;

	h = *(sel->seta);
	*pi++ = (int)h->name >> 16; /* ... */
	*pi++ = (int)h->name;
	*pi++ = h->index;
	*pi++ = h->bounding.geom;
	*pi++ = h->draw.geom;
	*pi++ = h->attribs.bits;
	*pi++ = h->attribs.session_filter;
	*pi++ = h->attribs.balance_filter;
	*pi++ = h->mass.kfactor;
	*pi++ = h->kfactor;
	*pi++ = h->vf_total;
	pf = (float *)pi;
	*pf++ = h->v_pos.x;
	*pf++ = h->v_pos.y;
	*pf++ = h->v_pos.z;
	*pf++ = h->v_pos.m;
	*pf++ = h->v_vel.x;
	*pf++ = h->v_vel.y;
	*pf++ = h->v_vel.z;
	*pf++ = h->v_vel.m;
	*pf++ = h->v_axi.x;
	*pf++ = h->v_axi.y;
	*pf++ = h->v_axi.z;
	*pf++ = h->v_axi.m;
	*pf++ = h->v_pre.x;
	*pf++ = h->v_pre.y;
	*pf++ = h->v_pre.z;
	*pf++ = h->v_pre.m;
	*pf++ = h->ang_spd;
	*pf++ = h->ang_dpl;
	*pf++ = h->mass.kg;
	*pf++ = h->bounding.v_sz.x;
	*pf++ = h->bounding.v_sz.y;
	*pf++ = h->bounding.v_sz.z;
	*pf++ = h->bounding.v_sz.m;
	v = h->p_data_vf;
	while(i++ < h->vf_total) {
		*pf++ = v->x;
		*pf++ = v->y;
		*pf++ = v->z;
		*pf++ = v->m;
		v++;
	}
	pi = (int *)pf;
	*pi++ = h->dialog_total;
	d = h->p_dialog;
	while(j++ < h->dialog_total)
		*pi++ = *d++;
	*pb = abs((int)pi - (int)pb) / sizeof(int);
}

/*
   read in an hmap vob file from file or network
   for now assume file/network is meta_u a_file_net_io[10000]
   for now just print it
*/
int
hmapunwrapf(select_t *sel)
{
	float *fl = (float *)a_file_net_io;
	int i, *in = (int *)a_file_net_io;

	__builtin_printf("hmap total: %i\n", (int)*in++); fl++;
	__builtin_printf("vobnum: %i:%i\n", (int)*in++, (int)*in++); fl++; fl++;
	__builtin_printf("index: %i\n", (int)*in++); fl++;
	__builtin_printf("bound geom: %i\n", (int)*in++); fl++;
	__builtin_printf("draw geom: %i\n", (int)*in++); fl++;
	__builtin_printf("attribs bits: %i\n", (int)*in++); fl++;
	__builtin_printf("session filter: %i\n", (int)*in++); fl++;
	__builtin_printf("balance filter: %i\n", (int)*in++); fl++;
	__builtin_printf("mass factor: %i\n", (int)*in++); fl++;
	__builtin_printf("factor: %i\n", (int)*in++); fl++;
	int vf_total = (int)*in;
	__builtin_printf("vf_total: %i\n", (int)*in++); fl++;
	__builtin_printf("pos: %f %f %f %f\n", \
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("vel: %f %f %f %f\n", \
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("axi: %f %f %f %f\n", \
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("pre: %f %f %f %f\n", \
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("ang_spd: %f\n", (float)*fl++); in++;
	__builtin_printf("ang_dpl: %f\n", (float)*fl++); in++;
	__builtin_printf("kg: %f\n", (float)*fl++); in++;
	__builtin_printf("bound size: %f %f %f %f\n", \
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	i=0;
	while(i < vf_total) {
		__builtin_printf("vf %i: %f", i, (float)*fl++); in++;
		__builtin_printf(" %f", (float)*fl++); in++;
		__builtin_printf(" %f", (float)*fl++); in++;
		__builtin_printf(" %f\n", (float)*fl++); in++;
		i++;
	}
	int dialog_total = (int)*in;
	__builtin_printf("dialog_total: %i\n", (int)*in++); fl++;
	i=0;
	__builtin_printf("dialog follows\n");
	while(i < dialog_total) {
		for(;!(i%8);i++)
			__builtin_printf("%x", (int)*in++); fl++;
	}
	__builtin_printf("\n");
	return 0;
}

/*
   resize hmap dialog allocation for entry of dialog theretofore
   preset dialog_total in &sel_a[0] to new value before calling
*/
int
alloc_dialog(select_t *sel)
{
	free((*sel->seta)->p_dialog);
	if(((*sel->seta)->p_dialog = (int *) malloc((*sel->seta)->dialog_total * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}
	return 0;
}

/*
   make a copy of an hmap in hmap vobspace
   hmap copy made less name/index retained by recieving hmap
*/
void
copy_hmapf(select_t *sel)
{
	hmapf_t *a, *b;
	vf_t *v, *w;
	int i, j, *d, *e;

	a = *(sel->seta);
	b = *(sel->setb);
	b->v_vel.x = a->v_vel.x;
	b->v_vel.y = a->v_vel.y;
	b->v_vel.z = a->v_vel.z;
	b->v_vel.m = a->v_vel.m;
	b->v_axi.x = a->v_axi.x;
	b->v_axi.y = a->v_axi.y;
	b->v_axi.z = a->v_axi.z;
	b->v_axi.m = a->v_axi.m;
	b->v_pre.x = a->v_pre.x;
	b->v_pre.y = a->v_pre.y;
	b->v_pre.z = a->v_pre.z;
	b->v_pre.m = a->v_pre.m;
	b->ang_spd = a->ang_spd;
	b->ang_dpl = a->ang_dpl;
	b->mass.kg = a->mass.kg;
	b->mass.kfactor = a->mass.kfactor;
	b->kfactor = a->kfactor;
	b->bounding.geom = a->bounding.geom;
	b->bounding.v_sz.x = a->bounding.v_sz.x;
	b->bounding.v_sz.y = a->bounding.v_sz.y;
	b->bounding.v_sz.z = a->bounding.v_sz.z;
	b->bounding.v_sz.m = a->bounding.v_sz.m;
	b->draw.geom = a->draw.geom;
	b->attribs = a->attribs;
	b->vf_total = a->vf_total;
	free(b->p_data_vf);
        if((b->p_data_vf = (vf_t *) malloc(a->vf_total * sizeof(vf_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: could not malloc vertice data for hmap %i\n",
			__FILE__, __LINE__, b->index);
		abort();
	}
	v = a->p_data_vf;
	w = b->p_data_vf;
	for(i=0;i<a->vf_total;i++, v++) {
		w->x = v->x;
		w->y = v->y;
		w->z = v->z;
		w->m = v->m;
	}
	free(b->p_dialog);
        if((b->p_dialog = (int *) malloc(a->dialog_total * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: could not malloc dialog data for hmap %i\n",
			__FILE__, __LINE__, b->index);
		abort();
	}
	d = a->p_dialog;
	e = b->p_dialog;
	for(j=0;j<a->dialog_total;j++)
		*e++ = *d++;
}
