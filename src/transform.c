/* transform.c: hmap transforms.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "hmap.h"
#include "vectors.h"

float vrt_render_cyc; /* external */


/* transform hmaps
   given two arrays referencing one or two set's of hmaps, affect those hmaps
   with given transform function that operate's independantly or as a mutually
   dependant set of transforms available at given compile time */

/*
   hapticNormill: ... 
*/
int
hapticNormill(select_t *sel)
{
	/* j: i will beta test this */
	return 0;
}

/*
   transform vob paths on hmap intersection.
   where hmap bound vol geom's intersected, calculate intersection. adjust
   attribs of both hmaps accordingly.
   for now, given 2 spherical hmap vob's in sel a and b respectively.
   notes: cheat by assuming equality between attributes of represented solids
*/
int
intersection(select_t *sel)
{
	/*
	btoggles_t toggles;
	toggles = sel->specbits;
	- determine bound geom
	*/

	hmapf_t *hmapa, *hmapb;
	vf_t *vvela, *vvelb;
	vf_t vacca = {0, 0, 0, 0};
	vf_t vaccb = {0, 0, 0, 0};
	vf_t vdist = {0, 0, 0, 0};
	float touch, inter, ma, mb, scale;
	float mva, mvb; /* for now */

	hmapa = *(sel->seta);
	hmapb = *(sel->setb);

	scale = .00005;

	dif_vf(&(hmapa->v_pos), &(hmapb->v_pos), &vdist);
	touch = hmapa->envelope.v_sz.x + hmapb->envelope.v_sz.x;

	if(hmapb->attribs.kg && (&vdist)->m < touch) {

		ma = hmapa->attribs.kg;
		mb = hmapb->attribs.kg;
		inter = touch - (&vdist)->m; /* -intersection */

		/* tend to hmap a */
		vvela = &(hmapa->v_vel);
		/* m*v for now */
		mva = (ma * vvela->m) ? ma * vvela->m : .000001;
		/* here calculate angle of reflection */

		/* calculate and set acc for a */
		tele_magz_vf(&vdist, &vacca,
			scale * inter * mb / mva);
		sum_vf(&vacca, vvela, vvela);

		/* tend to hmap b */
		vvelb = &(hmapb->v_vel);
		mvb = (mb * vvelb->m) ? mb * vvelb->m: .000001;
		tele_magz_vf(&vdist, &vaccb,
			scale * inter * ma / mvb);
		sum_vf(&vaccb, vvelb, vvelb);
	}

#define DIAG_OFF
#ifdef DIAG
	if(hmapb->index == 7) {
		__builtin_printf("\n\nindex a: %i\n", hmapa->index);
		__builtin_printf("  vposa: x %f y %f z %f m %f\n",
			hmapa->v_pos.x, hmapa->v_pos.y,
			hmapa->v_pos.z, hmapa->v_pos.m);
		__builtin_printf("  vvela: x %f y %f z %f m %f\n",
			hmapa->v_vel.x, hmapa->v_vel.y,
			hmapa->v_vel.z, hmapa->v_vel.m);
		__builtin_printf("  vacca: x %f y %f z %f m %f\n",
			(&vacca)->x, (&vacca)->y, (&vacca)->z, (&vacca)->m);
		__builtin_printf("  vdist: x %f y %f z %f m %f\n",
			(&vdist)->x, (&vdist)->y, (&vdist)->z, (&vdist)->m);
		__builtin_printf("  vaxia: x %f y %f z %f m %f\n\n",
			hmapa->v_axi.x, hmapa->v_axi.y,
			hmapa->v_axi.z, hmapa->v_axi.m);
		/* todo:
		   mass not assigned correctly */
		//__builtin_printf("   kg a: %f\n", ma);
		__builtin_printf("    sza: %f\n", hmapa->envelope.v_sz.x);
		__builtin_printf("    mva: %f\n\n", mva);
		__builtin_printf("  inter: %f\n", inter);
		__builtin_printf("  touch: %f\n", touch);
		__builtin_printf("  scale: %f\n\n", scale);
		/* hmap other */
		__builtin_printf("index b: %i\n", hmapb->index);
		__builtin_printf("  vposb: x %f y %f z %f m %f\n",
			hmapb->v_pos.x, hmapb->v_pos.y,
			hmapb->v_pos.z, hmapb->v_pos.m);
		__builtin_printf("  vvelb: x %f y %f z %f m %f\n",
			hmapb->v_vel.x, hmapb->v_vel.y,
			hmapb->v_vel.z, hmapb->v_vel.m);
		__builtin_printf("  vaccb: x %f y %f z %f m %f\n",
			(&vaccb)->x, (&vaccb)->y, (&vaccb)->z, (&vaccb)->m);
		__builtin_printf("  vaxib: x %f y %f z %f m %f\n\n",
			hmapb->v_axi.x, hmapb->v_axi.y,
			hmapb->v_axi.z, hmapb->v_axi.m);
		 //__builtin_printf("   kg b: %f\n", mb);
		__builtin_printf("    szb: %f\n", hmapb->envelope.v_sz.x);
		__builtin_printf("    mvb: %f\n\n", mvb);
		usleep(10000);
	}
#endif /* DIAG */

	return 0;
}

/*
   allow hmaps to share intersection, attach etc...  requires tending to
   session_filter and data via perhaps rname_hmapf(), enter_group(),
   leave_group(), and group_groups(), all based on hmap session_filter and name
*/
int
group_hmaps(select_t *sel)
{
	return 0;
}

/*
   join given hmaps
*/
int
join_hmaps(select_t *sel)
{
	return 0;
}

/*
   send vobs to recycler.
   the recycler could be written to function as undo stack per vob
   dependant on session name
*/
int
recycler(select_t *sel)
{
	return 0;
}

/*
   format and write to a proto .vrtmap, hmap referenced in the selection
   buffer to network or file.  this will be further optimized for sending
   over net by removing anything that can not be calculated inherently.
   this will then ofc lead to a revised hmapunwrapf()(see below).  for now
   assume file/network is meta_u a_file_net_io[10000].  later actually do the
   file write or and pass it over the network
*/
meta_u a_file_net_io[10000]; /* for now */
int
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
	*pi++ = (int)(h->name >> 16);
	*pi++ = (int)(h->name & 0xffff);
	*pi++ = h->index;
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
	*pf++ = h->v_rel.x;
	*pf++ = h->v_rel.y;
	*pf++ = h->v_rel.z;
	*pf++ = h->v_rel.m;
	*pf++ = h->v_pre.x;
	*pf++ = h->v_pre.y;
	*pf++ = h->v_pre.z;
	*pf++ = h->v_pre.m;
	*pf++ = h->ang_spd;
	*pf++ = h->ang_dpl;
	pi = (int *)pf;
	*pi++ = h->attribs.bits;
	*pi++ = h->attribs.modifiers;
	*pi++ = h->attribs.session_filter;
	*pi++ = h->attribs.balance_filter;
	pf = (float *)pi;
	*pf++ = h->attribs.kg;
	pi = (int *)pf;
	*pi++ = h->attribs.kfactorm;
	*pi++ = h->attribs.kfactord;
	*pi++ = h->envelope.geom;
	pf = (float *)pi;
	*pf++ = h->envelope.v_sz.x;
	*pf++ = h->envelope.v_sz.y;
	*pf++ = h->envelope.v_sz.z;
	*pf++ = h->envelope.v_sz.m;
	pi = (int *)pf;
	*pi++ = h->draw.geom;
	*pi++ = h->vf_total;
	pf = (float *)pi;
	v = h->p_data_vf;
	while(i++ < h->vf_total) {
		*pf++ = v->x;
		*pf++ = v->y;
		*pf++ = v->z;
		*pf++ = v->m;
		v++;
	}
	pi = (int *)pf;
	*pi++ = h->dialog_len;
	d = h->p_dialog;
	while(j++ < h->dialog_len)
		*pi++ = *d++;
	*pb = abs((int)pi - (int)pb) / sizeof(int);
	return 0;
}

/*
   read .vrtmap file/transfer formated hmap into selection buffer.  for now
   assume file/network is meta_u a_file_net_io[10000].  for now display it's
   contents to stdout.  later write it to an hmap, allocating for vf_t's and
   dialog.  as anything that can be calculated inherently will be omitted from
   an hmap file, this function will eventually be revised
*/
int
hmapunwrapf(select_t *sel)
{
	float *fl = (float *)a_file_net_io;
	int i, graph, space, ctrl, null, *in = (int *)a_file_net_io;
	graph = 0; space = 0; ctrl = 0; null = 0;

	__builtin_printf("hmapf size: %i\n", (int)*in++); fl++;
	__builtin_printf("vobnum: %i:", (int)*in++); fl++;
	__builtin_printf("%i\n", (int)*in++); fl++;
	__builtin_printf("index: %i\n", (int)*in++); fl++;
	__builtin_printf("pos: %f %f %f %f\n",
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("vel: %f %f %f %f\n",
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("axi: %f %f %f %f\n",
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("rel: %f %f %f %f\n",
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("pre: %f %f %f %f\n",
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("ang_spd: %f\n", (float)*fl++); in++;
	__builtin_printf("ang_dpl: %f\n", (float)*fl++); in++;
	__builtin_printf("attribs bits: 0x%x\n", (int)*in++); fl++;
	__builtin_printf("attribs modifiers: 0x%x\n", (int)*in++); fl++;
	__builtin_printf("session filter: 0x%x\n", (int)*in++); fl++;
	__builtin_printf("balance filter: 0x%x\n", (int)*in++); fl++;
	__builtin_printf("kg: %f\n", (float)*fl++); in++;
	__builtin_printf("kfactorm: %i\n", (int)*in++); fl++;
	__builtin_printf("kfactord: %i\n", (int)*in++); fl++;
	__builtin_printf("bound geom: %i\n", (int)*in++); fl++;
	__builtin_printf("bound size: %f %f %f %f\n", \
		(float)*fl++, (float)*fl++, (float)*fl++, (float)*fl++);
	in++; in++; in++; in++;	
	__builtin_printf("draw geom: %i\n", (int)*in++); fl++;
	int vf_total = (int)*in;
	__builtin_printf("vf_total: %i\n", (int)*in++); fl++;
	i=0;
	while(i < vf_total) {
		__builtin_printf("vf %i: %f", i, (float)*fl++); in++;
		__builtin_printf(" %f", (float)*fl++); in++;
		__builtin_printf(" %f", (float)*fl++); in++;
		__builtin_printf(" %f\n", (float)*fl++); in++;
		i++;
	}
	int dialog_len = (int)*in;
	__builtin_printf("dialog_len: %i\n", (int)*in++); fl++;
	i=0;
	__builtin_printf("dialog:\n");
	while(i < dialog_len) {
		/* print !last char 1 thru 7 of rows of 8 chars */
		for(;!((i%8)==7) && ((i+1)%dialog_len);i++) {
			if(iscntrl((char)*in)) { ctrl += 1; }
			if(isgraph((char)*in)) { graph += 1; }
			if(isspace((char)*in)) { space += 1; }
			if(!(*in)) { null += 1; }
			__builtin_printf("%10.8x", (int)*in++); fl++;
		}
		if((i+1)%dialog_len) { /* print !last char 8 in row of 8 */
			if(iscntrl((char)*in)) { ctrl += 1; }
			if(isgraph((char)*in)) { graph += 1; }
			if(isspace((char)*in)) { space += 1; }
			if(!(*in)) { null += 1; }
			__builtin_printf("%10.8x\n", (int)*in++); fl++;
			i++;
		} else { /* last char */
			if(iscntrl((char)*in)) { ctrl += 1; }
			if(isgraph((char)*in)) { graph += 1; }
			if(isspace((char)*in)) { space += 1; }
			if(!(*in)) { null += 1; }
			__builtin_printf("%10.8x\n", (int)*in++); fl++;
			i++; i++;
		}
	}
	__builtin_printf("dialog gscn: %i %i %i %i\n", graph, space, ctrl, null);
	return 0;
}

/*
   make a copy of (for now) a single hmap referenced as first item in
   *(sel->seta) into hmap memory referenced as first item in *(sel->setb).
   recieving hmap retains name/index, session_filter, position, and kfactord */
int
cp_hmapf(select_t *sel)
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
	b->v_rel.x = a->v_rel.x;
	b->v_rel.y = a->v_rel.y;
	b->v_rel.z = a->v_rel.z;
	b->v_rel.m = a->v_rel.m;
	b->v_pre.x = a->v_pre.x;
	b->v_pre.y = a->v_pre.y;
	b->v_pre.z = a->v_pre.z;
	b->v_pre.m = a->v_pre.m;
	b->ang_spd = a->ang_spd;
	b->ang_dpl = a->ang_dpl;
	b->attribs.bits = a->attribs.bits;
	b->attribs.modifiers = a->attribs.modifiers;
	b->attribs.balance_filter = a->attribs.balance_filter;
	b->attribs.kg = a->attribs.kg;
	b->attribs.kfactorm = a->attribs.kfactorm;
	b->envelope.geom = a->envelope.geom;
	b->envelope.v_sz.x = a->envelope.v_sz.x;
	b->envelope.v_sz.y = a->envelope.v_sz.y;
	b->envelope.v_sz.z = a->envelope.v_sz.z;
	b->envelope.v_sz.m = a->envelope.v_sz.m;
	b->draw.geom = a->draw.geom;
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
        if((b->p_dialog = (int *) malloc(a->dialog_len * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: could not malloc dialog data for hmap %i\n",
			__FILE__, __LINE__, b->index);
		abort();
	}
	d = a->p_dialog;
	e = b->p_dialog;
	for(j=0;j<a->dialog_len;j++)
		*e++ = *d++;
	return 0;
}


/* functions affecting allocation of hmap data */

/*
   hmap dialog referenced thru sel->seta recieves allocation of len + 1 int's
*/
int
alloc_dialog(select_t *sel, int len)
{
	(*sel->seta)->p_dialog = NULL;
	if(((*sel->seta)->p_dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}
	(*sel->seta)->dialog_len = 0;
	*(*sel->seta)->p_dialog = '\0';
	return 0;
}

/* write, append, or insert s into any given hmap dialog thru sel->seta */
int
add_dialog(select_t *sel, char *s, int offset)
{
	int i, addlen, orglen, newlen, *pti, *pti2, *swap = NULL;
	char *ptc;

	if((*sel->seta)->p_dialog) {
		/* allocate swap, memory to hold previous dialog */
		if((swap = (int *) malloc((*sel->seta)->dialog_len * sizeof(int))) == NULL) {
			__builtin_fprintf(stderr,  "vrtater:%s:%d: "
				"Error: Could not malloc for add_dialog()\n",
				__FILE__, __LINE__);
			abort();
		}

		orglen = (*sel->seta)->dialog_len; /* calc before freeing */
		addlen = strlen(s);
		newlen = orglen + addlen;

		/* put original hmap dialog in swap */
		pti = (*sel->seta)->p_dialog;
		pti2 = swap;
		for(i=0;i<orglen;i++)
			*pti2++ = *pti++;

		/* (re-)allocate memory for dialog with append */
		free((*sel->seta)->p_dialog);
		alloc_dialog(sel, newlen);

		/* write original string from swap */
		pti = (*sel->seta)->p_dialog;
		pti2 = swap;
		for(i=0;i<orglen;i++)
			*pti++ = *pti2++;
		free(swap);
	} else {
		newlen = addlen = strlen(s);
		alloc_dialog(sel, addlen);
		pti = (*sel->seta)->p_dialog;
	}

	/* write new string */
	ptc = s;
	for(i=0;i<addlen;i++)
		*pti++ = *ptc++;
	*pti = '\0';

	/* tend nicely */
	(*sel->seta)->dialog_len = newlen;

	return 0;
}

/* hmap dialog referenced thru sel->seta becomes string s */
int
write_dialog(select_t *sel, char *s)
{
	int i, len, *d;
	char *p;

	/* free previous */
	free((*sel->seta)->p_dialog);
	(*sel->seta)->p_dialog = NULL;

	/* allocate */
	len = strlen(s);
	if(((*sel->seta)->p_dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}

	/* write */
	p = s;
	d = (*sel->seta)->p_dialog;
	for(i=0;i<len;i++)
		*d++ = *p++;
	*d = '\0';

	(*sel->seta)->dialog_len = len;

	return 0;
}
