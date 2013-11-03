/* transform.c: hmap transforms.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "transform.h"
#include "vectors.h"
#include "stock.h"
#include "global.h"
#include "rotation.h"

float vrt_render_cyc; /* external */


/* Transform hmaps.
   Given two buffers referencing one or two set's of hmaps, affect those hmaps
   with given transform function that operate's independantly or as a dependant
   set, in some way altering at least one hmap. */

/* Enable hmap referenced in selectf_a, thereafter having VRT_MASK_TRUNKMAP
   and composite pointer set thus leading a linked list of hmaps referenced in
   selectf_b .  Following in list are then imbued with bit VRT_MASK_TREEMAP
   and adjoined pointer set thus positionally relative in vobspace thereafter
   vs. certain contextual group transforms.  This then providing for example,
   case where trunkmap might represent a fulcrum, with 2 linked maps
   representing ends of a lever.  Leverage across this fulcrum could then be
   optionally tested vs. intersection through hmap options where the two ends
   were attached as fixed vs. hinged.  Linked maps are parsed by proc_hmapf at
   same time as their trunkmap.  They are thus not sent to proc_hmapf.  Their
   linkage allows for them to be composed and stored in a sequence through
   their composite pointer.  Through their adjoined pointer they specify
   their relative position vs. other map.  Through their drawlist pointer, they
   are processed in the correct precedence.  Whenever a trunkmap is read
   leading a compounded .vrtater file on filesystem, any treemap sequencially
   following is joined to it using this function.  Calls to this function cause
   a resend of the effecting set to connected nodes where applicable.
   unjoin_hmaps compliment function reverses this process.  note: This function
   was to be called hapticNormill, as vrtater has much of it's inspiration in a
   haptic feedback project that took place to a significant extent at a certain
   most cozy mill, that project inspired by another moreso well known haptic
   feedback project related. */
int
join_hmaps(select_t *sel)
{
	/* Scheduled next+1. */

	return 0;
}

/* Reverse the process of join_hmaps for trunkmap in selectf_a vs. all linked
   maps in selectf_b.  Calls to this function cause a resend of the effecting
   set to connected nodes where applicable. */
int
unjoin_hmap(select_t *sel)
{
	return 0;
}

/* Merge hmaps in selectf_a together forming new hmap refrenced in selectf_b. */
int
extend_hmaps(select_t *sel)
{
	return 0;
}

/* Given groupmap refrenced in selectf_a, add keymaps refrenced in selectf_b to
   the dialog based group whose ptlgrp_t member map_name is equal to groupmap
   name.  keymaps must have the same hash part in their session name as the
   groupmap for this to succeed, also indicating that they are in the same
   partial. */
int
add_dialog_members(select_t *sel)
{
	return 0;
}

/* Given groupmap refrenced in selectf_a remove member keymaps refrenced in
   selectf_b. */
int
subtract_dialog_members(select_t *sel)
{
	return 0;
}

int
swap_hmaps(select_t *sel)
{
	return 0;
}

int
scale_hmap_c(select_t *sel)
{
	return 0;
}

int
bifold_hmap_c(select_t *sel)
{
	return 0;
}

/* Given reference to 2 hmaps in sel a and b respectively, transform path
   attributes of these apon hmap intersection.  Apply deformation to hmaps
   where intersection occurs.  notes: Still in progress.  For now, simply uses
   spherical bounding volumes of radius envelope.vsz and mass of attribs.kg per
   any hmaps.  Representation of mass is proportional to volume.  scheduled:
   Support for type cylinder and cube bounding volumes.  Calculating for
   triangles directly once within bounds.  Inside intersection and deformation. */
int
intersection(select_t *sel)
{
	hmapf_t *mapa, *mapb;
	vf_t *vvela, *vvelb;
	vf_t vacca = {0, 0, 0, 0};
	vf_t vaccb = {0, 0, 0, 0};
	vf_t vdist = {0, 0, 0, 0};
	float centerdist, isectn, ma, mb, scale;
	float mva, mvb; /* for now */
	int i, j;
	vf_t *idxtri_a, *idxtri_b, *nearesta, *nearestb, centroida, centroidb, contact;
	vf_t *vmap, disp_a, disp_b, disp_c, tmp, keya, keyb, keyc, alt_a, alt_b, alt_c, dif;
	float tdist = 0x7f7fffff;
	float plasmaconst = .15;
	float threshold = .0005; /* generated hmaps need slack */


	mapa = *(sel->seta);
	mapb = *(sel->setb);

	if (mapa->name == mapb->name)
		return 0;

	scale = .0008;

	dif_vf(&(mapa->vpos), &(mapb->vpos), &vdist);
	form_mag_vf(&vdist);
	centerdist = mapa->envelope.vsz.x + mapb->envelope.vsz.x;

	/* Apon bound intersection, determine closest triangles vs. centroid
	   of each in both hmaps.  For now, using spherical bounds checking
	   for all hmaps.  Functionality needs be further considered for mixed
	   bound types. */
	if (mapb->attribs.kg && (&vdist)->m < centerdist) {

		/* Determine 2 closest triangles.
		   For now, assuming triangles, reduce those in mapa/mapb to
		   match on nearest with an exaustive search.  Leave match
		   references in nearesta/nearestb. */
		for (i = 0, idxtri_a = mapa->vmap; i < mapa->vmap_total; i += 3) {

			/* Find centroid a. */
			(&centroida)->x = ((idxtri_a)->x + (idxtri_a + 1)->x + (idxtri_a + 2)->x) / 3;
			(&centroida)->y = ((idxtri_a)->y + (idxtri_a + 1)->y + (idxtri_a + 2)->y) / 3;
			(&centroida)->z = ((idxtri_a)->z + (idxtri_a + 1)->z + (idxtri_a + 2)->z) / 3;

			/* Position centroid a absolutely. */
			sum_vf(&(mapa->vpos), rotate_vf(form_mag_vf(&centroida), &(mapa->vaxi), mapa->ang_dpl, &centroida), &centroida);

			for (j = 0, idxtri_b = mapb->vmap; j < mapb->vmap_total; j += 3) {

				/* Find centroid b. */
				(&centroidb)->x = ((idxtri_b)->x + (idxtri_b + 1)->x + (idxtri_b + 2)->x) / 3;
				(&centroidb)->y = ((idxtri_b)->y + (idxtri_b + 1)->y + (idxtri_b + 2)->y) / 3;
				(&centroidb)->z = ((idxtri_b)->z + (idxtri_b + 1)->z + (idxtri_b + 2)->z) / 3;

				/* Position centroid b absolutely. */
				sum_vf(&(mapb->vpos), rotate_vf(form_mag_vf(&centroidb), &(mapb->vaxi), mapb->ang_dpl, &centroidb), &centroidb);

				/* Produce difference between absolute centroids a and b. */
				dif_vf(&centroida, &centroidb, &dif);

				/* Index closest triangles so far and approximate absolute contact point for a. */
				if ((&dif)->m < tdist) {
					tdist = (&dif)->m;
					nearesta = idxtri_a;
					nearestb = idxtri_b;
					cp_vf(&centroida, &contact);
				}
				idxtri_b += 3;
			}
			idxtri_a += 3;
		}

		/* Retreive absolute altitudes for nearest triangle in b. */
		sum_vf(&(mapb->vpos), rotate_vf(nearestb, &(mapb->vaxi), mapb->ang_dpl, &alt_a), &alt_a);
		sum_vf(&(mapb->vpos), rotate_vf(nearestb + 1, &(mapb->vaxi), mapb->ang_dpl, &alt_b), &alt_b);
		sum_vf(&(mapb->vpos), rotate_vf(nearestb + 2, &(mapb->vaxi), mapb->ang_dpl, &alt_c), &alt_c);

		/* Find in triangle b, absolute altitude displacements vs. triangle a absolute contact point. */
		dif_vf(&alt_a, &contact, &disp_a);
		dif_vf(&alt_b, &contact, &disp_b);
		dif_vf(&alt_c, &contact, &disp_c);

		/* Deformation.  If vertices are to remain syncronized,
		   exaustively scan for those, for now.  Adjust altitudes in
		   relative triangle b vs. given displacements along vvel, while
		   optionally syncronizing vertices. */
		if (!(mapb->attribs.mode & VRT_MASK_FIXED_FORM)) {
			if (mapb->attribs.mode & VRT_MASK_SYNC_VERTICES) {

				/* Take a copy of the key triangle vertices. */
				cp_vf(nearestb, &keya);
				cp_vf(nearestb + 1, &keyb);
				cp_vf(nearestb + 2, &keyc);

				/* Move the key triangle into new position. */
				tele_magz_vf(&(mapa->vvel), (&disp_a)->m * plasmaconst, &tmp);
				sum_vf(nearestb, rotate_vf(&tmp, &(mapb->vaxi),-mapb->ang_dpl, &tmp), nearestb);
				tele_magz_vf(&(mapa->vvel), (&disp_b)->m * plasmaconst, &tmp);
				sum_vf(nearestb + 1, rotate_vf(&tmp, &(mapb->vaxi),-mapb->ang_dpl, &tmp), nearestb + 1);
				tele_magz_vf(&(mapa->vvel), (&disp_c)->m * plasmaconst, &tmp);
				sum_vf(nearestb + 2, rotate_vf(&tmp, &(mapb->vaxi),-mapb->ang_dpl, &tmp), nearestb + 2);

				/* Adjust any vertices previously attached to the key triangle. */
				for (i = 0, vmap = mapb->vmap; i < mapb->vmap_total; i++, vmap++) {
					if ((vmap->x + threshold > (&keya)->x) && (vmap->x - threshold < (&keya)->x))
						if ((vmap->y + threshold > (&keya)->y) && (vmap->y - threshold < (&keya)->y))
							if ((vmap->z + threshold > (&keya)->z) && (vmap->z - threshold < (&keya)->z))
								cp_vf(nearestb, vmap);
					if ((vmap->x + threshold > (&keyb)->x) && (vmap->x - threshold < (&keyb)->x))
						if ((vmap->y + threshold > (&keyb)->y) && (vmap->y - threshold < (&keyb)->y))
							if ((vmap->z + threshold > (&keyb)->z) && (vmap->z - threshold < (&keyb)->z))
								cp_vf(nearestb + 1, vmap);
					if ((vmap->x + threshold > (&keyc)->x) && (vmap->x - threshold < (&keyc)->x))
						if ((vmap->y + threshold > (&keyc)->y) && (vmap->y - threshold < (&keyc)->y))
							if ((vmap->z + threshold > (&keyc)->z) && (vmap->z - threshold < (&keyc)->z))
								cp_vf(nearestb + 2, vmap);
				}
			} else {
				/* Move the key triangle into new position. */
				tele_magz_vf(&(mapa->vvel), (&disp_a)->m * plasmaconst, &tmp);
				sum_vf(nearestb, rotate_vf(&tmp, &(mapb->vaxi),-mapb->ang_dpl, &tmp), nearestb);
				tele_magz_vf(&(mapa->vvel), (&disp_b)->m * plasmaconst, &tmp);
				sum_vf(nearestb + 1, rotate_vf(&tmp, &(mapb->vaxi),-mapb->ang_dpl, &tmp), nearestb + 1);
				tele_magz_vf(&(mapa->vvel), (&disp_c)->m * plasmaconst, &tmp);
				sum_vf(nearestb + 2, rotate_vf(&tmp, &(mapb->vaxi),-mapb->ang_dpl, &tmp), nearestb + 2);
			}
		}

		/* Calculate reflection. */
		ma = mapa->attribs.kg;
		mb = mapb->attribs.kg;
		isectn = centerdist - (&vdist)->m; /* -intersection */

		/* Angle of reflection. */
		/* ... */

		/* Acceleration for mapa. */
		vvela = &(mapa->vvel);
		/* m*v for now */
		mva = ma * vvela->m;
		if (mva)
			tele_magz_vf(&vdist, scale * isectn * mb * (2 * (1 / mb)) / mva, &vacca);
		sum_vf(&vacca, vvela, vvela);

		/* Tend to mapb. */
		vvelb = &(mapb->vvel);
		mvb = mb * vvelb->m;
		if (mvb)
			tele_magz_vf(&vdist, scale * isectn * ma / mvb, &vaccb);
		sum_vf(&vaccb, vvelb, vvelb);
	}

#ifdef DIAG_INTERSECTION
	if (mapb->index == 7) {
		__builtin_printf("\n\nindex a: %i\n", mapa->index);
		__builtin_printf("  vposa: x %f y %f z %f m %f\n",
			mapa->vpos.x, hmapa->vpos.y,
			mapa->vpos.z, hmapa->vpos.m);
		__builtin_printf("  vvela: x %f y %f z %f m %f\n",
			mapa->vvel.x, hmapa->vvel.y,
			mapa->vvel.z, hmapa->vvel.m);
		__builtin_printf("  vacca: x %f y %f z %f m %f\n",
			(&vacca)->x, (&vacca)->y, (&vacca)->z, (&vacca)->m);
		__builtin_printf("  vdist: x %f y %f z %f m %f\n",
			(&vdist)->x, (&vdist)->y, (&vdist)->z, (&vdist)->m);
		__builtin_printf("  vaxia: x %f y %f z %f m %f\n\n",
			mapa->vaxi.x, hmapa->vaxi.y,
			mapa->vaxi.z, hmapa->vaxi.m);
		/* todo:
		   Mass not assigned correctly. */
		//__builtin_printf("   kg a: %f\n", ma);
		__builtin_printf("    sza: %f\n", mapa->envelope.vsz.x);
		__builtin_printf("    mva: %f\n\n", mva);
		__builtin_printf("  isectn: %f\n", isectn);
		__builtin_printf("  centerdist: %f\n", centerdist);
		__builtin_printf("  scale: %f\n\n", scale);
		/* hmap other */
		__builtin_printf("index b: %i\n", mapb->index);
		__builtin_printf("  vposb: x %f y %f z %f m %f\n",
			mapb->vpos.x, mapb->vpos.y,
			mapb->vpos.z, mapb->vpos.m);
		__builtin_printf("  vvelb: x %f y %f z %f m %f\n",
			mapb->vvel.x, mapb->vvel.y,
			mapb->vvel.z, mapb->vvel.m);
		__builtin_printf("  vaccb: x %f y %f z %f m %f\n",
			(&vaccb)->x, (&vaccb)->y, (&vaccb)->z, (&vaccb)->m);
		__builtin_printf("  vaxib: x %f y %f z %f m %f\n\n",
			mapb->vaxi.x, mapb->vaxi.y,
			mapb->vaxi.z, mapb->vaxi.m);
		 //__builtin_printf("   kg b: %f\n", mb);
		__builtin_printf("    szb: %f\n", mapb->envelope.vsz.x);
		__builtin_printf("    mvb: %f\n\n", mvb);
		usleep(10000);
	}
#endif /* DIAG_INTERSECTION */

	return 0;
}

/* Recycle counta hmaps referenced in selectf_a.  For now just detach these. */
int
recycle(select_t *sel)
{
	int i;
	hmapf_t **map = sel->seta;

	for (i = 0; i < sel->counta; i++, map++) {
		__builtin_printf(" recycling: %x\n", (*map)->name);
		(*map)->attribs.sign |= VRT_MASK_DETACH;
	}

	return 0;
}

/* Make a copy of (for now) a single hmap referenced thru sel->seta into hmap
   memory referenced thru sel->setb.  Receiving hmap retains name/index,
   position, and kfactord. */
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
	b->attribs.sign = a->attribs.sign;
	b->attribs.mode = a->attribs.mode;
	b->attribs.balance_filter = a->attribs.balance_filter;
	b->attribs.kg = a->attribs.kg;
	b->attribs.kfactorm = a->attribs.kfactorm;
	b->envelope.geom = a->envelope.geom;
	b->envelope.vsz.x = a->envelope.vsz.x;
	b->envelope.vsz.y = a->envelope.vsz.y;
	b->envelope.vsz.z = a->envelope.vsz.z;
	b->envelope.vsz.m = a->envelope.vsz.m;
	b->draw.geom = a->draw.geom;

	if (a->vmap_total) {
		free(b->vmap);
		b->vmap = NULL;
		if ((b->vmap = (vf_t *) malloc(a->vmap_total * sizeof(vf_t))) == NULL) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: could not malloc vertice data "
				"copying hmap %i to %i\n",
				__FILE__, __LINE__, a->index, a->index);
			abort();
		}
		av = a->vmap;
		bv = b->vmap;
		for (i = 0; i < a->vmap_total; i++, av++, bv++) {
			bv->x = av->x;
			bv->y = av->y;
			bv->z = av->z;
			bv->m = av->m;
		}
		b->vmap_total = a->vmap_total;
	}

	if (a->dialog_len) {
		free(b->dialog);
		b->dialog = NULL;
	   if ((b->dialog = (int *) malloc(a->dialog_len * sizeof(int))) == NULL) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: could not malloc dialog data "
				"copying hmap %i to %i\n",
				__FILE__, __LINE__, a->index, b->index);
			abort();
		}
		ad = a->dialog;
		bd = b->dialog;
		for (i = 0; i < a->dialog_len; i++)
			*bd++ = *ad++;
		b->dialog_len = a->dialog_len;
	}

	return 0;
}

/* For c series hmaps, invert surface normals of (for now) a single hmap
   refrenced thru sel->seta by inverting the precedence of drawing for each
   vertice. */
int
surface_inv_hmapf(select_t *sel)
{
	int i;
	hmapf_t *map;
	vf_t *org, *cpy;

	map = *(sel->seta);
	org = map->vmap;
	if ((cpy = (vf_t *) malloc(map->vmap_total * sizeof(vf_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
		"Error: Could not malloc vertice data when inverting hmap %i\n",
		__FILE__, __LINE__, map->index);
		abort();
	}

	for (i = 0; i < map->vmap_total; i++) {
		cpy->x = org->x;
		cpy->y = org->y;
		cpy->z = org->z;
		(cpy++)->m = (org++)->m;
	}

	org = map->vmap;
	for (i = map->vmap_total; i > 0; i--) {
		org->x = (--cpy)->x;
		org->y = cpy->y;
		org->z = cpy->z;
		(org++)->m = cpy->m;
	}

	free(cpy);
	return 0;
}


/* hmap transformative functions affecting ip network or filesystem. */

/* Format and write hmaps referenced in selectf_a vs. sel, in format specified
   by options, named name(if applicable) _or_ available referenced by output.
   filename and output are exclusive, therefore where filename is NULL, output
   is a pointer to pointer, and where output is NULL filename is a string.  If
   filename is omitted, caller is expected to _free_data_referenced_by_output_
   after use.  If VRT_MASK_OPT_COMPOUNDED is set, multiple maps may be provided
   in selection.  notes: Once stablized, any file written will recieve .vrtater
   extension and be readable by current and future hmapunwrapf versions.  Masks
   VRT_MASK_OPT_NULL_SESSION_NAME, VRT_MASK_OPT_MINIMAL, VRT_MASK_OPT_UPDATE,
   VRT_MASK_OPT_COMPRESSED and VRT_MASK_OPT_ENCRYPTED are not working fully or
   at all yet when applied as options, however testing of session.c code may
   still proceed without these.  Return total of int type wrapped on success or
   a value less than 0 on any error. */
int
hmapwrapf(select_t *sel, btoggles_t options, char *filename, int **output)
{
	int fd, bufsz = 0;
	int i, j, enqueue = 1;
	int *pb, *pi, *dlg, *nxtmapsz, *outbuf = NULL;
	float *pf;
	hmapf_t **maps;
	vf_t *v;
	ssize_t nwritten = 0;
	int mapstruct = sizeof(hmapf_t) - sizeof(options_t *) - sizeof(vf_t *) - sizeof(int *) - (2 * sizeof(hmapf_t *)) - sizeof(drawlist_t *);

	/* Allocate outbuf of bufsz bytes for file write or ip network send. */
	maps = sel->seta;
	bufsz += 2 * sizeof(int); /* total and options */
	if (options & VRT_MASK_OPT_COMPOUNDED)
		bufsz += sel->counta * sizeof(int); /* nxtmapsz's */
	for (i = 0; i < sel->counta; i++, maps++)
		bufsz += mapstruct + ((*maps)->vmap_total * sizeof(vf_t)) + ((*maps)->dialog_len * sizeof(int));
	if ((outbuf = (int *) malloc(bufsz)) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for outbuf\n",
			__FILE__, __LINE__);
		abort();
	}

	/* If not a file write, caller will reference outbuf as output. */
	if (output)
		*output = outbuf;

	__builtin_printf("Bufsz is %i:%i\n", bufsz, bufsz / sizeof(int));

	maps = sel->seta;
	pb = outbuf; /* pb is start of file. */
	pi = pb + 1; /* start with space for .vrtater size */
	*pi++ = (int) options;

	__builtin_printf("total_hmaps is %i\n", sel->counta);

	for (i = 0; i < sel->counta; i++, maps++) {

		if (options & VRT_MASK_OPT_COMPOUNDED)
			nxtmapsz = pi++; /* space for map size */

		if (!(options & VRT_MASK_OPT_NULL_SESSION_NAME)) {
			*pi++ = (int) (*maps)->name;
			__builtin_printf("Wrapping map %x\n",
				(int) (*maps)->name);
		} else {

			__builtin_printf("hmap file recieves no session name\n");

		}

		if (!(options & VRT_MASK_OPT_MINIMAL))
			*pi++ = (*maps)->index;
		else {

			__builtin_printf("hmap file is written minimally\n");

		}

		pf = (float *) pi;
		*pf++ = (*maps)->vpos.x;
		*pf++ = (*maps)->vpos.y;
		*pf++ = (*maps)->vpos.z;

		if (!(options & VRT_MASK_OPT_MINIMAL))
			*pf++ = (*maps)->vpos.m;

		*pf++ = (*maps)->vvel.x;
		*pf++ = (*maps)->vvel.y;
		*pf++ = (*maps)->vvel.z;

		if (!(options & VRT_MASK_OPT_MINIMAL))
			*pf++ = (*maps)->vvel.m;

		*pf++ = (*maps)->vaxi.x;
		*pf++ = (*maps)->vaxi.y;
		*pf++ = (*maps)->vaxi.z;

		if (!(options & VRT_MASK_OPT_MINIMAL))
			*pf++ = (*maps)->vaxi.m;

		*pf++ = (*maps)->vrel.x;
		*pf++ = (*maps)->vrel.y;
		*pf++ = (*maps)->vrel.z;

		if (!(options & VRT_MASK_OPT_MINIMAL))
			*pf++ = (*maps)->vrel.m;

		*pf++ = (*maps)->vpre.x;
		*pf++ = (*maps)->vpre.y;
		*pf++ = (*maps)->vpre.z;

		if (!(options & VRT_MASK_OPT_MINIMAL))
			*pf++ = (*maps)->vpre.m;

		*pf++ = (*maps)->ang_spd;
		*pf++ = (*maps)->ang_dpl;

		if (options & VRT_MASK_OPT_UPDATE) {

			__builtin_printf("hmap file is an update\n");

			break;
		}
		pi = (int *) pf;
		*pi++ = (*maps)->attribs.sign;
		*pi++ = (*maps)->attribs.mode;
		*pi++ = (*maps)->attribs.balance_filter;
		pf = (float *) pi;
		*pf++ = (*maps)->attribs.kg;
		pi = (int *) pf;
		*pi++ = (*maps)->attribs.kfactorm;
		*pi++ = (*maps)->attribs.kfactord;

		*pi++ = (*maps)->envelope.geom;
		pf = (float *) pi;
		*pf++ = (*maps)->envelope.vsz.x;
		*pf++ = (*maps)->envelope.vsz.y;
		*pf++ = (*maps)->envelope.vsz.z;

		if (!(options & VRT_MASK_OPT_MINIMAL))
			*pf++ = (*maps)->envelope.vsz.m;

		pi = (int *) pf;
		*pi++ = (*maps)->draw.geom;

		*pi++ = (*maps)->vmap_total;
		if ((*maps)->vmap_total) {
			pf = (float *) pi;
			v = (*maps)->vmap;
			for (j = 0; j < (*maps)->vmap_total; j++) {
				*pf++ = v->x;
				*pf++ = v->y;
				*pf++ = v->z;
				*pf++ = v->m;
				v++;
			}
			pi = (int *) pf;
		}
		__builtin_printf("vmap total %i\n", (*maps)->vmap_total);

		*pi++ = (*maps)->dialog_len;
		if ((*maps)->dialog_len) {
			dlg = (*maps)->dialog;
			j = 0;
			for (j = 0; j < (*maps)->dialog_len; j++)
				*pi++ = *dlg++;
		}
		__builtin_printf("dialog_len %i\n", (*maps)->dialog_len);

		if (options & VRT_MASK_OPT_COMPOUNDED) {
			/* Include size preceeding each map's data. */
			*nxtmapsz = (int) (pi - nxtmapsz);

			__builtin_printf("hmapf size: %i\n", *nxtmapsz);

		} else
			__builtin_printf("hmapf size: %i\n",
				(int) (pi - (pb + 2)));
	}

	if (options & VRT_MASK_OPT_ENCRYPTED) {
		__builtin_printf("hmap file is encrypted\n");
	}

	if (options & VRT_MASK_OPT_COMPRESSED) {
		__builtin_printf("hmap file is compressed\n");
	}

	*pb = (int) (pi - pb); /* write length at pb */
	__builtin_printf("total count including size and options %i\n", *pb);
	if (!(abs(*pb) <= 0x7fffffff)) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: hmapwrapf got cold feet wrapping, as the "
			"count of int type elements exceeded pre-alpha "
			"version limit\n",
			__FILE__, __LINE__);
		return -1;
	}

	if (filename) {
		if ((fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 0644)) == -1) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: hmapwrapf couldn't open %s when "
				"attempting to write\n",
				__FILE__, __LINE__, filename);
			return -1;
		}
		if((*pb * sizeof(int)) == bufsz) { /* for now */
			while (enqueue) {
				nwritten = write(fd, (char *) outbuf, bufsz);
				enqueue = 0;
			}
			__builtin_printf("%s: %i bytes written\n",
				filename, (int) nwritten);
		} else {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: hmapwrapf found a mismatch between "
				"calculated hmap size and it's actual size.  "
				"0 bytes were written\n",
				__FILE__, __LINE__);
			close(fd);
			return -1;
		}
		close(fd);
	}

	return *pb;
}

/* Read .vrtater file data filename _or_ input, unwrapping and referencing it's
   hmap(s) into selectf_b vs. sel (and soon to come writing or overwriting them
   into vohspace) based on options within data.  Set VRT_OPT_MASK_BUFFER for
   each hmap implied.  If filename is not given, input references data.  In
   this case, count of data should never exceed what ssize_t can handle nor
   0x7fffffff, for now.  When input is not NULL, caller is expected to free
   input after use.  If VRT_MASK_OPT_COMPOUNDED is set there may be multiple
   maps.  If session is given, an index unique for node-orgin and node-orgin
   originating maps is applied and map(s) implied are then part of session.
   notes: Once resizing of vohspace is fully supported, a check for enough
   space would be done in advance of each new map placement with a resize call
   conditionally.  Return number of int's read from the .vrtater file or data,
   or a value less than zero for errors.  note: For now calling this with
   session name of selected map may cause breakage, see note in following. */
int
hmapunwrapf(select_t *sel, session_t *session, char *filename, int *input)
{
	int fd, i, sum, size, tl, graph, space, ctrl, null, *pi, *dialog = NULL;
	float *pf;
	char *filebuf = NULL;
	ssize_t nread;
	off_t filesz;
	hmapf_t **map;
	vf_t *rebuild;
	session_t current;
	btoggles_t options;

	if (filename) {
		/* Read from filename. */
		if ((fd = open(filename, O_RDONLY)) == -1) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: hmapunwrapf couldn't open %s\n",
				__FILE__, __LINE__, filename);
			return -1;
		}
		filesz = lseek(fd, (off_t) 0, SEEK_END);
		lseek(fd, (off_t) 0, SEEK_SET);
		__builtin_printf("Reading %i bytes from %s\n",
			(int) filesz, filename);

		if ((filebuf = (char *) malloc((int) filesz)) == NULL) {
			__builtin_fprintf(stderr,  "vrtater:%s:%d: "
				"Error: Could not malloc for "
				"hmap file input buffer\n",
				__FILE__, __LINE__);
			abort();
		}
		if ((nread = read(fd, filebuf, filesz)) == -1) {
			__builtin_fprintf(stderr,  "vrtater:%s:%d: "
				"Error: hmapwrapf couldn't read %s\n",
				__FILE__, __LINE__, filename);
			abort(); /* for now */
		}

		pi = (int *) filebuf;

	} else
		pi = input;

	/* notes: Adding integrity checking.  Reading from .vrtater data, code
	   in session.c would check vs. a maximum threshold for data totals
	   received, also providing the actual value that should then be
	   converted to ssize_t and placed in nread.  size should match this
	   value.  Calculating size while unwrapping should also produce value.
	   Perhaps a magic number or a key in the options bits could be used
	   for further integrity.  Any dialog data is checked for within
	   printable range when applied. */
	size = *pi;
	__builtin_printf("Unwrapping %i int size elements\n", (int) *pi++);

	options = *pi++;
	print_toggles("options ", (btoggles_t *) &options);

	/* Count b maps are unwrapped, then referenced thru set b. */
	map = sel->setb;

	/* Place all maps in the .vrtater file into vohspace.  sum begins at
	   2, just beyond .vrtater size and options */
	for (tl = 0, sum = 2; sum < size; map++, tl++) {
		/* Determine size for this map. */
		if (options & VRT_MASK_OPT_COMPOUNDED) {
			__builtin_printf("hmapf size: %i\n", (int) *pi);
			sum += *pi++;
		} else
			sum = size;

		/* If data sessionless, hmapf requires session or break. If
		   not, use session if provided else data provided session for
		   hmapf.  Use mapref to mask out hmapf call where this is an
		   update. */
		if (options & VRT_MASK_OPT_NULL_SESSION_NAME) {

			if (session) {
				current = (session_t) *session;
				if (!(*map = hmapf(&current)))
					break;
					sel->countb += 1;
				__builtin_printf("Argued session %x\n",
					(int) current);
			} else
				break;
		} else {

			if (session) {
				current = *session;
				pi++;
				__builtin_printf("Using argued session %x\n",
					(int) current);
			} else {
				current = (session_t) *pi++;
				__builtin_printf("Last valid session %x\n",
					(int) current);
			}
			if ((*map = mapref(&current)) != NULL) {
				/* note: map is incremented above every pass.
				   If countb gets an increment, the map will
				   not be present so for now do not argue
				   session name of selected map.  When
				   additions here are made, (soon), if passed
				   session name of selected map, update.  To
				   update for now possibly call hmapf here,
				   after planned addition swap_hmaps transform
				   with a full option is written, then recycle
				   the original map.  also: mapref still needs
				   revision. */
				__builtin_printf("Updating one map %x\n",
					(int) current);
				sel->countb += 1;
			} else {

				/* Build new map. */
				if (!(*map = hmapf(&current)))
					break;
				sel->countb += 1;
			}
		}
		__builtin_printf("Added/updating map %x index %i\n",
			(int) (*map)->name, (*map)->index);

		if (!(options & VRT_MASK_OPT_MINIMAL))
			pi++; /* skip index part of data */

		pf = (float *) pi;

		rebuild = &((*map)->vpos);
		rebuild->x = (float) *pf++;
		rebuild->y = (float) *pf++;
		rebuild->z = (float) *pf++;

		if (options & VRT_MASK_OPT_MINIMAL)
			form_mag_vf(rebuild);
		else
			rebuild->m = (float) *pf++;

		__builtin_printf("pos: %f %f %f %f\n",
			rebuild->x, rebuild->y, rebuild->z, rebuild->m);

		rebuild = &((*map)->vvel);
		rebuild->x = (float) *pf++;
		rebuild->y = (float) *pf++;
		rebuild->z = (float) *pf++;

		if (options & VRT_MASK_OPT_MINIMAL)
			form_mag_vf(rebuild);
		else
			rebuild->m = (float) *pf++;

		__builtin_printf("vel: %f %f %f %f\n",
			rebuild->x, rebuild->y, rebuild->z, rebuild->m);

		rebuild = &((*map)->vaxi);
		rebuild->x = (float) *pf++;
		rebuild->y = (float) *pf++;
		rebuild->z = (float) *pf++;

		if (options & VRT_MASK_OPT_MINIMAL)
			form_mag_vf(rebuild);
		else
			rebuild->m = (float) *pf++;

		__builtin_printf("axi: %f %f %f %f\n",
			rebuild->x, rebuild->y, rebuild->z, rebuild->m);

		rebuild = &((*map)->vrel);
		rebuild->x = (float) *pf++;
		rebuild->y = (float) *pf++;
		rebuild->z = (float) *pf++;

		if (options & VRT_MASK_OPT_MINIMAL)
			form_mag_vf(rebuild);
		else
			rebuild->m = (float) *pf++;

		__builtin_printf("rel: %f %f %f %f\n",
			rebuild->x, rebuild->y, rebuild->z, rebuild->m);

		rebuild = &((*map)->vpre);
		rebuild->x = (float) *pf++;
		rebuild->y = (float) *pf++;
		rebuild->z = (float) *pf++;

		if (options & VRT_MASK_OPT_MINIMAL)
			form_mag_vf(rebuild);
		else
			rebuild->m = (float) *pf++;

		__builtin_printf("pre: %f %f %f %f\n",
			rebuild->x, rebuild->y, rebuild->z, rebuild->m);

		__builtin_printf("ang_spd: %f\n", (float) *pf);
		(*map)->ang_spd = *pf++;
		__builtin_printf("ang_dpl: %f\n", (float) *pf);
		(*map)->ang_dpl = *pf++;

		if (options & VRT_MASK_OPT_UPDATE)
			break; /* for now includes positional attribs only */

		pi = (int *) pf;

		__builtin_printf("attribs sign: 0x%x\n", (int) *pi);
		(*map)->attribs.sign = *pi++ | VRT_MASK_BUFFER;
		__builtin_printf("attribs mode: 0x%x\n", (int) *pi);
		(*map)->attribs.mode = *pi++;
		__builtin_printf("balance filter: 0x%x\n", (int) *pi);
		(*map)->attribs.balance_filter = *pi++;

		pf = (float *) pi;

		__builtin_printf("kg: %f\n", (float) *pf);
		(*map)->attribs.kg = *pf++;

		pi = (int *) pf;

		__builtin_printf("kfactorm: %i\n", (int) *pi);
		(*map)->attribs.kfactorm = *pi++;
		__builtin_printf("kfactord: %i\n", (int) *pi);
		(*map)->attribs.kfactord = *pi++;

		__builtin_printf("bound geom: %i\n", (int) *pi);
		(*map)->envelope.geom = *pi++;

		pf = (float *) pi;

		rebuild = &((*map)->envelope.vsz);
		rebuild->x = (float) *pf++;
		rebuild->y = (float) *pf++;
		rebuild->z = (float) *pf++;

		if (options & VRT_MASK_OPT_MINIMAL)
			form_mag_vf(rebuild);
		else
			rebuild->m = (float) *pf++;

		__builtin_printf("bound size: %f %f %f %f\n",
			rebuild->x, rebuild->y, rebuild->z, rebuild->m);

		pi = (int *) pf;

		__builtin_printf("draw geometry: %i\n", (int) *pi);
		(*map)->draw.geom = *pi++;

		__builtin_printf("vmap_total: %i\n", (int) *pi);
		if (((*map)->vmap_total = (int) *pi++)) {

			if (((*map)->vmap = (vf_t *) malloc((*map)->vmap_total * sizeof(vf_t))) == NULL) {
				__builtin_fprintf(stderr,  "vrtater:%s:%d: "
					"Error: Could not malloc for vmap\n",
					__FILE__, __LINE__);
				abort();
			}

			pf = (float *) pi;

			rebuild = (*map)->vmap;
			for (i = 0; i < (*map)->vmap_total; i++, rebuild++) {
				__builtin_printf("vf %i: %f",
					i + 1, (float) *pf);
				rebuild->x = (float) *pf++;
				__builtin_printf(" %f", (float) *pf);
				rebuild->y = (float) *pf++;
				__builtin_printf(" %f", (float) *pf);
				rebuild->z = (float) *pf++;
				if (options & VRT_MASK_OPT_MINIMAL) {
					form_mag_vf(rebuild);
					__builtin_printf(" auto\n");
				} else {
					__builtin_printf(" %f\n", (float) *pf);
					rebuild->m = (float) *pf++;
				}
			}

			pi = (int *) pf;
		}

		__builtin_printf("dialog_len: %i\n", (int) *pi);
		if (((*map)->dialog_len = (int) *pi++)) {

			if (((*map)->dialog = (int *) malloc(((*map)->dialog_len + 1) * sizeof(int))) == NULL) {
				__builtin_fprintf(stderr,  "vrtater:%s:%d: "
					"Error: Could not malloc for dialog\n",
					__FILE__, __LINE__);
				abort();
			}

			graph = 0;
			space = 0;
			ctrl = 0;
			null = 0;

			dialog = (*map)->dialog;
			for (i = 0; i < (*map)->dialog_len;) {
				/* print !last char 1 to 7 of rows of 8 chars */
				for (;!((i%8)==7) && ((i+1)%(*map)->dialog_len); i++) {
					if (iscntrl((char) *pi)) { ctrl += 1; }
					if (isgraph((char) *pi)) { graph += 1; }
					if (isspace((char) *pi)) { space += 1; }
					if (!(*pi)) { null += 1; }
					__builtin_printf("%10.8x", (int) *pi);
					*dialog++ = *pi++;
				}
				if ((i+1)%(*map)->dialog_len) {
					/* print !last char 8 in row of 8 */
					if (iscntrl((char) *pi)) { ctrl += 1; }
					if (isgraph((char) *pi)) { graph += 1; }
					if (isspace((char) *pi)) { space += 1; }
					if (!(*pi)) { null += 1; }
					__builtin_printf("%10.8x\n", (int) *pi);
					*dialog++ = *pi++;
					i++;
				} else { /* last char */
					if (iscntrl((char) *pi)) { ctrl += 1; }
					if (isgraph((char) *pi)) { graph += 1; }
					if (isspace((char) *pi)) { space += 1; }
					if (!(*pi)) { null += 1; }
					__builtin_printf("%10.8x\n", (int) *pi);
					*dialog++ = *pi++;
					i += 2;
				}
			}
			__builtin_printf("dialog gscn: %i %i %i %i\n",
				graph, space, ctrl, null);
		}
		__builtin_printf("type int elements total %i\n", sum);
	}

	if (filename)
		close(fd);
	if (filebuf)
		free(filebuf);

	__builtin_printf("Total maps %i\n", sel->countb);
	if (abs(size) <= 0x7fffffff)
		return size;
	else {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: hmapunwrapf got cold feet unwrapping  %s, as "
			"it's size exceeded pre-alpha version limit.\n",
			__FILE__, __LINE__, filename);
		return -1;
	}
}

/* hmap dialog referenced recieves allocation of len + 1 int's.  This maintains
   the NULL char at the end of any sum of strlen chars.  Where dialog has been
   allocated it will always contain at least the NULL string untill free'd. */
int
alloc_dialog(hmapf_t *map, int len)
{
	map->dialog = NULL;
	if ((map->dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}
	map->dialog_len = 0;
	*(map->dialog) = '\0';
	return 0;
}

/* Write or append string s if given to string referenced by int pointer dialog
   in hmap referenced by map.  When supported, insert or delete count at offset
   into hmap dialog while where s is not NULL count is ignored, and where s is
   NULL, count chars are deleted.  Where s is given, count is ignored and
   assumed to be sizeof(s).  offset decodes to strlen(s) as an ordinal index
   into s where offset is then part of the selection within string to be deleted
   or the start of chars to be shifted right in the string.  notes: For now
   append s to end of hmap dialog.  Insert feature is scheduled to be ready for
   dialog.c call for authors. */
int
add_dialog(hmapf_t *map, char *s, int offset, int count)
{
	int i, addlen, orglen, newlen, *dlg, *cpy, *swap = NULL;
	char *c;

	if (map->dialog) {
		/* Allocate swap memory to hold previous dialog. */
		if ((swap = (int *) malloc(map->dialog_len * sizeof(int))) == NULL) {
			__builtin_fprintf(stderr,  "vrtater:%s:%d: "
				"Error: Could not malloc for add_dialog()\n",
				__FILE__, __LINE__);
			abort();
		}

		orglen = map->dialog_len; /* needed after alloc_dialog */
		addlen = strlen(s);
		newlen = orglen + addlen;

		/* Put original hmap dialog in swap. */
		dlg = map->dialog;
		cpy = swap;
		for (i = 0; i < orglen; i++)
			*cpy++ = *dlg++;

		/* (re-)allocate memory for dialog with append. */
		free(map->dialog);
		alloc_dialog(map, newlen);

		/* Write original string from swap. */
		dlg = map->dialog;
		cpy = swap;
		for (i = 0; i < orglen; i++)
			*dlg++ = *cpy++;
		free(swap);
	} else {
		newlen = addlen = strlen(s);
		alloc_dialog(map, addlen);
		dlg = map->dialog;
	}

	/* Write new string. */
	c = s;
	for (i = 0; i < addlen; i++)
		*dlg++ = *c++;
	*dlg = '\0';

	/* Tend nicely. */
	map->dialog_len = newlen;

	return 0;
}

/* hmap dialog referenced by map becomes string s. */
int
write_dialog(hmapf_t *map, char *s)
{
	int i, len, *d;
	char *p;

	/* Free previous. */
	free(map->dialog);
	map->dialog = NULL;

	/* Allocate. */
	len = strlen(s);
	if ((map->dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Write. */
	p = s;
	d = map->dialog;
	for (i = 0; i < len; i++)
		*d++ = *p++;
	*d = '\0';

	map->dialog_len = len;

	return 0;
}
