/* attribs.c: store and maintain hmaps vs. node feedback creating vobspace.
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

/* for now, to ease things */
#define SMALLRANDOM 0.000000100000000
#define TINYRANDOM 0.000000001000000

unsigned int vrt_hmaps_max; /* external */
float vrt_render_cyc; /* external */
int fov0_index;

/* hmap memory, sorting */
hmapf_t *ap_hmap_near;
hmapf_t *ap_hmap_perif;
hmapf_t *ap_hmap_far;
hmapf_t **pp_lr_n;
hmapf_t **pp_rl_n;
hmapf_t **pp_lr_p;
hmapf_t **pp_rl_p;
hmapf_t **pp_lr_f;
hmapf_t **pp_rl_f;
int dlr_n, dlr_p, dlr_f;
unsigned int passes;

/* selection buffer */
hmapf_t *selectf_a; /* external */
hmapf_t *selectf_b; /* external */

void flow_over(btoggles_t *balance_criteria);
float estimate_radiusf(hmapf_t *);
void wanderf(hmapf_t *hmap, float e, float m, float r);

/* set up hmap arrays, pointers thereto, nullify their attribs */
void
init_vohspace(void)
{
	int i;
	hmapf_t *p, **pap_hmap_n;

	attached_hmaps = 0;
	if((a_hmaps = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for a_hmaps\n",
			__FILE__, __LINE__);
		abort();
	}
	if((ap_hmap_near = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for ap_hmap_near\n",
			__FILE__, __LINE__);
		abort();
	}
	if((ap_hmap_perif = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for ap_hmap_perif\n",
			__FILE__, __LINE__);
		abort();
	}
	if((ap_hmap_far = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for ap_hmap_far\n",
			__FILE__, __LINE__);
		abort();
	}

	p = &a_hmaps[0];
	pap_hmap_n = (hmapf_t **)&ap_hmap_near[0];

	/* point out of bounds when buffer empty */
	pp_lr_n = (hmapf_t **) &ap_hmap_near[0];
	pp_lr_n--;
	pp_rl_n = (hmapf_t **) &ap_hmap_near[vrt_hmaps_max - 1];
	pp_rl_n++;
	pp_lr_p = (hmapf_t **) &ap_hmap_perif[0];
	pp_lr_p--;
	pp_rl_p = (hmapf_t **) &ap_hmap_perif[vrt_hmaps_max - 1];
	pp_rl_p++;
	pp_lr_f = (hmapf_t **) &ap_hmap_far[0];
	pp_lr_f--;
	pp_rl_f = (hmapf_t **) &ap_hmap_far[vrt_hmaps_max - 1];
	pp_rl_f++;

	passes = -1;
	dlr_n = 1;
	dlr_p = 1;
	dlr_f = 1;

	/* put vrt_hmaps_max hmap pointers in near buf */
	for(i = 0; i < vrt_hmaps_max; i++, ++pp_lr_n, pap_hmap_n++)
		*pap_hmap_n = p++;
	/* set hmaps to null default */
	p = a_hmaps;
	for(i = 0; i < vrt_hmaps_max; i++, p++) {
		p->name = (session_t)0;
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
		p->ang_spd = 0; /* radians/second */
		p->ang_dpl = 0; /* radians */
		p->attribs.bits = 0;
		p->attribs.modifiers = VRT_MASK_FLOW_OVER;
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

	/* lod envelope defaults for sort_proc_hmaps(), for now */
	sort_perif_ratio = 1; /* render decomplextimates */
	sort_far_ratio = 15; /* approx once per 1/2 second */
	near_threshf = 10000.0; /* represent 10000m */
	perif_threshf  = 100000.0;
}

/* free all dynamic memory associated with vohspace and selection buffer */
void
free_vohspace_memory(void)
{
	int i;
	for(i = 0; i < vrt_hmaps_max; i++) {
		free((&a_hmaps[i])->vmap);
		(&a_hmaps[i])->vmap = NULL;
		free((&a_hmaps[i])->dialog);
		(&a_hmaps[i])->dialog = NULL;
	}
	free(a_hmaps);
	free(ap_hmap_near);
	free(ap_hmap_perif);
	free(ap_hmap_far);
	free(selectf_a);
	free(selectf_b);
}

/* from given finite set, find then attach unattached hmap */
hmapf_t *
attach_hmapf(void)
{
	int i;
	hmapf_t *p = a_hmaps;

	for(i = 0; i < vrt_hmaps_max; i++, p++) {
		if(!(p->attribs.modifiers &= VRT_MASK_ATTACHED)) {
			p->index = i; /* caller index */
			p->attribs.modifiers |= VRT_MASK_ATTACHED;
			attached_hmaps++;
			return p;
		}
	}
	return NULL;
}

/* re-initialize hmap referenced by p.  free any associated memory */
void
detach_hmapf(hmapf_t *p)
{
	hmapf_t *sb = &a_hmaps[0];
	hmapf_t *eb = &a_hmaps[vrt_hmaps_max];

	if((p >= sb) && (p <= eb)) {
		p->name = (session_t)0;
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
		p->attribs.bits = 0;
		p->attribs.modifiers = VRT_MASK_FLOW_OVER;
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

/* for <= vrt_hmaps_max, in each lod set, optionally(except for near and based
   on sort ratio) sort all hmaps therein, meanwhile sending them to be drawn or
   rendered apon.  also, shift hmaps into a new lod set if implied by their
   current position given most recent changes.  each lod set is sorted/sent
   a given 1:sort_*_ratio of times or 1:1 for near one's based on proximity to
   viewpoint(vpt).  as the hmap holding fov0 thus vpt, is sent directly to
   proc_hmapf() in advance of this function, it passes here yet does not get
   proc'd on the second try at proc. */
void
sort_proc_hmaps(vf_t *vpt)
{
	int i, count;
	vf_t rel;

	/* conditional for multiple hmap types */
	/* ... */

	/* keep passthrough number for proximity prioritization */
	passes++;

	/* fixed pointers to array ends */
	hmapf_t **pp_b_n = (hmapf_t **) &ap_hmap_near[0];
	hmapf_t **pp_e_n = (hmapf_t **) &ap_hmap_near[vrt_hmaps_max - 1];
	hmapf_t **pp_b_p = (hmapf_t **) &ap_hmap_perif[0];
	hmapf_t **pp_e_p = (hmapf_t **) &ap_hmap_perif[vrt_hmaps_max - 1];
	hmapf_t **pp_b_f = (hmapf_t **) &ap_hmap_far[0];
	hmapf_t **pp_e_f = (hmapf_t **) &ap_hmap_far[vrt_hmaps_max - 1];

	/* sort for near, perif, far */

	/* near hmaps */
	if(dlr_n) { /* lr */

		/* determine near count based on direction */
		count = (pp_lr_n + 1) - pp_b_n;

		/* set rl pointer to empty position */
		pp_rl_n = pp_e_n + 1;

		/* sort */
		for(i = 0; i < count; i++) {
			/* move relative lod orgin */
			(&rel)->x = -vpt->x + (*pp_lr_n)->vpos.x;
			(&rel)->y = -vpt->y + (*pp_lr_n)->vpos.y;
			(&rel)->z = -vpt->z + (*pp_lr_n)->vpos.z;
			(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
			if((&rel)->m <= near_threshf) {
				proc_hmapf(*pp_lr_n, VRT_MASK_LOD_NEAR, 1);
				*(--pp_rl_n) = *(pp_lr_n--); /* move in near */
			} else {
				if(dlr_p) { /* move to perif left to right */
					proc_hmapf(*pp_lr_n, VRT_MASK_LOD_PERIF, 1);
					*(++pp_lr_p) = *(pp_lr_n--);
				} else { /* move to perif right to left */
					proc_hmapf(*pp_lr_n, VRT_MASK_LOD_PERIF, 1);
					*(--pp_rl_p) = *(pp_lr_n--);
				}
			}
		}
		dlr_n = (dlr_n + 1) % 2; /* flip direction */

	} else { /* rl */

		/* determine near count based on direction */
		count = (pp_e_n + 1) - pp_rl_n;

		/* set lr pointer to empty position */
		pp_lr_n = pp_b_n - 1;

		/* sort */
		for(i = 0; i < count; i++) {
			/* move relative lod orgin */
			(&rel)->x = -vpt->x + (*pp_rl_n)->vpos.x;
			(&rel)->y = -vpt->y + (*pp_rl_n)->vpos.y;
			(&rel)->z = -vpt->z + (*pp_rl_n)->vpos.z;
			(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
			if((&rel)->m <= near_threshf) {
				proc_hmapf(*pp_rl_n, VRT_MASK_LOD_NEAR, 1);
				*(++pp_lr_n) = *(pp_rl_n++); /* moves in near */
			} else {
				if(dlr_p) { /* moves to perif left to right */
					proc_hmapf(*pp_rl_n, VRT_MASK_LOD_PERIF, 1);
					*(++pp_lr_p) = *(pp_rl_n++);
				} else { /* moves to perif right to left */
					proc_hmapf(*pp_rl_n, VRT_MASK_LOD_PERIF, 1);
					*(--pp_rl_p) = *(pp_rl_n++);
				}
			}
		}
		dlr_n = (dlr_n + 1) % 2; /* flip direction */
	}

	/* perif hmaps */
	if(dlr_p) { /* lr */

		/* ratio */
		if(!(passes % sort_perif_ratio)) {

			/* determine perif count based on direction */
			count = (pp_lr_p + 1) - pp_b_p;

			/* set rl pointer to empty position */
			pp_rl_p = pp_e_p + 1;

			/* sort */
			for(i = 0; i < count; i++) {
				/* move relative lod orgin */
				(&rel)->x = -vpt->x + (*pp_lr_p)->vpos.x;
				(&rel)->y = -vpt->y + (*pp_lr_p)->vpos.y;
				(&rel)->z = -vpt->z + (*pp_lr_p)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if(((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
					proc_hmapf(*pp_lr_p, VRT_MASK_LOD_PERIF, sort_perif_ratio);
					*(--pp_rl_p) = *(pp_lr_p--); /* move in perif */
				} else {
					if((&rel)->m <= near_threshf) {
						if(dlr_n) { /* moves to near left to right */
							proc_hmapf(*pp_lr_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(++pp_lr_n) = *(pp_lr_p--);
						} else { /* moves to near right to left */
							proc_hmapf(*pp_lr_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(--pp_rl_n) = *(pp_lr_p--);
						}
					} else { /* far */
						if(dlr_f) { /* moves to far left to right */
							proc_hmapf(*pp_lr_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(++pp_lr_f) = *(pp_lr_p--);
						} else { /* moves to far right to left */
							proc_hmapf(*pp_lr_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(--pp_rl_f) = *(pp_lr_p--);
						}
					}
				}
			}
			dlr_p = (dlr_p + 1) % 2; /* flip direction */
		}

	} else { /* rl */

		/* ratio */
		if(!(passes % sort_perif_ratio)) {

			/* determine perif count based on direction */
			count = (pp_e_p + 1) - pp_rl_p;

			/* set lr pointer to empty position */
			pp_lr_p = pp_b_p - 1;

			/* sort */
			for(i = 0; i < count; i++) {
				/* move relative lod orgin */
				(&rel)->x = -vpt->x + (*pp_rl_p)->vpos.x;
				(&rel)->y = -vpt->y + (*pp_rl_p)->vpos.y;
				(&rel)->z = -vpt->z + (*pp_rl_p)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if(((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
					proc_hmapf(*pp_rl_p, VRT_MASK_LOD_PERIF, sort_perif_ratio);
					*(++pp_lr_p) = *(pp_rl_p++); /* moves in perif */
				} else {
					if((&rel)->m <= near_threshf) {
						if(dlr_n) { /* moves to near left to right */
							proc_hmapf(*pp_rl_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(++pp_lr_n) = *(pp_rl_p++);

						} else { /* moves to near right to left */
							proc_hmapf(*pp_rl_p, VRT_MASK_LOD_NEAR, sort_perif_ratio);
							*(--pp_rl_n) = *(pp_rl_p++);
						}
					} else { /* far */
						if(dlr_f) { /* moves to far left to right */
							proc_hmapf(*pp_rl_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(++pp_lr_f) = *(pp_rl_p++);
						} else { /* moves to far right to left */
							proc_hmapf(*pp_rl_p, VRT_MASK_LOD_FAR, sort_perif_ratio);
							*(--pp_rl_f) = *(pp_rl_p++);
						}
					}
				}
			}
			dlr_p = (dlr_p + 1) % 2; /* flip direction */
		}
	}

	/* far hmaps */
	if(dlr_f) { /* lr */

		/* ratio */
		if(!(passes % sort_far_ratio)) {

			/* determine far count based on direction */
			count = (pp_lr_f + 1) - pp_b_f;

			/* set rl pointer to empty position */
			pp_rl_f = pp_e_f + 1;

			/* sort */
			for(i = 0; i < count; i++) {
				/* move relative lod orgin */
				(&rel)->x = -vpt->x + (*pp_lr_f)->vpos.x;
				(&rel)->y = -vpt->y + (*pp_lr_f)->vpos.y;
				(&rel)->z = -vpt->z + (*pp_lr_f)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if((&rel)->m > perif_threshf) {
					proc_hmapf(*pp_lr_f, VRT_MASK_LOD_FAR, sort_far_ratio);
					*(--pp_rl_f) = *(pp_lr_f--); /* moves in far */
				} else {
					/* note: could save cycles heretofore by moving
					   all non far to perif */
					if(((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
						if(dlr_f) { /* moves to perif left to right */
							proc_hmapf(*pp_lr_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(++pp_lr_p) = *(pp_lr_f--);
						} else { /* moves to perif right to left */
							proc_hmapf(*pp_lr_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(--pp_rl_p) = *(pp_lr_f--);
						}
					} else { /* near */
						if(dlr_n) { /* moves to near left to right */
							proc_hmapf(*pp_lr_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(++pp_lr_n) = *(pp_lr_f--);
						} else { /* moves to near right to left */
							proc_hmapf(*pp_lr_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(--pp_rl_n) = *(pp_lr_f--);
						}
					}
				}
			}
			dlr_f = (dlr_f + 1) % 2; /* flip direction */
		}

	} else { /* rl */

		/* ratio */
		if(!(passes % sort_far_ratio)) {

			/* determine far count based on direction */
			count = (pp_e_f + 1) - pp_rl_f;

			/* set lr pointer to empty position */
			pp_lr_f = pp_b_f - 1;

			/* sort */
			for(i = 0; i < count; i++) {
				/* move relative lod orgin */
				(&rel)->x = -vpt->x + (*pp_rl_f)->vpos.x;
				(&rel)->y = -vpt->y + (*pp_rl_f)->vpos.y;
				(&rel)->z = -vpt->z + (*pp_rl_f)->vpos.z;
				(&rel)->m = sqrt((&rel)->x * (&rel)->x + (&rel)->y * (&rel)->y + (&rel)->z * (&rel)->z);
				if((&rel)->m > perif_threshf) {
					proc_hmapf(*pp_rl_f, VRT_MASK_LOD_FAR, sort_far_ratio);
					*(++pp_lr_f) = *(pp_rl_f++); /* moves in far */
				} else {
					if(((&rel)->m > near_threshf) & ((&rel)->m <= perif_threshf)) {
						if(dlr_f) { /* moves to perif left to right */
							proc_hmapf(*pp_rl_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(++pp_lr_p) = *(pp_rl_f++);
						} else { /* moves to perif right to left */
							proc_hmapf(*pp_rl_f, VRT_MASK_LOD_PERIF, sort_far_ratio);
							*(--pp_rl_p) = *(pp_rl_f++);
						}
					} else { /* near */
						if(dlr_n) { /* moves to near left to right */
							proc_hmapf(*pp_rl_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(++pp_lr_n) = *(pp_rl_f++);
						} else { /* moves to near right to left */
							proc_hmapf(*pp_rl_f, VRT_MASK_LOD_NEAR, sort_far_ratio);
							*(--pp_rl_n) = *(pp_rl_f++);
						}
					}
				}
			}
			dlr_f = (dlr_f + 1) % 2; /* flip direction */
		}
	}
}

/* per hmap referenced by m, per frame in frequency, process humdrum attribs.
   foreward lod prerequisite to draw function */
void
proc_hmapf(hmapf_t *m, int lod, int sort_ratio)
{

	vf_t d; /* delta d/t^2 */
	hmapf_t **a, **b; /* selection buffer */

	/* filter out hmap holding fov0 in alternate passes */
	if(lod & VRT_MASK_LOD_INF) /* arrives pre sort_proc_hmaps() per frame */
		fov0_index = m->index;
	else { /* ignore hmap holding fov0 unless it has VRT_MASK_LOD_INF */
		if(m->index == fov0_index)
			return;
	}

	/* filter to tend to attribs.bits */
	if(m->attribs.bits & VRT_MASK_DETACH) {
		__builtin_printf("detaching hmap %x "
			"(index %i, free maps %u/%u)\n", (int)m->name, m->index,
			vrt_hmaps_max - attached_hmaps, vrt_hmaps_max);
		detach_hmapf(m);
	}

	/* adjust hmap via kbase if set */
	/* ... */

	/* for in-node partial vobs, hash together thru session_filter a list
	   of pointers to associated hmaps per associated sessions for session.c
	   sync with remote nodes */
	/* ... */

	/* sum intersection effects for hmap with fov0 vs. next argued hmap */
	a = (hmapf_t **)selectf_a;
	*a = p_hmapf(0);
	b = (hmapf_t **)selectf_b;
	*b = m;
	select_t sel = { 0, 0, (hmapf_t **)a, 0, (hmapf_t **)b };
	intersection(&sel);

	/* sum velocity into position for this frame */
	cp_vf(&(m->vvel), &d); /* take a copy of direction/velocity */
	factor_vf(&d, &d, vrt_render_cyc * sort_ratio); /* delta given freq */
	sum_vf(&(m->vpos), &d, &(m->vpos)); /* new pos = delta vector + pos */

	/* set vob angular displacement
	   note: v_ang_vel will be pseudovector.  on fly calc moreso optimal */
	m->ang_dpl += m->ang_spd * vrt_render_cyc * sort_ratio;

	/* wraparound for 2 * M_PI, without this a glitch occurs on wrap */
	if(fabs(m->ang_dpl) >= 2 * M_PI)
		m->ang_dpl = fmodf(m->ang_dpl, 2 * M_PI);

	if(m->vmap)
		render_hmapf(m, lod);
}

/* vs. overload, release any hmaps with balance_filter over balance_criteria.
   needs further implementation/considerations */
void
flow_over(btoggles_t *balance_criteria)
{
	int i;
	hmapf_t *p = &a_hmaps[0];

	for(i = 0; i < vrt_hmaps_max; i++, p++) {
		if((p->attribs.modifiers & VRT_MASK_FLOW_OVER) &
			(p->attribs.balance_filter &= *balance_criteria))
		p->attribs.bits |= VRT_MASK_RECYCLE;
	}
	/* then in positional round these are recycled */
}

/* search vohspace for hmaps holding any of attribits and return references to
   those in selectf_a.  return number of matches referenced.  until vohspace
   memory can be resized while running all partials and in-node, this search
   may be rather suboptimal, as vrt_hmaps_max hmaps are always searched
   regardless of whether they are attached or not */
int
search_vohspace(select_t *sel, int attribits)
{
	int i, matches = 0;
	hmapf_t *hmap, *p = &a_hmaps[0];

	hmap = *(sel->seta);
	for(i = 0; i < vrt_hmaps_max; i++, p++) {
		if(p->attribs.bits & attribits) {
			hmap = p;
			hmap++;
			matches++;
		}	
	}
	return matches;
}

/* tag vobspace */
/* ... */

/* if seekpos(p, loc) is successful, arrival(p, loc) becomes successful */
void
nportf(hmapf_t *p, vf_t *loc)
{
	float r;

	/* diag */
	if(!p) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attempted nport of NULL vob\n"
			"Nport destination was %f %f %f\n",
			__FILE__, __LINE__, loc->x, loc->y, loc->z);
		return;
	}

	/* seekpos() calculation */
	/* ... */

	/* nport */
	cp_vf(loc, &(p->vpos));
	form_mag_vf(&(p->vpos));

	/* arrival(), for now */
	r = estimate_radiusf(p);
	double e;
	e = (double)rand() * TINYRANDOM;
	wanderf(p, (float)e, p->attribs.kg, r);
}

/* for now, guestimate for hmap p, a proportion vs. where it were the same
   volume and reshaped as a sphere, needed to calculate rotation /w a radius
   this will be moved to transform and expanded to guess bound */
float
estimate_radiusf(hmapf_t *p)
{
	float r;
	switch(p->envelope.geom) {
		case VRT_BOUND_NONE:
		r = 0;
		break;

		case VRT_BOUND_SPHERE:
		r = p->envelope.vsz.x;
		break;

		case VRT_BOUND_CYLINDER:
		r = p->envelope.vsz.x * p->envelope.vsz.y;
		break;

		case VRT_BOUND_RCUBOID:
		r = M_SQRT1_2 * sqrt(
			p->envelope.vsz.x * p->envelope.vsz.x +
			p->envelope.vsz.y * p->envelope.vsz.y +
			p->envelope.vsz.z * p->envelope.vsz.z);
		break;

		case VRT_BOUND_CUBE:
		r = M_SQRT1_2 * sqrt(
			p->envelope.vsz.x * p->envelope.vsz.x * 3);
		break;
	}
	return r;
}

/* set vob in hmap referenced by p to arbitrary but energy factored initial
   conditions vs. given energy e.  vvel, vaxi, vpre, ang_spd, and ang_dpl,
   are set vs. given determinate or estimated radius r and given mass m.
   note: now that e, r, and m are easily derived from an hmap, this will be
   moved to transform.  also could needs a re-write */
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

	/* note: vobspace is represented as 1 default renderer unit per m.
	   e, will be represented in joule units, 1 ampere/s, or 1 neuton/m.
	   this function will be moved to transform.  e can be there derived
	   and redistributed as can r */

	/* pseudo random reciprocal part factors */
	rnd1 = (double)rand(); rnd2 = (double)rand(); rnd3 = rnd1 + rnd2;

	/* given a spherical regular solid represented as an hmap
	   distribute input energy */
	kinetic_energy = fabs(e) / m; /* kinetic energy per mass */
	avg_absorbed_ke = avg_reflected_ke = kinetic_energy / 2;
	avg_orginv_ke = avg_absorbed_ke * M_SQRT1_2 * (float)(rnd1 / rnd3);
	avg_tangentv_ke = avg_absorbed_ke * (1 - M_SQRT1_2) * (float)(rnd2 / rnd3);

	/* arbitrary direction vector */
	rnd = (double)rand() * SMALLRANDOM;
	p->vvel.x = (float)(rnd = ((rnd + (double)rand()) * SMALLRANDOM));
	if(((int)rnd) % 2) { sign = sign * -1.0; }
	p->vvel.x = copysign(p->vvel.x, sign);
	p->vvel.y = (float)(rnd = ((rnd + (double)rand()) * SMALLRANDOM));
	if(((int)rnd) % 2) { sign = sign * -1.0; }
	p->vvel.y = copysign(p->vvel.y, sign);
	p->vvel.z = (float)(rnd = ((rnd + (double)rand()) * SMALLRANDOM));
	if(((int)rnd) % 2) { sign = sign * -1.0; }
	p->vvel.z = copysign(p->vvel.z, sign);
	normz_vf(&(p->vvel), &(p->vvel));

	/* velocity thereapon */
	tele_magz_vf(&(p->vvel), &(p->vvel), avg_orginv_ke); /* m/s */

	/* arbitrary vpre vector axis of mass distribution */
	p->vpre.x = (float)(rnd = ((rnd + (double)rand()) * SMALLRANDOM));
	if(((int)rnd) % 2) { sign = sign * -1.0; }
	p->vpre.x = copysign(p->vpre.x, sign);
	p->vpre.y = (float)(rnd = ((rnd + (double)rand()) * SMALLRANDOM));
	if(((int)rnd) % 2) { sign = sign * -1.0; }
	p->vpre.y = copysign(p->vpre.y, sign);
	p->vpre.z = (float)(rnd = ((rnd + (double)rand()) * SMALLRANDOM));
	if(((int)rnd) % 2) { sign = sign * -1.0; }
	p->vpre.z = copysign(p->vpre.z, sign);
	normz_vf(&(p->vpre), &(p->vpre));

	/* arbitrary vaxi and thus relative rotation/sense when vaxi is
	   combined with ang_dpl.  for now vpre is unused, so use that */
	cp_vf(&(p->vpre), &(p->vaxi));

	/* arbitrary signed rotation speed and initial displacement */
	p->ang_spd = ANG_AFS * avg_tangentv_ke / r; /* r/s */
	rnd = (rnd + (double)rand()) / 2;
	if(((int)rnd) % 2) { sign = sign * -1.0; }
	p->ang_spd = copysign(p->ang_spd, sign);
	p->ang_dpl = (float)rnd + (double)rand();
}
