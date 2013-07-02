/* transform.c: hmap transforms.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "transform.h"
#include "vectors.h"
#include "stock.h" /* ... */
#include "progscope.h"

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

	dif_vf(&(hmapa->vpos), &(hmapb->vpos), &vdist);
	touch = hmapa->envelope.vsz.x + hmapb->envelope.vsz.x;

	if(hmapb->attribs.kg && (&vdist)->m < touch) {

		ma = hmapa->attribs.kg;
		mb = hmapb->attribs.kg;
		inter = touch - (&vdist)->m; /* -intersection */

		/* tend to hmap a */
		vvela = &(hmapa->vvel);
		/* m*v for now */
		mva = ma * vvela->m;
		/* here calculate angle of reflection */

		/* calculate and set acc for a */
		if(mva)
			tele_magz_vf(&vdist, &vacca, scale * inter * mb / mva);
		sum_vf(&vacca, vvela, vvela);

		/* tend to hmap b */
		vvelb = &(hmapb->vvel);
		mvb = mb * vvelb->m;
		if(mvb)
			tele_magz_vf(&vdist, &vaccb, scale * inter * ma / mvb);
		sum_vf(&vaccb, vvelb, vvelb);
	}

#ifdef DIAG_INTERSECTION
	if(hmapb->index == 7) {
		__builtin_printf("\n\nindex a: %i\n", hmapa->index);
		__builtin_printf("  vposa: x %f y %f z %f m %f\n",
			hmapa->vpos.x, hmapa->vpos.y,
			hmapa->vpos.z, hmapa->vpos.m);
		__builtin_printf("  vvela: x %f y %f z %f m %f\n",
			hmapa->vvel.x, hmapa->vvel.y,
			hmapa->vvel.z, hmapa->vvel.m);
		__builtin_printf("  vacca: x %f y %f z %f m %f\n",
			(&vacca)->x, (&vacca)->y, (&vacca)->z, (&vacca)->m);
		__builtin_printf("  vdist: x %f y %f z %f m %f\n",
			(&vdist)->x, (&vdist)->y, (&vdist)->z, (&vdist)->m);
		__builtin_printf("  vaxia: x %f y %f z %f m %f\n\n",
			hmapa->vaxi.x, hmapa->vaxi.y,
			hmapa->vaxi.z, hmapa->vaxi.m);
		/* todo:
		   mass not assigned correctly */
		//__builtin_printf("   kg a: %f\n", ma);
		__builtin_printf("    sza: %f\n", hmapa->envelope.vsz.x);
		__builtin_printf("    mva: %f\n\n", mva);
		__builtin_printf("  inter: %f\n", inter);
		__builtin_printf("  touch: %f\n", touch);
		__builtin_printf("  scale: %f\n\n", scale);
		/* hmap other */
		__builtin_printf("index b: %i\n", hmapb->index);
		__builtin_printf("  vposb: x %f y %f z %f m %f\n",
			hmapb->vpos.x, hmapb->vpos.y,
			hmapb->vpos.z, hmapb->vpos.m);
		__builtin_printf("  vvelb: x %f y %f z %f m %f\n",
			hmapb->vvel.x, hmapb->vvel.y,
			hmapb->vvel.z, hmapb->vvel.m);
		__builtin_printf("  vaccb: x %f y %f z %f m %f\n",
			(&vaccb)->x, (&vaccb)->y, (&vaccb)->z, (&vaccb)->m);
		__builtin_printf("  vaxib: x %f y %f z %f m %f\n\n",
			hmapb->vaxi.x, hmapb->vaxi.y,
			hmapb->vaxi.z, hmapb->vaxi.m);
		 //__builtin_printf("   kg b: %f\n", mb);
		__builtin_printf("    szb: %f\n", hmapb->envelope.vsz.x);
		__builtin_printf("    mvb: %f\n\n", mvb);
		usleep(10000);
	}
#endif /* DIAG_ISECTION */

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
   extend given hmaps by joining them together
*/
int
extend_hmaps(select_t *sel)
{
	return 0;
}

/*
   recycle counta hmaps given in select_a.  for now just detach these.
   the recycler could be written to function as undo stack per vob
   dependant on session name
*/
int
recycle(select_t *sel)
{
	int i;
	hmapf_t *map;

	map = *(sel->seta);
	for(i = 0; i < sel->counta; i++, map++) {
		__builtin_printf(" recycling: %x\n", (int)map->name);
		map->attribs.bits |= VRT_MASK_DETACH;
	}

	return 0;
}

/*
   format and write to a proto .vrtmap, hmap referenced in the selection
   buffer to network or file.  this will be further optimized for sending
   over net by removing anything that can not be calculated inherently.
   for example the magnitudes of vectors as these may calculated on unwrap.
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
	*pf++ = h->vpos.x;
	*pf++ = h->vpos.y;
	*pf++ = h->vpos.z;
	*pf++ = h->vpos.m;
	*pf++ = h->vvel.x;
	*pf++ = h->vvel.y;
	*pf++ = h->vvel.z;
	*pf++ = h->vvel.m;
	*pf++ = h->vaxi.x;
	*pf++ = h->vaxi.y;
	*pf++ = h->vaxi.z;
	*pf++ = h->vaxi.m;
	*pf++ = h->vrel.x;
	*pf++ = h->vrel.y;
	*pf++ = h->vrel.z;
	*pf++ = h->vrel.m;
	*pf++ = h->vpre.x;
	*pf++ = h->vpre.y;
	*pf++ = h->vpre.z;
	*pf++ = h->vpre.m;
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
	*pf++ = h->envelope.vsz.x;
	*pf++ = h->envelope.vsz.y;
	*pf++ = h->envelope.vsz.z;
	*pf++ = h->envelope.vsz.m;
	pi = (int *)pf;
	*pi++ = h->draw.geom;
	*pi++ = h->vmap_total;
	pf = (float *)pi;
	v = h->vmap;
	while(i++ < h->vmap_total) {
		*pf++ = v->x;
		*pf++ = v->y;
		*pf++ = v->z;
		*pf++ = v->m;
		v++;
	}
	pi = (int *)pf;
	*pi++ = h->dialog_len;
	d = h->dialog;
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
	int vmap_total = (int)*in;
	__builtin_printf("vmap_total: %i\n", (int)*in++); fl++;
	i = 0;
	while(i < vmap_total) {
		__builtin_printf("vf %i: %f", i + 1, (float)*fl++); in++;
		__builtin_printf(" %f", (float)*fl++); in++;
		__builtin_printf(" %f", (float)*fl++); in++;
		__builtin_printf(" %f\n", (float)*fl++); in++;
		i++;
	}
	int dialog_len = (int)*in;
	__builtin_printf("dialog_len: %i\n", (int)*in++); fl++;
	i = 0;
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
	vf_t *av, *bv;
	int i, *ad, *bd;

	a = *(sel->seta);
	b = *(sel->setb);
	b->vvel.x = a->vvel.x;
	b->vvel.y = a->vvel.y;
	b->vvel.z = a->vvel.z;
	b->vvel.m = a->vvel.m;
	b->vaxi.x = a->vaxi.x;
	b->vaxi.y = a->vaxi.y;
	b->vaxi.z = a->vaxi.z;
	b->vaxi.m = a->vaxi.m;
	b->vrel.x = a->vrel.x;
	b->vrel.y = a->vrel.y;
	b->vrel.z = a->vrel.z;
	b->vrel.m = a->vrel.m;
	b->vpre.x = a->vpre.x;
	b->vpre.y = a->vpre.y;
	b->vpre.z = a->vpre.z;
	b->vpre.m = a->vpre.m;
	b->ang_spd = a->ang_spd;
	b->ang_dpl = a->ang_dpl;
	b->attribs.bits = a->attribs.bits;
	b->attribs.modifiers = a->attribs.modifiers;
	b->attribs.balance_filter = a->attribs.balance_filter;
	b->attribs.kg = a->attribs.kg;
	b->attribs.kfactorm = a->attribs.kfactorm;
	b->envelope.geom = a->envelope.geom;
	b->envelope.vsz.x = a->envelope.vsz.x;
	b->envelope.vsz.y = a->envelope.vsz.y;
	b->envelope.vsz.z = a->envelope.vsz.z;
	b->envelope.vsz.m = a->envelope.vsz.m;
	b->draw.geom = a->draw.geom;

	if(a->vmap_total) {
		free(b->vmap);
		b->vmap = NULL;
		if((b->vmap = (vf_t *) malloc(a->vmap_total * sizeof(vf_t))) == NULL) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: could not malloc vertice data "
				"copying hmap %i to %i\n",
				__FILE__, __LINE__, a->index, a->index);
			abort();
		}
		av = a->vmap;
		bv = b->vmap;
		for(i = 0; i < a->vmap_total; i++, av++, bv++) {
			bv->x = av->x;
			bv->y = av->y;
			bv->z = av->z;
			bv->m = av->m;
		}
		b->vmap_total = a->vmap_total;
	}

	if(a->dialog_len) {
		free(b->dialog);
		b->dialog = NULL;
	   if((b->dialog = (int *) malloc(a->dialog_len * sizeof(int))) == NULL) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: could not malloc dialog data copying hmap %i to %i\n",
				__FILE__, __LINE__, a->index, b->index);
			abort();
		}
		ad = a->dialog;
		bd = b->dialog;
		for(i = 0; i < a->dialog_len; i++)
			*bd++ = *ad++;
		b->dialog_len = a->dialog_len;
	}

	return 0;
}

/*
   for c series hmaps, invert surface normals of (for now) a single hmap
   referenced as first item in *(sel->seta) by inverting the precedence of
   drawing for each vertice.
*/
int
surfinv_hmapf(select_t *sel)
{
	int i;
	hmapf_t *map;
	vf_t *org, *cpy;

	map = *(sel->seta);
	org = map->vmap;
	if((cpy = (vf_t *) malloc(map->vmap_total * sizeof(vf_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
		"Error: Could not malloc vertice data when inverting hmap %i\n",
		__FILE__, __LINE__, map->index);
		abort();
	}

	for(i = 0; i < map->vmap_total; i++) {
		cpy->x = org->x;
		cpy->y = org->y;
		cpy->z = org->z;
		(cpy++)->m = (org++)->m;
	}

	org = map->vmap;
	for(i = map->vmap_total; i > 0; i--) {
		org->x = (--cpy)->x;
		org->y = cpy->y;
		org->z = cpy->z;
		(org++)->m = cpy->m;
	}

	free(cpy);
	return 0;
}

/* functions affecting allocation of hmap data */

/*
   hmap dialog referenced thru sel->seta recieves allocation of len + 1 int's
*/
int
alloc_dialog(select_t *sel, int len)
{
	(*sel->seta)->dialog = NULL;
	if(((*sel->seta)->dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}
	(*sel->seta)->dialog_len = 0;
	*(*sel->seta)->dialog = '\0';
	return 0;
}

/* write or append s to dialog(for now at end of current dialog) for any given
   hmap thru sel->seta.  when supported will also insert or delete count at
   offset to s, where sign of count will determine insert or delete and sense
   of count will be after offset */
int
add_dialog(select_t *sel, char *s, int count, int offset)
{
	int i, addlen, orglen, newlen, *pti, *pti2, *swap = NULL;
	char *ptc;

	if((*sel->seta)->dialog) {
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
		pti = (*sel->seta)->dialog;
		pti2 = swap;
		for(i = 0; i < orglen; i++)
			*pti2++ = *pti++;

		/* (re-)allocate memory for dialog with append */
		free((*sel->seta)->dialog);
		alloc_dialog(sel, newlen);

		/* write original string from swap */
		pti = (*sel->seta)->dialog;
		pti2 = swap;
		for(i = 0; i < orglen; i++)
			*pti++ = *pti2++;
		free(swap);
	} else {
		newlen = addlen = strlen(s);
		alloc_dialog(sel, addlen);
		pti = (*sel->seta)->dialog;
	}

	/* write new string */
	ptc = s;
	for(i = 0; i < addlen; i++)
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
	free((*sel->seta)->dialog);
	(*sel->seta)->dialog = NULL;

	/* allocate */
	len = strlen(s);
	if(((*sel->seta)->dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}

	/* write */
	p = s;
	d = (*sel->seta)->dialog;
	for(i = 0; i < len; i++)
		*d++ = *p++;
	*d = '\0';

	(*sel->seta)->dialog_len = len;

	return 0;
}
