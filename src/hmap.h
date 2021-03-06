/* hmap.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef HMAP_H
#define HMAP_H

#include "vectors.h"

/* Proposed now 96 bit session_t session with adjunct sequence value.  An hmap
   index continues, usefull for a fixed reference per vohspace.  A match on
   session.hash.l member alone or with seq will speed up matching while the m
   and h members can be skipped when sending over the ip network after partial
   exists using hmapwrapf with .vrtater options set using VRT_MASK_OPT_MINIMAL.
   Peer nodes then become 'partial' based apon having unique hash->l part in
   partial.  Meanwhile the session_t loginkeys for syncronizing nodes remain
   highly unique, providing an authorship mechanism when hmaps are timestamped
   and flow out of partial flexible nodes. */
struct hashpart {
  int h;
  int m;
  int l;
};

struct nodekey {
  struct hashpart hash;
  int seq;
};
typedef struct nodekey session_t;

struct complextimate {
  unsigned int hmap_count;
  unsigned int tl_vdata;
  unsigned int tl_dialog;
};

struct parameters {
  int hinged; /* type also therein */
  int elastic;
  int hardness;
};

struct attributes {
  int sign;
  int mode;
  int balance_filter; /* vob dur-ability vs. balance_criteria */
  float kg; /* mass representation vs. SI units */
  int kfactorm; /* 1 or 1000^exponent vs. mass */
  int kfactord; /* 1 or 1000^exponent vs. distance */
};

enum { /* attributes sign, tested at least once every state increment */
  VRT_ORDINAL_RECYCLE, /* send to recycler */
#define VRT_MASK_RECYCLE (1 << VRT_ORDINAL_RECYCLE)
  VRT_ORDINAL_BUFFER, /* stack in vobspace /w attribute indicator */
#define VRT_MASK_BUFFER (1 << VRT_ORDINAL_BUFFER)
  VRT_ORDINAL_DETACH, /* code in generator.c will detach this hmap */
#define VRT_MASK_DETACH (1 << VRT_ORDINAL_DETACH)
  VRT_ORDINAL_DIALOG, /* dialog may be buffered in hmaps */
#define VRT_MASK_DIALOG (1 << VRT_ORDINAL_DIALOG)
  VRT_ORDINAL_PARTIAL, /* apon arriving, partial */
#define VRT_MASK_PARTIAL (1 << VRT_ORDINAL_PARTIAL)
  VRT_ORDINAL_VERTICE_MODS, /* unaffix this locally when sending */
#define VRT_MASK_VERTICE_MODS (1 << VRT_ORDINAL_VERTICE_MODS)
  VRT_ORDINAL_DIALOG_MODS, /* unaffix this locally when sending */
#define VRT_MASK_DIALOG_MODS (1 << VRT_ORDINAL_DIALOG_MODS)
  VRT_ORDINAL_PARTIAL_MODS, /* sync with nodes in the formed set */
#define VRT_MASK_PARTIAL_MODS (1 << VRT_ORDINAL_PARTIAL_MODS)
  VRT_ORDINAL_RENDER_DIALOG, /* if set and implemented, render dialog */
#define VRT_MASK_RENDER_DIALOG (1 << VRT_ORDINAL_RENDER_DIALOG)
  VRT_ORDINAL_PLAYFAIR, /* option for node attribs symmetry checking */
#define VRT_MASK_PLAYFAIR (1 << VRT_ORDINAL_PLAYFAIR)
  VRT_ORDINAL_RESEND, /* resend this and all linked maps if trunk */
#define VRT_MASK_RESEND (1 << VRT_ORDINAL_RESEND)
  VRT_ORDINAL_TRUNKMAP, /* composite linked map, may be joined to tree */
#define VRT_MASK_TRUNKMAP (1 << VRT_ORDINAL_TRUNKMAP)
  VRT_ORDINAL_TREEMAP, /* composite linked map, is joined to trunk */
#define VRT_MASK_TREEMAP (1 << VRT_ORDINAL_TREEMAP)
  VRT_ORDINAL_KEYMAP /* set when used as caller map */
#define VRT_MASK_KEYMAP (1 << VRT_ORDINAL_KEYMAP)
};

enum { /* attributes mode, tested in context based functions */
  VRT_ORDINAL_ATTACHED,
#define VRT_MASK_ATTACHED (1 << VRT_ORDINAL_ATTACHED)
  VRT_ORDINAL_BALANCE_FILTER,
#define VRT_MASK_BALANCE_FILTER (1 << VRT_ORDINAL_BALANCE_FILTER)
  VRT_ORDINAL_SESSION_FILTER,
#define VRT_MASK_SESSION_FILTER (1 << VRT_ORDINAL_SESSION_FILTER)
  VRT_ORDINAL_IP, /* any map that has ever been partial */
#define VRT_MASK_IP (1 << VRT_ORDINAL_IP)
  VRT_ORDINAL_FLOW, /* ask code in dialog*.c to timestamp, publicize */
#define VRT_MASK_FLOW (1 << VRT_ORDINAL_FLOW)
  VRT_ORDINAL_HOLD, /* hold hmap time preconfigured on node */
#define VRT_MASK_HOLD (1 << VRT_ORDINAL_HOLD)
  VRT_ORDINAL_RENDER_FOLLOWS, /* for renderer */
#define VRT_MASK_RENDER_FOLLOWS (1 << VRT_ORDINAL_RENDER_FOLLOWS)
  VRT_ORDINAL_VOB_SELECTED,
#define VRT_MASK_VOB_SELECTED (1 << VRT_ORDINAL_VOB_SELECTED)
  VRT_ORDINAL_LEFT_IN_TUNNEL, /* send to recycler, 'may be deleted' */
#define VRT_MASK_LEFT_IN_TUNNEL (1 << VRT_ORDINAL_LEFT_IN_TUNNEL)
  VRT_ORDINAL_FLOW_OVER,
#define VRT_MASK_FLOW_OVER (1 << VRT_ORDINAL_FLOW_OVER)
  VRT_ORDINAL_FIXED_FORM, /* requests no deformation */
#define VRT_MASK_FIXED_FORM (1 << VRT_ORDINAL_FIXED_FORM)
  VRT_ORDINAL_EXTEND_BY_FILTER, /* map may only extend to keygroup */
#define VRT_MASK_EXTEND_BY_FILTER (1 << VRT_ORDINAL_EXTEND_BY_FILTER)
  VRT_ORDINAL_EXTEND_ANY,
#define VRT_MASK_EXTEND_ANY (1 << VRT_ORDINAL_EXTEND_ANY)
  VRT_ORDINAL_EXTEND_SEAMLESS,
#define VRT_MASK_EXTEND_SEAMLESS (1 << VRT_ORDINAL_EXTEND_SEAMLESS)
  VRT_ORDINAL_WALL_TYPEA,
#define VRT_MASK_WALL_TYPEA (1 << VRT_ORDINAL_WALL_TYPEA)
  VRT_ORDINAL_NODEMAP, /* for renderer */
#define VRT_MASK_NODEMAP (1 << VRT_ORDINAL_NODEMAP)
  VRT_ORDINAL_GROUPMAP, /* for joining and maintaining groups */
#define VRT_MASK_GROUPMAP (1 << VRT_ORDINAL_GROUPMAP)
  VRT_ORDINAL_SYNC_VERTICES, /* keep identical vertices together */
#define VRT_MASK_SYNC_VERTICES (1 << VRT_ORDINAL_SYNC_VERTICES)
  VRT_ORDINAL_POP /* if a keymap, pop holdkey when logging out */
#define VRT_MASK_POP (1 << VRT_ORDINAL_POP)
};

struct bound {
  int geom;
  vf_t vsz;
};

enum { /* bound geom */
  VRT_BOUND_NONE,
  VRT_BOUND_SPHERE,
  VRT_BOUND_CYLINDER,
  VRT_BOUND_RCUBOID,
  VRT_BOUND_CUBE
};

struct drawing {
  int geom; /* so-far, VRT_DRAWGEOM_TRIANGLES is supported */
  /* add options as per stock/renderer support */
};

enum { /* draw geom.  note precedence follows n edges */
  VRT_DRAWGEOM_NONE,
  VRT_DRAWGEOM_POINTS,
  VRT_DRAWGEOM_LINES,
  VRT_DRAWGEOM_TRIANGLES
};

struct drawlist_s {
  unsigned int *list;
  unsigned int count;
};
typedef struct drawlist_s drawlist_t;

struct hmapf_s {
  session_t name; /* network id */
  int index; /* identify's hmap from within it's given node-orgin */
  vf_t vpos; /* position vector from node-orgin orgin to hmap orgin */
  vf_t vvel; /* direction of travel/velocity vector */
  vf_t vaxi; /* pole bias along hmap vob rotational axis vs. vobspace */
  vf_t vrel; /* relative frame of reference, if any */
  vf_t vpre; /* axis of mass distribution vs. vaxi precession */
  float ang_spd; /* (r/s), angular speed about rotational axes */
  float ang_dpl; /* (r), angular displacement about rotational axes */
  struct attributes attribs; /* other data hmap carries */
  struct parameters *params; /* parameter data hmap keeps locally */
  struct bound envelope; /* bounding volume */
  struct drawing draw; /* format vs. stock/display support */
  int vmap_total; /* total hmap vertices of vf_t */
  vf_t *vmap;
  int dialog_len; /* as per strlen(), does not count trailing '\0' */
  int *dialog;
  struct hmapf_s *composite; /* link list for local hmap group if any */
  struct hmapf_s *adjoined; /* relative map in local hmap group if any */
  drawlist_t *drawlist; /* sequence for adjoined maps if trunkmap */
};
typedef struct hmapf_s hmapf_t;

struct select_s {
  int specbits;
  int counta; /* for 2 or more. counta + countb <= vrt_hmaps_max */
  hmapf_t **seta; /* hmap set a, for read, transform, or replace */
  int countb;
  hmapf_t **setb; /* complimentary set if any */
};
typedef struct select_s select_t;

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

/* Enumerated levels of detail.  These are used by attribs.c for sorting hmaps
   and generator.c for selecting a set of lod envelopes desireable vs.
   positional code.  They also provide render*.c with a determined average
   distance of hmaps from viewpoint(vpt) given and are configurable therein.
   note: currently under review is the possible combining of these with bound
   size. */
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

/* Selection buffers. */
extern hmapf_t *selectf_a;
extern hmapf_t *selectf_b;

void zero_session(session_t *);
void zero_fullname(session_t *);
void set_nodename(session_t *b, session_t *a);
void cp_session(session_t *a, session_t *b);
void cp_mapname(session_t *a, session_t *b);
int match_mapname(session_t *a, session_t *b);
int match_session(session_t *a, session_t *b);
int match_mapped(session_t *a, session_t *b);
int match_little(session_t *name, int *p);
hmapf_t *hmapf(session_t *);
hmapf_t *p_hmapf(int i); /* note: replacement will match on session name. */
hmapf_t *mapref(session_t *session);
unsigned int hmap_count(void);
unsigned int search_vohspace(hmapf_t **, int signs, int modes);
void diag_selection(select_t *sel); /* diagnostic */
void set_lod_envelopef(float near_thresh, float perif_thresh, unsigned int sp_ratio, unsigned int sf_ratio);

#endif /* HMAP_H */
