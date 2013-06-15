/* progscope.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_PROGSCOPE_H
#define VRT_PROGSCOPE_H

#include "bittoggles.h"
#include "vectors.h"

/* progscope essentials.  see end of this file for return value definitions */
#define VRT_X_SUPPORT
#define VRT_RENDERER_GL
#define VRT_US101KBD_X11_DIAG /* for now */
#define PI_180 0.017453292519943
#define ANG_AFS 1.697652 /* from geomutil.c (not yet ready for inclusion) */

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

/* session_t, complextimate_t, session_desc_t, prev_caller_session_t */
typedef long long unsigned int session_t; /* 2^16 * n = 2^bitcount */

struct complextimate { /* these stats kept by code in generator.c */
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
typedef struct session_desc session_desc_t;

enum { /* session_desc_t level */
	VRT_ORDINAL_SESSION_CALLED,
#define VRT_MASK_SESSION_CALLED (1 << VRT_ORDINAL_SESSION_CALLED)
	VRT_ORDINAL_SESSION_CUED,
#define VRT_MASK_SESSION_CUED (1 << VRT_ORDINAL_SESSION_CUED)
	VRT_ORDINAL_SESSION_PARTIAL,
#define VRT_MASK_SESSION_PARTIAL (1 << VRT_ORDINAL_SESSION_PARTIAL)
	VRT_ORDINAL_SESSION_INBOUND
#define VRT_MASK_SESSION_INBOUND (1 << VRT_ORDINAL_SESSION_INBOUND)
};

struct prev_caller_sessions {
	session_t session;
};
typedef struct prev_caller_sessions prev_caller_sessions_t;

/* enumerated levels of detail
   these are used by attribs.c for sorting hmaps and generator.c for selecting
   a set of lod envelopes desireable vs. positional code.  they also provide
   render*.c with a deterermined average distance from givn viewpoint(vpt) vs.
   field of view(fov) held by a given hmap.  furthermore, LOD_INF is passed
   may affect update for positional code and level of detail for renderer */
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

/* rvals */
#define LVAL_TRUE 1
#define LVAL_FALSE 0
#define SUCCESS 0
#define FAIL -1

#endif /* VRT_PROGSCOPE_H */
