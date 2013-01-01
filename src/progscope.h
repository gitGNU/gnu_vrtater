/* progscope.h
   Copyright (C) 2012 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_TYPES_H
#define VRT_TYPES_H

#include "bittoggles.h"
#include "vectors.h"

/* progscope essentials, see below for another list of these */
#define VRT_HMAPS_MAX 20
#define VRT_UNDO_DEPTH 20
#define VRT_X_SUPPORT
#define VRT_RENDERER_GL
/* meta_u */
union meta_unit_u {
	char c;
	int i;
	float f;
};
typedef union meta_unit_u meta_u;

/* session.c: 
   defines, for now these are set for testing */
#define VRT_SCALE_SESSIONS_REQ 1 /* depends on system resources */
/* set these to taste */
#define VRT_OUTBOUND_SESSIONS_MIN 5
#define VRT_INBOUND_SESSIONS_MAX 15
/* calculate the rest
   adjust factors to taste */
#define VRT_MAX_RUNNING_SESSIONS \
(VRT_OUTBOUND_SESSIONS_MIN + VRT_INBOUND_SESSIONS_MAX * VRT_SCALE_SESSIONS_REQ)
#define VRT_MAX_CUED_SESSIONS (10 * VRT_MAX_RUNNING_SESSIONS)
#define VRT_MAX_CALLED_SESSIONS (100 * VRT_MAX_RUNNING_SESSIONS)
#define VRT_MAX_PREV_CALLER_SESSIONS (1000 * VRT_MAX_RUNNING_SESSIONS)
/* session.c:
   session_t, complextimate_t, session_desc_t, prev_caller_session_t */
typedef long long unsigned int session_t; /* 2^16 * n = 2^bitcount */
struct complextimate {
	int hmap_count;
	int avg_vertices;
	int avg_map_sz;
};
typedef struct complextimate complextimate_t;
struct session_desc {
	session_t session;
	btoggles_t level;
	char adesc[81]; /* if called, cued */
	complextimate_t partial_vobspace;
};
enum { /* level */
	VRT_ORDINAL_SESSION_CALLED,
#define VRT_MASK_SESSION_CALLED (1 << VRT_ORDINAL_SESSION_CALLED)
	VRT_ORDINAL_SESSION_CUED,
#define VRT_MASK_SESSION_CUED (1 << VRT_ORDINAL_SESSION_CUED)
	VRT_ORDINAL_SESSION_PARTIAL,
#define VRT_MASK_SESSION_PARTIAL (1 << VRT_ORDINAL_SESSION_PARTIAL)
	VRT_ORDINAL_SESSION_INBOUND
#define VRT_MASK_SESSION_INBOUND (1 << VRT_ORDINAL_SESSION_INBOUND)
};
typedef struct session_desc session_desc_t;
struct prev_caller_sessions {
	session_t session;
};
typedef struct prev_caller_sessions prev_caller_sessions_t;

/* render*.c:
   test palette */
#ifdef VRT_RENDERER_GL
#define VLT() glColor3f(.8,  4, .1)
#define BLU() glColor3f( 0,  0, .9)
#define GRN() glColor3f(.1, .9,  0)
#define YEL() glColor3f(.9, .8,  0)
#define ORN() glColor3f(.8, .3,  0)
#define RED() glColor3f(.9,  0,  0)
#endif /* VRT_RENDERER_GL */
#ifdef VRT_RENDERER_OTHER
#define VLT()
#define BLU()
#define GRN()
#define YEL()
#define ORN()
#define RED()
#endif /* VRT_RENDERER_OTHER */
/* draw_format_t */
struct draw_format_opts {
	int geom; /* so-far, VRT_DRAWGEOM_TRIANGLES is supported */
	/* add options as per stock/renderer support */
};
typedef struct draw_format_opts draw_format_t;
enum { /* geom options */
	VRT_DRAWGEOM_NONE,
	VRT_DRAWGEOM_TRIANGLES,
	VRT_DRAWGEOM_LINES
};

/* attribs.c:
   bounding_volf_t, massf_t, attribs_t */
struct bounding_volf {
	int geom;
	vf_t v_sz;
};
typedef struct bounding_volf bounding_volf_t;
struct scale_massf {
	float kg;
	int kfactor; /* 1 or 1000^exponent */
};
typedef struct scale_massf massf_t;
struct attrib_bits {
	btoggles_t bits;
	btoggles_t session_filter; /* vs. the max current sessions def */
	btoggles_t balance_filter; /* vob dur-ability */
};
typedef struct attrib_bits attribs_t;
enum { /* hmap attribs.bits */
	VRT_ORDINAL_VOB_SELECTED,
#define VRT_MASK_VOB_SELECTED (1 << VRT_ORDINAL_VOB_SELECTED)
	VRT_ORDINAL_FLOW_OVER,
#define VRT_MASK_FLOW_OVER (1 << VRT_ORDINAL_FLOW_OVER)
	VRT_ORDINAL_HOLD, /* hold vob for given time, hold() */
#define VRT_MASK_HOLD (1 << VRT_ORDINAL_HOLD)
	VRT_ORDINAL_LEFT_IN_TUNNEL, /* send to recycler(), 'may be deleted' */
#define VRT_MASK_LEFT_IN_TUNNEL (1 << VRT_ORDINAL_LEFT_IN_TUNNEL)
	VRT_ORDINAL_RECYCLE,
#define VRT_MASK_RECYCLE (1 << VRT_ORDINAL_RECYCLE)
	VRT_ORDINAL_SESSION_FILTER,
#define VRT_MASK_SESSION_FILTER (1 << VRT_ORDINAL_SESSION_FILTER)
	VRT_ORDINAL_ATTACH_BY_FILTER,
#define VRT_MASK_ATTACH_BY_FILTER (1 << VRT_ORDINAL_ATTACH_BY_FILTER)
	VRT_ORDINAL_ATTACH_ANY,
#define VRT_MASK_ATTACH_ANY (1 << VRT_ORDINAL_ATTACH_ANY)
	VRT_ORDINAL_BALANCE_FILTER,
#define VRT_MASK_BALANCE_FILTER (1 << VRT_ORDINAL_BALANCE_FILTER)
	VRT_ORDINAL_PUBLISHED,
#define VRT_MASK_PUBLISHED (1 << VRT_ORDINAL_PUBLISHED)
	VRT_ORDINAL_PARTIAL,
#define VRT_MASK_PARTIAL (1 << VRT_ORDINAL_PARTIAL)
	VRT_ORDINAL_RENDER_FOLLOWS, /* for renderer */
#define VRT_MASK_RENDER_FOLLOWS (1 << VRT_ORDINAL_RENDER_FOLLOWS)
	VRT_ORDINAL_FIXED_FORM, /* requests no deformation */
#define VRT_MASK_FIXED_FORM (1 << VRT_ORDINAL_FIXED_FORM)
	VRT_ORDINAL_DIALOG,
#define VRT_MASK_DIALOG (1 << VRT_ORDINAL_DIALOG)
	VRT_ORDINAL_BUFFER, /* stack in vobspace /w attribute indicator */
#define VRT_MASK_BUFFER (1 << VRT_ORDINAL_BUFFER)
};

/* hmap.c:
   hmapf_t, select_t */
struct hmapf {
	/* vob identifier */
	session_t name;
	int index;
	/* base quantities: distance meters(m), time seconds(s), mass (kg)
	   direction radians(r)	*/
	vf_t v_pos; /* position vector from node orgin to hmap orgin */
	vf_t v_vel; /* direction of travel/velocity vector */
	vf_t v_axi; /* pole bias along hmap vob rotational axis vs. vobspace */
	vf_t v_pre; /* axis of mass distribution vs. v_axi precession */
	float ang_spd; /* (r/s), angular speed about rotational axes */
	float ang_dpl; /* (r), angular displacement about rotational axes */
	massf_t mass;
	float kfactor; /* 1 or 1000^exponent */
	/* bounding volume */
	bounding_volf_t bounding;
	/* drawing format */
	draw_format_t draw; /* set vs. stock/display supported format options */
	/* hmap attribute bits, struct includes values for some */
	attribs_t attribs;
	/* float vertice data */
	int vf_total; /* total hmap vertices of vf_t */
	vf_t * p_data_vf; /* if p_data_vf->m == 0, has other vobspace data */
	/* dialog */
	int dialog_total; /* if dialog_total != 0 has dialog */
	int * p_dialog;
};
typedef struct hmapf hmapf_t;
/* select_t(for use through hmap selection buffer)
   example sel buf entry for a single float type hmap:
   select_t sel = { 0, 0, &sel_a[0], 0, NULL };
*/
struct select {
	btoggles_t specbits;
	int counta; /* for 2 or more. counta + countb <= VRT_HMAPS_MAX */
	hmapf_t ** seta; /* hmap set for read, transform, or replace */
	int countb;
	hmapf_t ** setb; /* complimentary set if any */
};
typedef struct select select_t;
enum { /* specbits */
	VRT_ORDINAL_ARGSTYPE_FLOAT, /* default */
#define VRT_MASK_ARGSTYPE_FLOAT (1 << VRT_ORDINAL_ARGSTYPE_FLOAT)
	VRT_ORDINAL_NULL_TERMINATED, /* if so, set count 0 */
#define VRT_MASK_NULL_TERMINATED (1 << VRT_ORDINAL_NULL_TERMINATED)
	VRT_ORDINAL_HAS_SETB, /* vs. may consider as NULL */
#define VRT_MASK_HAS_SETB (1 << VRT_ORDINAL_HAS_SETB)
	VRT_ORDINAL_IS_OUTPUT, /* vobs do not change */
#define VRT_MASK_IS_OUTPUT (1 << VRT_ORDINAL_IS_OUTPUT)
	VRT_ORDINAL_UNDO,
#define VRT_MASK_UNDO (1 << VRT_ORDINAL_UNDO)
	VRT_ORDINAL_MOD_A,
#define VRT_MASK_MOD_A (1 << VRT_ORDINAL_MOD_A)
	VRT_ORDINAL_MOD_B,
#define VRT_MASK_MOD_B (1 << VRT_ORDINAL_MOD_B)
	VRT_ORDINAL_MOD_BOTH
#define VRT_MASK_MOD_BOTH (1 << VRT_ORDINAL_MOD_BOTH)
};

/* generator.c */
enum {
	VRT_ATTACH_SEAMLESS
};
enum {
	VRT_WALL_TYPEA
};
/* generator_opts_t */
struct generator_opts {
	/* may be used by caller and called */
	btoggles_t balance_criteria;
	btoggles_t vobspace_criteria;
	int why; /* fail, ... */
	int what; /* shutdown, ... */
	double when; /* shutdown, ... */
};
typedef struct generator_opts gen_opts_t;
enum { /* vobspace criteria, these always cleared after use by generator */
	VRT_ORDINAL_SHUTDOWN,
#define VRT_MASK_SHUTDOWN (1 << VRT_ORDINAL_SHUTDOWN)
	VRT_ORDINAL_DASHF,
#define VRT_MASK_DASHF (1 << VRT_ORDINAL_DASHF)
	VRT_ORDINAL_HMAP_MODELING
#define VRT_MASK_HMAP_MODELING (1 << VRT_ORDINAL_HMAP_MODELING)
};

/* stock.c:
   bounding volume geometry */
enum {
	VRT_BOUND_NONE,
	VRT_BOUND_SPHERE,
	VRT_BOUND_CYL,
	VRT_BOUND_RCUBOID,
	VRT_BOUND_CUBE
};
/* in progress, voh asteroid: types, geometries */
enum {
	VRT_ASTEROID_TYPEC,
	VRT_ASTEROID_TYPES,
	VRT_ASTEROID_TYPEM,
	VRT_ASTEROID_GEOMA,
	VRT_ASTEROID_GEOMB,
	VRT_ASTEROID_GEOMC
};
/* vob cap's, face counts */
#define VRT_CAPC_FCOUNT 5
/* vob type sphere_b, capped, face count */
#define VRT_ICOSAHEDRON_B_CFCOUNT 10
/* vob type cube_b, face count */
#define VRT_CUBE_B_FCOUNT 12
/* vob type c asteroid, capped, face count */
#define VRT_ASTEROID_GEOMC_CFCOUNT 10
/* loose bound thresholds */
#define LOOSE_BOUND1_100 1 /* 1 cm */
#define LOOSE_BOUND1_10 1 /* 1 dm */
#define LOOSE_BOUND_1 1 /* 1 m */
#define LOOSE_BOUND_10 1 /* 10 m */
#define LOOSE_BOUND_100 1 /* 100 m */

/* more progscope */
/* rvals */
#define LVAL_TRUE 1
#define LVAL_FALSE 0
#define SUCCESS 0
#define FAIL -1

#endif /* VRT_TYPES_H */
