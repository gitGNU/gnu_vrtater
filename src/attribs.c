/* attribs.c: Hold and maintain hmaps vs. node feedback representing vohspace.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "attribs.h"
#include "progscope.h"
#include "transform.h"

#ifdef VRT_RENDERER_GL
#include "rendergl.h"
#endif /* VRT_RENDER_GL */

/* For now. */
#define SMALLRANDOM 0.000000100000000
#define TINYRANDOM 0.000000001000000

hmapf_t *near_hmaps;
hmapf_t *perif_hmaps;
hmapf_t *far_hmaps;
hmapf_t **lr_n;
hmapf_t **rl_n;
hmapf_t **lr_p;
hmapf_t **rl_p;
hmapf_t **lr_f;
hmapf_t **rl_f;
hmapf_t **b_n;
hmapf_t **e_n;
hmapf_t **b_p;
hmapf_t **e_p;
hmapf_t **b_f;
hmapf_t **e_f;
int dlr_n, dlr_p, dlr_f; /* direction */
unsigned int passes;

/* Renderer. */
float vrt_render_cyc;
int fov0_name;

/* External. */
unsigned int vrt_hmaps_max;
hmapf_t *selectf_a;
hmapf_t *selectf_b;

void flow_over(btoggles_t *balance_criteria);
float estimate_radiusf(hmapf_t *);
void wanderf(hmapf_t *hmap, float e, float m, float r);

/* Set up hmap and lod envelope allocations.  Set map attribs for unattached. */
void
init_vohspace(void)
{
	int i;
	hmapf_t *p, **near;

	attached_hmaps = 0;
	if ((vohspace = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for vohspace\n",
			__FILE__, __LINE__);
		abort();
	}
	if ((near_hmaps = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for near_hmaps\n",
			__FILE__, __LINE__);
		abort();
	}
	if ((perif_hmaps = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for perif_hmaps\n",
			__FILE__, __LINE__);
		abort();
	}
	if ((far_hmaps = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for far_hmaps\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Allocation delimiters. */
	b_n = (hmapf_t **) &near_hmaps[0];
	e_n = (hmapf_t **) &near_hmaps[vrt_hmaps_max - 1];
	b_p = (hmapf_t **) &perif_hmaps[0];
	e_p = (hmapf_t **) &perif_hmaps[vrt_hmaps_max - 1];
	b_f = (hmapf_t **) &far_hmaps[0];
	e_f = (hmapf_t **) &far_hmaps[vrt_hmaps_max - 1];

	p = &vohspace[0];
	near = (hmapf_t **) &near_hmaps[0];

	/* Point out of bounds when an envelope buffer is empty. */
	lr_n = (hmapf_t **) &near_hmaps[0];
	lr_n--;
	rl_n = (hmapf_t **) &near_hmaps[vrt_hmaps_max - 1];
	rl_n++;
	lr_p = (hmapf_t **) &perif_hmaps[0];
	lr_p--;
	rl_p = (hmapf_t **) &perif_hmaps[vrt_hmaps_max - 1];
	rl_p++;
	lr_f = (hmapf_t **) &far_hmaps[0];
	lr_f--;
	rl_f = (hmapf_t **) &far_hmaps[vrt_hmaps_max - 1];
	rl_f++;

	passes = -1;
	dlr_n = 1;
	dlr_p = 1;
	dlr_f = 1;

	/* Put vrt_hmaps_max hmap pointers in near buf. */
	for (i = 0; i < vrt_hmaps_max; i++, ++lr_n, near++)
		*near = p++;

	/* Set all maps to unattached default. */
	p = vohspace;
	for (i = 0; i < vrt_hmaps_max; i++, p++) {
		p->name = 0;
		p->vpos.x = 0;
		p->vpos.y = 0;
		p->vpos.z = 0;
		p->vpos.m = 0x7f7fffff; /* max float */
		p->vvel.x = 0;
		p->vvel.y = 0;
		p->vvel.z = 0;
		p->vvel.m = 0;
		p->vaxi.x = 0;
		p->vaxi.y = 0;
		p->vaxi.z = 0;
		p->vaxi.m = 0;
		p->vrel.x = 0;
		p->vrel.y = 0;
		p->vrel.z = 0;
		p->vrel.m = 0;
		p->vpre.x = 0;
		p->vpre.y = 0;
		p->vpre.z = 0;
		p->vpre.m = 0;
		p->ang_spd = 0; /* radians/second, if timer  */
		p->ang_dpl = 0; /* radians */
		p->attribs.sign = 0;
		p->attribs.mode = VRT_MASK_FLOW_OVER;
		p->attribs.session_filter = 0;
		p->attribs.balance_filter = 0;
		p->attribs.kg = 0;
		p->attribs.kfactorm = 1;
		p->attribs.kfactord = 1;
		p->envelope.geom = VRT_BOUND_NONE;
		p->envelope.vsz.x = 0;
		p->envelope.vsz.z = 0;
		p->envelope.vsz.y = 0;
		p->envelope.vsz.m = 0;
		p->draw.geom = VRT_DRAWGEOM_NONE;
		p->vmap = NULL;
		p->vmap_total = 0;
		p->dialog = NULL;
		p->dialog_len = 0;
	}

	/* Set lod envelope to defaults. */
	sort_perif_ratio = 1; /* render decomplextimates */
	sort_far_ratio = 15; /* approx once per 1/2 second */
	near_threshf = 10000.0; /* represent 10000m */
	perif_threshf  = 100000.0;
}

/* Free all dynamic memory associated with vohspace and selection buffer. */
void
free_vohspace_memory(void)
{
	int i;
	for (i = 0; i < vrt_hmaps_max; i++) {
		free((&vohspace[i])->vmap);
		(&vohspace[i])->vmap = NULL;
		free((&vohspace[i])->dialog);
		(&vohspace[i])->dialog = NULL;
	}
	free(vohspace);
	free(near_hmaps);
	free(perif_hmaps);
	free(far_hmaps);
	free(selectf_a);
	free(selectf_b);
}

/* From given set, find then attach and return unattached hmap. */
hmapf_t *
attach_hmapf(void)
{
	int i;
	hmapf_t *map = vohspace;

	for (i = 0; i < vrt_hmaps_max; i++, map++) {
		if (!(map->attribs.mode &= VRT_MASK_ATTACHED)) {
			map->index = i; /* node-orgin unique index */
			map->attribs.mode |= VRT_MASK_ATTACHED;
			attached_hmaps++;
			return map;
		}
	}
	return NULL;
}

/* Re-initialize hmap referenced by p.  Free any associated memory. */
void
detach_hmapf(hmapf_t *p)
{
	hmapf_t *sb = &vohspace[0];
	hmapf_t *eb = &vohspace[vrt_hmaps_max];

	if ((p >= sb) && (p <= eb)) {
		p->name = 0;
		p->vpos.x = 0;
		p->vpos.y = 0;
		p->vpos.z = 0;
		p->vpos.m = 0x7f7fffff;
		p->vvel.x = 0;
		p->vvel.y = 0;
		p->vvel.z = 0;
		p->vvel.m = 0;
		p->vaxi.x = 0;
		p->vaxi.y = 0;
		p->vaxi.z = 0;
		p->vaxi.m = 0;
		p->vrel.x = 0;
		p->vrel.y = 0;
		p->vrel.z = 0;
		p->vrel.m = 0;
		p->vpre.x = 0;
		p->vpre.y = 0;
		p->vpre.z = 0;
		p->vpre.m = 0;
		p->ang_spd = 0;
		p->ang_dpl = 0;
		p->attribs.sign = 0;
		p->attribs.mode = VRT_MASK_FLOW_OVER;
		p->attribs.session_filter = 0;
		p->attribs.balance_filter = 0;
		p->attribs.kg = 0;
		p->attribs.kfactorm = 1;
		p->attribs.kfactord = 1;
		p->envelope.geom = VRT_BOUND_NONE;
		p->envelope.vsz.x = 0;
		p->envelope.vsz.z = 0;
		p->envelope.vsz.y = 0;
		p->envelope.vsz.m = 0;
		p->draw.geom = VRT_DRAWGEOM_NONE;
		p->vmap_total = 0;
		free(p->vmap);
		p->dialog = NULL;
		p->dialog_len = 0;
		p->vmap = NULL;
		free(p->dialog);
		attached_hmaps--;
		return;
	}
	__builtin_fprintf(stderr, "vrtater:%s:%d: "
		"Error: Can not detach hmap\n "
		"Pointer reference NULL or outside of buffer\n",
		__FILE__, __LINE__);
	abort();
}

/* Given viewpoint vpt, for <= vrt_hmaps_max, in each lod set, optionally(except
   for near and based on sort ratio) sort all hmaps therein, meanwhile sending
   them to be drawn or rendered apon.  Also, shift hmaps into a new lod set if
   implied by their current position, and soon bound size, given most recent
   changes.  Each lod set is sorted/sent a given 1:sort_*_ratio of times or 1:1
   for near one's based on proximity to vpt.  As the hmap holding fov0 thus vpt,
   is sent directly to proc_hmapf in advance of this function, it passes here
   yet does not get re-adjusted on the second try. */
void
sort_proc_hmaps(vf_t *vpt)
{
	int i, count;
	vf_t rel;

	passes++;

	/* Near hmaps. */
	if (dlr_n) { /* lr */

		/* Determine near count based on direction. */
		count = (lr_n + 1) - b_n;

		/* Set rl pointer to empty position. */
		rl_n = e_n + 1;

		/* Sort */
		for (i = 0; i < count; i++) {
			/* Move relative lod orgin. */
			(&rel)->x = -vpt->x + (*lr_n)->vpos.x;
			(&rel)->y = -vpt->y + (*lr_n)->vpos.y;
			(&rel)->z = -vpt->z + (*lr_n)->vpos.z;
			(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
			if ((&rel)->m <= near_threshf) {
				proc_hmapf(*lr_n, VRT_MASK_LOD_NEAR, 1);
				*(--rl_n) = *(lr_n--); /* move in near */
			} else {
				if (dlr_p) { /* move to perif left to right */
					proc_hmapf(*lr_n, VRT_MASK_LOD_PERIF, 1);
					*(++lr_p) = *(lr_n--);
				} else { /* move to perif right to left */
					proc_hmapf(*lr_n, VRT_MASK_LOD_PERIF, 1);
					*(--rl_p) = *(lr_n--);
				}
			}
		}
		dlr_n = (dlr_n + 1) % 2; /* flip direction */
		return;

	} else { /* rl */

		count = (e_n + 1) - rl_n;
		lr_n = b_n - 1;

		for (i = 0; i < count; i++) {
			(&rel)->x = -vpt->x + (*rl_n)->vpos.x;
			(&rel)->y = -vpt->y + (*rl_n)->vpos.y;
			(&rel)->z = -vpt->z + (*rl_n)->vpos.z;
			(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
			if ((&rel)->m <= near_threshf) {
				proc_hmapf(*rl_n, VRT_MASK_LOD_NEAR, 1);
				*(++lr_n) = *(rl_n++); /* moves in near */
			} else {
				if (dlr_p) { /* moves to perif left to right */
					proc_hmapf(*rl_n, VRT_MASK_LOD_PERIF, 1);
					*(++lr_p) = *(rl_n++);
				} else { /* moves to perif right to left */
					proc_hmapf(*rl_n, VRT_MASK_LOD_PERIF, 1);
					*(--rl_p) = *(rl_n++);
				}
			}
		}
		dlr_n = (dlr_n + 1) % 2; /* flip direction */
		return;
	}

	/* Perif hmaps. */
	if (dlr_p) {

		/* Ratio */
		if (!(passes % sort_perif_ratio)) {

			count = (lr_p + 1) - b_p;
			rl_p = e_p + 1;

			for (i = 0; i < count; i++) {
				(&rel)->x = -vpt->x + (*lr_p)->vpos.x;
				(&rel)->y = -vpt->y + (*lr_p)->vpos.y;
				(&rel)->z = -vpt->z + (*lr_p)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if (((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
					proc_hmapf(*lr_p, VRT_MASK_LOD_PERIF, sort_perif_ratio);
					*(--rl_p) = *(lr_p--); /* move in perif */
				} else {
					if ((&rel)->m <= near_threshf) {
						if (dlr_n) { /* moves to near left to right */
							proc_hmapf(*lr_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(++lr_n) = *(lr_p--);
						} else { /* moves to near right to left */
							proc_hmapf(*lr_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(--rl_n) = *(lr_p--);
						}
					} else { /* far */
						if (dlr_f) { /* moves to far left to right */
							proc_hmapf(*lr_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(++lr_f) = *(lr_p--);
						} else { /* moves to far right to left */
							proc_hmapf(*lr_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(--rl_f) = *(lr_p--);
						}
					}
				}
			}
			dlr_p = (dlr_p + 1) % 2; /* flip direction */
			return;
		}
	} else {
		if (!(passes % sort_perif_ratio)) {

			count = (e_p + 1) - rl_p;
			lr_p = b_p - 1;

			for (i = 0; i < count; i++) {
				/* move relative lod orgin */
				(&rel)->x = -vpt->x + (*rl_p)->vpos.x;
				(&rel)->y = -vpt->y + (*rl_p)->vpos.y;
				(&rel)->z = -vpt->z + (*rl_p)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if (((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
					proc_hmapf(*rl_p, VRT_MASK_LOD_PERIF, sort_perif_ratio);
					*(++lr_p) = *(rl_p++); /* moves in perif */
				} else {
					if ((&rel)->m <= near_threshf) {
						if (dlr_n) { /* moves to near left to right */
							proc_hmapf(*rl_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(++lr_n) = *(rl_p++);

						} else { /* moves to near right to left */
							proc_hmapf(*rl_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(--rl_n) = *(rl_p++);
						}
					} else { /* far */
						if (dlr_f) { /* moves to far left to right */
							proc_hmapf(*rl_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(++lr_f) = *(rl_p++);
						} else { /* moves to far right to left */
							proc_hmapf(*rl_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(--rl_f) = *(rl_p++);
						}
					}
				}
			}
			dlr_p = (dlr_p + 1) % 2; /* flip direction */
			return;
		}
	}

	/* Far hmaps. */
	if (dlr_f) {
		if (!(passes % sort_far_ratio)) {

			count = (lr_f + 1) - b_f;
			rl_f = e_f + 1;

			for (i = 0; i < count; i++) {
				(&rel)->x = -vpt->x + (*lr_f)->vpos.x;
				(&rel)->y = -vpt->y + (*lr_f)->vpos.y;
				(&rel)->z = -vpt->z + (*lr_f)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if ((&rel)->m > perif_threshf) {
					proc_hmapf(*lr_f, VRT_MASK_LOD_FAR, sort_far_ratio);
					*(--rl_f) = *(lr_f--); /* moves in far */
				} else {
					if (((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
						if (dlr_f) { /* moves to perif left to right */
							proc_hmapf(*lr_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(++lr_p) = *(lr_f--);
						} else { /* moves to perif right to left */
							proc_hmapf(*lr_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(--rl_p) = *(lr_f--);
						}
					} else { /* near */
						if (dlr_n) { /* moves to near left to right */
							proc_hmapf(*lr_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(++lr_n) = *(lr_f--);
						} else { /* moves to near right to left */
							proc_hmapf(*lr_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(--rl_n) = *(lr_f--);
						}
					}
				}
			}
			dlr_f = (dlr_f + 1) % 2; /* flip direction */
		}
	} else {
		if (!(passes % sort_far_ratio)) {

			count = (e_f + 1) - rl_f;
			lr_f = b_f - 1;

			for (i = 0; i < count; i++) {
				(&rel)->x = -vpt->x + (*rl_f)->vpos.x;
				(&rel)->y = -vpt->y + (*rl_f)->vpos.y;
				(&rel)->z = -vpt->z + (*rl_f)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if ((&rel)->m > perif_threshf) {
					proc_hmapf(*rl_f, VRT_MASK_LOD_FAR, sort_far_ratio);
					*(++lr_f) = *(rl_f++); /* moves in far */
				} else {
					if (((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
						if (dlr_f) { /* moves to perif left to right */
							proc_hmapf(*rl_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(++lr_p) = *(rl_f++);
						} else { /* moves to perif right to left */
							proc_hmapf(*rl_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(--rl_p) = *(rl_f++);
						}
					} else { /* near */
						if (dlr_n) { /* moves to near left to right */
							proc_hmapf(*rl_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(++lr_n) = *(rl_f++);
						} else { /* moves to near right to left */
							proc_hmapf(*rl_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(--rl_n) = *(rl_f++);
						}
					}
				}
			}
			dlr_f = (dlr_f + 1) % 2; /* flip direction */
		}
	}
}

/* Per hmap reference map process humdrum attribs factored by sort_ratio.
   If map has vertices, foreward reference to map and given lod envelope to
   renderer. */
void
proc_hmapf(hmapf_t *map, int lod, int sort_ratio)
{

	vf_t d; /* delta */
	hmapf_t **sela = (hmapf_t **) selectf_a;
	hmapf_t **selb = (hmapf_t **) selectf_b;
	select_t sel = { 0, 0, sela, 0, selb };

	/* Filter out hmap holding fov0 in alternate passes while setting up for
	   intersection on the first alternate.  It arrives immediately before
	   sort_proc_hmaps per frame, then again in sort. */
	if (lod & VRT_MASK_LOD_INF) {
		fov0_name = map->name;
		*sela = map; /* for intersection */
	} else { /* skip hmap holding fov0 unless it has VRT_MASK_LOD_INF */
		if (map->name == fov0_name)
			return;
	}

	/* Tend to attribs bits. */
	if (map->attribs.sign & VRT_MASK_DETACH) {
		__builtin_printf("detaching hmap %x (", map->name);
		detach_hmapf(map);
		__builtin_printf("free maps %u/%u)\n",
			vrt_hmaps_max - attached_hmaps, vrt_hmaps_max);
	}

	/* Sum intersection effects for hmap with fov0 vs. next argued hmap.
	   note: hmaps do not intersect with themselv's, so this will work
	   for now. */
	*selb = map;
	intersection(&sel);

	/* Sum velocity into position for this frame. */
	cp_vf(&(map->vvel), &d); /* take a copy of direction/velocity */
	factor_vf(&d, &d, vrt_render_cyc * sort_ratio); /* delta given freq */
	sum_vf(&(map->vpos), &d, &(map->vpos)); /* pos' = delta vector + pos */

	/* Set vob angular displacement.  note: v_ang_vel will be pseudovector
	   where on the fly calculation will be moreso optimal. */
	map->ang_dpl += map->ang_spd * vrt_render_cyc * sort_ratio;

	/* Overflow for 2 * M_PI, without this a glitch occurs on wrap. */
	if (fabs(map->ang_dpl) >= 2 * M_PI)
		map->ang_dpl = fmodf(map->ang_dpl, 2 * M_PI);

	if (map->vmap)
		render_hmapf(map, lod);
}

/* Given any overloading, release hmaps with balance_filter over
   balance_criteria.  Needs further implementation/considerations. */
void
flow_over(btoggles_t *balance_criteria)
{
	int i;
	hmapf_t *map = &vohspace[0];

	for (i = 0; i < vrt_hmaps_max; i++, map++) {
		if ((map->attribs.mode & VRT_MASK_FLOW_OVER) &
			(map->attribs.balance_filter &= *balance_criteria))
		map->attribs.sign |= VRT_MASK_RECYCLE;
	}
	/* Then in positional round these are recycled. */
}


/* Conditionally place map at loc.  note: Still in progress.  Conditions are
   determinate where if seekpos calculation is successful, arrival becomes
   successful. */
void
nportf(hmapf_t *map, vf_t *loc)
{
	float r, e;

	if (!map) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attempted nport of NULL vob\n"
			"Nport destination was %f %f %f\n",
			__FILE__, __LINE__, loc->x, loc->y, loc->z);
		return;
	}

	/* seekpos calculation. */
	r = estimate_radiusf(map);
	/* ... */

	/* arrival, for now */
	cp_vf(loc, &(map->vpos));
	form_mag_vf(&(map->vpos));
	e = rand() * TINYRANDOM;
	wanderf(map, e, map->attribs.kg, r);
}

/* Aproximate and return, for hmap map, a fair proportion vs. where it were the
   same ideal volume and reshaped as a sphere.  Cheating a bit for now. */
float
estimate_radiusf(hmapf_t *map)
{
	float r;
	switch (map->envelope.geom) {
		case VRT_BOUND_NONE:
		r = 0;
		break;

		case VRT_BOUND_SPHERE:
		r = map->envelope.vsz.x;
		break;

		case VRT_BOUND_CYLINDER:
		r = map->envelope.vsz.x * map->envelope.vsz.y;
		break;

		case VRT_BOUND_RCUBOID:
		r = M_SQRT1_2 * sqrt(map->envelope.vsz.x * map->envelope.vsz.x + map->envelope.vsz.y * map->envelope.vsz.y + map->envelope.vsz.z * map->envelope.vsz.z);
		break;

		case VRT_BOUND_CUBE:
		r = M_SQRT1_2 * sqrt(map->envelope.vsz.x * map->envelope.vsz.x * 3);
		break;
	}
	return r;
}

/* Set hmap referenced by p to arbitrary but energy factored initial conditions
   vs. e joule units.  vvel, vaxi, vpre, ang_spd, and ang_dpl, are set vs.
   given determinate or estimated radius r and given mass m.  note: Now that e,
   r, and m are easily derived from an hmap, this will be moved to transform and
   revised some as well. */
void
wanderf(hmapf_t *p, float e, float m, float r)
{
	float kinetic_energy;
	float avg_absorbed_ke;
	float avg_reflected_ke;
	float avg_orginv_ke;
	float avg_tangentv_ke;
	double rnd, rnd1, rnd2, rnd3;
	static float sign = 1.0;

	/* Pseudo random reciprocal part factors. */
	rnd1 = (double) rand(); rnd2 = (double) rand(); rnd3 = rnd1 + rnd2;

	/* Given a spherical regular solid represented as an hmap
	   distribute input energy. */
	kinetic_energy = fabs(e) / m; /* kinetic energy per mass */
	avg_absorbed_ke = avg_reflected_ke = kinetic_energy / 2;
	avg_orginv_ke = avg_absorbed_ke * M_SQRT1_2 * (float) (rnd1 / rnd3);
	avg_tangentv_ke = avg_absorbed_ke * (1 - M_SQRT1_2) * (float) (rnd2 / rnd3);

	/* Arbitrary direction vector. */
	rnd = (double) rand() * SMALLRANDOM;
	p->vvel.x = (float) (rnd = ((rnd + (double) rand()) * SMALLRANDOM));
	if (((int) rnd) % 2) { sign = sign * -1.0; }
	p->vvel.x = copysign(p->vvel.x, sign);
	p->vvel.y = (float) (rnd = ((rnd + (double) rand()) * SMALLRANDOM));
	if (((int) rnd) % 2) { sign = sign * -1.0; }
	p->vvel.y = copysign(p->vvel.y, sign);
	p->vvel.z = (float) (rnd = ((rnd + (double) rand()) * SMALLRANDOM));
	if (((int) rnd) % 2) { sign = sign * -1.0; }
	p->vvel.z = copysign(p->vvel.z, sign);
	normz_vf(&(p->vvel), &(p->vvel));

	/* Velocity thereapon. */
	tele_magz_vf(&(p->vvel), &(p->vvel), avg_orginv_ke); /* m/s */

	/* Arbitrary vpre vector axis of mass distribution. */
	p->vpre.x = (float) (rnd = ((rnd + (double) rand()) * SMALLRANDOM));
	if (((int) rnd) % 2) { sign = sign * -1.0; }
	p->vpre.x = copysign(p->vpre.x, sign);
	p->vpre.y = (float) (rnd = ((rnd + (double) rand()) * SMALLRANDOM));
	if (((int) rnd) % 2) { sign = sign * -1.0; }
	p->vpre.y = copysign(p->vpre.y, sign);
	p->vpre.z = (float) (rnd = ((rnd + (double) rand()) * SMALLRANDOM));
	if (((int) rnd) % 2) { sign = sign * -1.0; }
	p->vpre.z = copysign(p->vpre.z, sign);
	normz_vf(&(p->vpre), &(p->vpre));

	/* Arbitrary vaxi and thus relative rotation/sense when vaxi is
	   combined with ang_dpl.  for now vpre is unused, so use that. */
	cp_vf(&(p->vpre), &(p->vaxi));

	/* Arbitrary signed rotation speed and initial displacement. */
	p->ang_spd = ANG_AFS * avg_tangentv_ke / r; /* r/s */
	rnd = (rnd + (double) rand()) / 2;
	if (((int) rnd) % 2) { sign = sign * -1.0; }
	p->ang_spd = copysign(p->ang_spd, sign);
	p->ang_dpl = (float) rnd + (double) rand();
}
