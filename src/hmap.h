/* hmap.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_HMAP_H
#define VRT_HMAP_H

#include "bittoggles.h"
#include "vectors.h"

typedef long long unsigned int session_t; /* 2^16 * n = 2^bitcount */

struct attributes {
	btoggles_t bits;
	btoggles_t modifiers;
	btoggles_t session_filter; /* vs. the max current sessions def */
	btoggles_t balance_filter; /* vob dur-ability vs. balance_criteria */
	float kg; /* mass representation vs. SI units */
	int kfactorm; /* 1 or 1000^exponent vs. mass */
	int kfactord; /* 1 or 1000^exponent vs. distance */
};
typedef struct attributes attribs_t;

enum { /* attribs_t bits, tested at least once every state increment */
	VRT_ORDINAL_HOLD, /* hold vob for given time, hold() */
#define VRT_MASK_HOLD (1 << VRT_ORDINAL_HOLD)
	VRT_ORDINAL_RECYCLE, /* send to recycler */
#define VRT_MASK_RECYCLE (1 << VRT_ORDINAL_RECYCLE)
	VRT_ORDINAL_PARTIAL,
#define VRT_MASK_PARTIAL (1 << VRT_ORDINAL_PARTIAL)
	VRT_ORDINAL_DIALOG, /* need or xtra cyc vs. dialog_len in generator */
#define	VRT_MASK_DIALOG (1 << VRT_ORDINAL_DIALOG)
	VRT_ORDINAL_BUFFER, /* stack in vobspace /w attribute indicator */
#define VRT_MASK_BUFFER (1 << VRT_ORDINAL_BUFFER)
	VRT_ORDINAL_VERTICE_MODS, /* unaffix this locally when sending */
#define VRT_MASK_VERTICE_MODS (1 << VRT_ORDINAL_VERTICE_MODS)
	VRT_ORDINAL_DIALOG_MODS, /* unaffix this locally when sending */
#define VRT_MASK_DIALOG_MODS (1 << VRT_ORDINAL_DIALOG_MODS)
	VRT_ORDINAL_DETACH /* code in generator.c will detach this hmap */
#define VRT_MASK_DETACH (1 << VRT_ORDINAL_DETACH)
};

enum { /* attribs_t modifiers, tested in context based functions */
	VRT_ORDINAL_ATTACHED, /* true when hmap is not detached */
#define VRT_MASK_ATTACHED (1 << VRT_ORDINAL_ATTACHED)
	VRT_ORDINAL_BALANCE_FILTER,
#define VRT_MASK_BALANCE_FILTER (1 << VRT_ORDINAL_BALANCE_FILTER)
	VRT_ORDINAL_SESSION_FILTER,
#define VRT_MASK_SESSION_FILTER (1 << VRT_ORDINAL_SESSION_FILTER)
	VRT_ORDINAL_PUBLISHED,
#define VRT_MASK_PUBLISHED (1 << VRT_ORDINAL_PUBLISHED)
	VRT_ORDINAL_RENDER_FOLLOWS, /* for renderer */
#define VRT_MASK_RENDER_FOLLOWS (1 << VRT_ORDINAL_RENDER_FOLLOWS)
	VRT_ORDINAL_VOB_SELECTED,
#define VRT_MASK_VOB_SELECTED (1 << VRT_ORDINAL_VOB_SELECTED)
	VRT_ORDINAL_LEFT_IN_TUNNEL, /* send to recycler(), 'may be deleted' */
#define VRT_MASK_LEFT_IN_TUNNEL (1 << VRT_ORDINAL_LEFT_IN_TUNNEL)
	VRT_ORDINAL_FLOW_OVER,
#define VRT_MASK_FLOW_OVER (1 << VRT_ORDINAL_FLOW_OVER)
	VRT_ORDINAL_FIXED_FORM, /* requests no deformation */
#define VRT_MASK_FIXED_FORM (1 << VRT_ORDINAL_FIXED_FORM)
	VRT_ORDINAL_EXTEND_BY_FILTER,
#define VRT_MASK_EXTEND_BY_FILTER (1 << VRT_ORDINAL_EXTEND_BY_FILTER)
	VRT_ORDINAL_EXTEND_ANY,
#define VRT_MASK_EXTEND_ANY (1 << VRT_ORDINAL_EXTEND_ANY)
	VRT_ORDINAL_EXTEND_SEAMLESS,
#define VRT_MASK_EXTEND_SEAMLESS (1 << VRT_ORDINAL_EXTEND_SEAMLESS)
	VRT_ORDINAL_WALL_TYPEA
#define VRT_MASK_WALL_TYPEA (1 << VRT_ORDINAL_WALL_TYPEA)
};

struct bounds {
	int geom;
	vf_t v_sz;
};
typedef struct bounds envelope_t;

enum { /* envelope_t geom */
	VRT_BOUND_NONE,
	VRT_BOUND_SPHERE,
	VRT_BOUND_CYLINDER,
	VRT_BOUND_RCUBOID,
	VRT_BOUND_CUBE
};

struct draw_format {
	int geom; /* so-far, VRT_DRAWGEOM_TRIANGLES is supported */
	/* add options as per stock/renderer support */
};
typedef struct draw_format draw_t;

enum { /* draw_t geom.  note precedence follows n edges */
	VRT_DRAWGEOM_NONE,
	VRT_DRAWGEOM_POINTS,
	VRT_DRAWGEOM_LINES,
	VRT_DRAWGEOM_TRIANGLES
};

struct hmapf {
	session_t name; /* network id */
	int index; /* in-node id */
	vf_t v_pos; /* position vector from node orgin to hmap orgin */
	vf_t v_vel; /* direction of travel/velocity vector */
	vf_t v_axi; /* pole bias along hmap vob rotational axis vs. vobspace */
	vf_t v_rel; /* optional use relative frame of reference */
	vf_t v_pre; /* axis of mass distribution vs. v_axi precession */
	float ang_spd; /* (r/s), angular speed about rotational axes */
	float ang_dpl; /* (r), angular displacement about rotational axes */
	attribs_t attribs; /* attribs, session and balance filters */
	envelope_t envelope; /* bounding volume */
	draw_t draw; /* format vs. stock/display support */
	int vf_total; /* total hmap vertices of vf_t */
	vf_t *p_data_vf; /* if p_data_vf->m == 0, has other vobspace data */
	int dialog_len; /* as per strlen(), does not count trailing '\0' */
	int *p_dialog;
};
typedef struct hmapf hmapf_t;

struct select {
	btoggles_t specbits;
	int counta; /* for 2 or more. counta + countb <= vrt_hmaps_max */
	hmapf_t **seta; /* hmap set a, for read, transform, or replace */
	int countb;
	hmapf_t **setb; /* complimentary set if any */
};
typedef struct select select_t;

enum { /* select_t specbits */
	VRT_ORDINAL_ARGSTYPE_FLOAT, /* still considering simutanious types */
#define VRT_MASK_ARGSTYPE_FLOAT (1 << VRT_ORDINAL_ARGSTYPE_FLOAT)
	VRT_ORDINAL_NULL_TERMINATED, /* if so, set count 0 */
#define VRT_MASK_NULL_TERMINATED (1 << VRT_ORDINAL_NULL_TERMINATED)
	VRT_ORDINAL_HAS_SETB, /* vs. may consider as NULL */
#define VRT_MASK_HAS_SETB (1 << VRT_ORDINAL_HAS_SETB)
	VRT_ORDINAL_IS_OUTPUT, /* hmaps do not change */
#define VRT_MASK_IS_OUTPUT (1 << VRT_ORDINAL_IS_OUTPUT)
	VRT_ORDINAL_UNDO,
#define VRT_MASK_UNDO (1 << VRT_ORDINAL_UNDO)
	VRT_ORDINAL_MOD_A,
#define VRT_MASK_MOD_A (1 << VRT_ORDINAL_MOD_A)
	VRT_ORDINAL_MOD_B,
#define VRT_MASK_MOD_B (1 << VRT_ORDINAL_MOD_B)
	VRT_ORDINAL_MOD_BOTH,
#define VRT_MASK_MOD_BOTH (1 << VRT_ORDINAL_MOD_BOTH)
	VRT_ORDINAL_SRCH_BKW
#define VRT_MASK_SRCH_BKW (1 << VRT_ORDINAL_SRCH_BKW)
};

/* enumerated levels of detail
   these are used by attribs.c for sorting hmaps and generator.c for selecting
   a set of lod envelopes desireable vs. positional code.  they also provide
   render*.c with a determined average distance of hmaps from viewpoint(vpt)
   given */
enum {
	VRT_ORDINAL_LOD_INF,
#define VRT_MASK_LOD_INF (1 << VRT_ORDINAL_LOD_INF)
	VRT_ORDINAL_LOD_NEAR,
#define VRT_MASK_LOD_NEAR (1 << VRT_ORDINAL_LOD_NEAR)
	VRT_ORDINAL_LOD_PERIF,
#define VRT_MASK_LOD_PERIF (1 << VRT_ORDINAL_LOD_PERIF)
	VRT_ORDINAL_LOD_FAR
#define VRT_MASK_LOD_FAR (1 << VRT_ORDINAL_LOD_FAR)
};

/* selection buffers */
extern hmapf_t *selectf_a;
extern hmapf_t *selectf_b;

hmapf_t *hmapf(session_t *);
hmapf_t *p_hmapf(int i);
unsigned int hmap_count(void);
void set_lod_envelopef(float near_thresh, float perif_thresh, unsigned int sp_ratio, unsigned int sf_ratio);

#endif /* VRT_HMAP_H */
