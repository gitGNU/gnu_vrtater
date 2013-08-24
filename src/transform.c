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
#include "progscope.h"

float vrt_render_cyc; /* external */


/* Transform hmaps.
   Given two buffers referencing one or two set's of hmaps, affect those hmaps
   with given transform function that operate's independantly or as a dependant
   set, in some way altering at least one hmap. */

/* Join two hmaps in selectf_a and selectf_b respectively, with one representing
   a fulcrum and one a lever.  This will require implementation of
   session_filter prehand.  note: This function was to be called hapticNormill,
   as vrtater has much of it's inspiration in a haptic feedback project that
   took place to a significant extent at a certain most cozy mill, that project
   inspired by another moreso well known haptic feedback project related. */
int
join_hmaps(select_t *sel)
{
	return 0;
}

/* Given reference to 2 hmaps in sel a and b respectively, transform path
   attributes of these apon hmap intersection.  For now, simply uses spherical
   bounding volumes of radius envelope.vsz and mass of attribs.kg per any hmaps.
   note: Representation of mass is proportional to volume. */
int
intersection(select_t *sel)
{
	/* scrawl:
	   still in progress
	   toggles_t toggles;
	   toggles = sel->specbits;
	   - determine bound geom
	*/

	hmapf_t *mapa, *mapb;
	vf_t *vvela, *vvelb;
	vf_t vacca = {0, 0, 0, 0};
	vf_t vaccb = {0, 0, 0, 0};
	vf_t vdist = {0, 0, 0, 0};
	float touch, inter, ma, mb, scale;
	float mva, mvb; /* for now */

	mapa = *(sel->seta);
	mapb = *(sel->setb);

	scale = .00005;

	dif_vf(&(mapa->vpos), &(mapb->vpos), &vdist);
	touch = mapa->envelope.vsz.x + mapb->envelope.vsz.x;

	if (mapb->attribs.kg && (&vdist)->m < touch) {

		ma = mapa->attribs.kg;
		mb = mapb->attribs.kg;
		inter = touch - (&vdist)->m; /* -intersection */

		/* Tend to hmap a. */
		vvela = &(mapa->vvel);
		/* m*v for now */
		mva = ma * vvela->m;
		/* Here calculate angle of reflection. */

		/* Calculate and set acc for a. */
		if (mva)
			tele_magz_vf(&vdist, &vacca, scale * inter * mb / mva);
		sum_vf(&vacca, vvela, vvela);

		/* Tend to hmap b. */
		vvelb = &(mapb->vvel);
		mvb = mb * vvelb->m;
		if (mvb)
			tele_magz_vf(&vdist, &vaccb, scale * inter * ma / mvb);
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
		__builtin_printf("  inter: %f\n", inter);
		__builtin_printf("  touch: %f\n", touch);
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

/* Enable hmaps referenced in selectf_a to be affected by certain contextual
   transforms with hmap referenced in selectf_b.  This will require
   implementation of session_filter prehand.  Other transforms to follow would
   be leave_group, and group_groups. */
int
group_hmaps(select_t *sel)
{
	return 0;
}

/* Extend given hmaps by merging them together. */
int
extend_hmaps(select_t *sel)
{
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
   session_filter, position, and kfactord. */
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

/* Format and write hmaps in selectf_a vs. sel, in format specified by options,
   named name(if applicable) or available referenced by output.  filename and
   output are exclusive, therefore filename must be NULL where reference output
   is to be used.  If filename is omitted, caller is expected to free data
   referenced by output after use.  Any file written will recieve .vrtater
   extension and be readable by hmapunwrapf now and in future versions.
   If VRT_MASK_OPT_COMPOUNDED is set there may be multiple maps. */
int
hmapwrapf(select_t *sel, btoggles_t options, char *filename, int **output)
{
	int fd, *pb, *pi, *outbuf, *dlg, *nxtmapsz, i, j, bufsz = 0;
	float *pf;
	hmapf_t **maps;
	vf_t *v;
	ssize_t nwritten = 0;

	/* Allocate outbuf of bufsz bytes for file write or ip network send. */
	maps = sel->seta;
	for (i = 0; i < sel->counta; i++, maps++)
		bufsz += sizeof(hmapf_t) + ((*maps)->vmap_total * sizeof(vf_t)) + (((*maps)->dialog_len + 1) * sizeof(int));
	if ((outbuf = (int *) malloc(bufsz)) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for outbuf\n",
			__FILE__, __LINE__);
		abort();
	}

	/* If not a file write, caller will reference outbuf as output. */
	if (output)
		*output = outbuf;

	__builtin_printf("Bufsz is %i bytes\n", bufsz);

	maps = sel->seta;
	pb = outbuf; /* pb is start of file. */
	pi = pb + 1; /* a space for .vrtater size vs. int */
	*pi++ = (int) options;

	__builtin_printf("total_hmaps is %i\n", sel->counta);

	for (i = 0; i < sel->counta; i++, maps++) {

		if (options & VRT_MASK_OPT_COMPOUNDED) {
			nxtmapsz = pi++; /* a space per map size vs. int */

			__builtin_printf("hmap is compounded\n");

		}

		if (!(options & VRT_MASK_OPT_NULL_SESSION_NAME)) {
			*pi++ = (int) ((*maps)->name >> 16);
			*pi++ = (int) ((*maps)->name & 0xffff);
			__builtin_printf("Wrapping map %x\n", (int) (*maps)->name);
		} else {
			__builtin_printf("hmap file recieves no session name\n");
		}

		if (!(options & VRT_MASK_OPT_MINIMAL)) {
			*pi++ = (*maps)->index;
		} else {

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
		*pi++ = (*maps)->attribs.session_filter;
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
			/* Write next maps size at beginning of map. */
			*nxtmapsz = abs((int) pi - (int) nxtmapsz) / sizeof(int);

			__builtin_printf("hmapf size: %i\n", *nxtmapsz);

		}
	}
	*pb = abs((int) pi - (int) pb) / sizeof(int); /* write length at pb */

	__builtin_printf("total count is %i\n", *pb);

	if (options & VRT_MASK_OPT_ENCRYPTED) {
		__builtin_printf("hmap file is encrypted\n");
	}

	if (options & VRT_MASK_OPT_COMPRESSED) {
		__builtin_printf("hmap file is compressed\n");
	}

	if (filename) {

		if ((fd = open(filename, O_RDWR | O_CREAT | O_EXCL, 0644)) == -1) {
			__builtin_fprintf(stderr, "Error: Couldn't create %s\n",
				filename);
			return(-1);
		}

		if((*pb * 4) == bufsz) { /* for now */
			nwritten = write(fd, (char *) outbuf, bufsz);
			__builtin_printf("%s: %i bytes written\n",
				filename, (int) nwritten);
		} else
			return -1;
	}

	close(fd);
	return 0;
}

/* Read .vrtater file data filename or input, unwrapping and referencing it's
   hmap(s) into selectf_b vs. sel and writing or overwriting them into vohspace
   based on options within data.  Set VRT_OPT_MASK_BUFFER for each hmap implied.
   If filename is not given input references data.  When input is not NULL,
   caller is expected to free input after use.  If VRT_MASK_OPT_COMPOUNDED is
   set there may be multiple maps.  If session is given an index unique for
   node-orgin and node-orgin originating maps is applied and map(s) implied
   are then part of session.  notes: Once resizing of vohspace is fully
   supported, a check for enough space would be done in advance of each new map
   placement with a resize call conditionally. */
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
			__builtin_printf("hmapunwrap: Couldn't open %s\n", filename);
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
		nread = read(fd, filebuf, filesz);

		pi = (int *) filebuf;

	} else {
		__builtin_printf("hmapunwrapf: Reading from session buffer\n");
		pi = input;
	}

	size = *pi;
	__builtin_printf("Unwrapping %i sizeof(int) sized elements\n", (int) *pi++);

	options = *pi++;
	print_toggles("options ", (btoggles_t *) &options);

	map = sel->setb;

	/* Place all maps in the .vrtater file into vohspace. */
	for (tl = 0, sum = 0; size > sum; map++, tl++) {

		/* Determine size for this map. */
		if (options & VRT_MASK_OPT_COMPOUNDED) {
			__builtin_printf("hmapf size: %i\n", (int) *pi + 1);
			sum += (*pi++ + 1);
		} else
			sum = size;

__builtin_printf("Byte total this pass %i\n", sum);


		/* If data sessionless, hmapf requires session or break. If not,
		   use session if provided else data provided session for hmapf.
		   Use mapref to mask out hmapf call where this is an update. */
		if (options & VRT_MASK_OPT_NULL_SESSION_NAME) {

			if (session) {
				current = (session_t) *session;
				if (!(*map = hmapf(&current)))
					break;
					sel->countb += 1;
				__builtin_printf("Argued session %x\n", (int) current);
			} else
				break;
		} else {

			if (session) {
				current = *session;
				pi++;
				pi++;
				__builtin_printf("Using argued session %x\n", (int) current);
			} else {
				current = (session_t) (*pi++ << 16);
				current |= (session_t) *pi++;
				__builtin_printf("Last valid session %x\n", (int) current);
			}
			if ((*map = mapref(&current)) != NULL) {

				/* Update one map with name current. */
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
		__builtin_printf("Added/updated map %x index %i\n",
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
		__builtin_printf("session filter: 0x%x\n", (int) *pi);
		(*map)->attribs.session_filter = *pi++;
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
				__builtin_printf("vf %i: %f", i + 1, (float) *pf);
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
			__builtin_printf("dialog gscn: %i %i %i %i\n", graph, space, ctrl, null);
		}
	}

	if (filename)
		close(fd);
	if (filebuf)
		free(filebuf);

	__builtin_printf("Total maps %i\n", sel->countb);
	return 0;
}


/* hmap transformative functions affecting allocation. */

/* hmap dialog referenced  selectf_a recieves allocation of len + 1 int's. */
int
alloc_dialog(select_t *sel, int len)
{
	(*sel->seta)->dialog = NULL;
	if (((*sel->seta)->dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}
	(*sel->seta)->dialog_len = 0;
	*(*sel->seta)->dialog = '\0';
	return 0;
}

/* Write or append s to dialog(for now at end of current dialog) for any given
   hmap thru sel->seta.  When supported will also insert or delete count at
   offset to s, where sign of count will determine insert or delete and sense
   of count will be after offset. */
int
add_dialog(select_t *sel, char *s, int count, int offset)
{
	int i, addlen, orglen, newlen, *pti, *pti2, *swap = NULL;
	char *ptc;

	if ((*sel->seta)->dialog) {
		/* Allocate swap memory to hold previous dialog. */
		if ((swap = (int *) malloc((*sel->seta)->dialog_len * sizeof(int))) == NULL) {
			__builtin_fprintf(stderr,  "vrtater:%s:%d: "
				"Error: Could not malloc for add_dialog()\n",
				__FILE__, __LINE__);
			abort();
		}

		orglen = (*sel->seta)->dialog_len; /* calc before freeing */
		addlen = strlen(s);
		newlen = orglen + addlen;

		/* Put original hmap dialog in swap. */
		pti = (*sel->seta)->dialog;
		pti2 = swap;
		for (i = 0; i < orglen; i++)
			*pti2++ = *pti++;

		/* (re-)allocate memory for dialog with append. */
		free((*sel->seta)->dialog);
		alloc_dialog(sel, newlen);

		/* Write original string from swap. */
		pti = (*sel->seta)->dialog;
		pti2 = swap;
		for (i = 0; i < orglen; i++)
			*pti++ = *pti2++;
		free(swap);
	} else {
		newlen = addlen = strlen(s);
		alloc_dialog(sel, addlen);
		pti = (*sel->seta)->dialog;
	}

	/* Write new string. */
	ptc = s;
	for (i = 0; i < addlen; i++)
		*pti++ = *ptc++;
	*pti = '\0';

	/* Tend nicely. */
	(*sel->seta)->dialog_len = newlen;

	return 0;
}

/* hmap dialog referenced thru sel->seta becomes string s. */
int
write_dialog(select_t *sel, char *s)
{
	int i, len, *d;
	char *p;

	/* Free previous. */
	free((*sel->seta)->dialog);
	(*sel->seta)->dialog = NULL;

	/* Allocate. */
	len = strlen(s);
	if (((*sel->seta)->dialog = (int *) malloc((len + 1) * sizeof(int))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for given dialog\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Write. */
	p = s;
	d = (*sel->seta)->dialog;
	for (i = 0; i < len; i++)
		*d++ = *p++;
	*d = '\0';

	(*sel->seta)->dialog_len = len;

	return 0;
}
