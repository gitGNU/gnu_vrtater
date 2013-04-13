/* hmap.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_HMAP_H
#define VRT_HMAP_H

enum {
	LOD_NEAR,
	LOD_PERIF,
	LOD_FAR
};

/* hmapf selection buffers */
extern hmapf_t *selectf_a;
extern hmapf_t *selectf_b;

/* per display, hmaps defining feild of view */
extern hmapf_t *fov0;

void init_selection_buffers(void);
hmapf_t *hmapf(session_t *);
void add_tri_to_hmapf(vf_t *, vf_t **);
void add_vf_to_hmap(vf_t *, vf_t **);
hmapf_t *p_hmapf(int i);
void adjust_positionf(hmapf_t *, float x, float y, float z, float m);
void adjust_dir_momentf(hmapf_t *, float x, float y, float z, float m);
void adjust_pos_momentf(hmapf_t *, float x, float y, float z, float m);
void adjust_rad_momentf(hmapf_t *, float x, float y, float z, float m);
void panf(vf_t *, float lr);
void tiltf(vf_t *, float fb);
void rollf(vf_t *, float lr);
void zoomf(vf_t *, float nf);
void warpf(vf_t *, float x, float y, float z);
int seekposf(vf_t *, float x, float y, float z);

#endif /* VRT_HMAP_H */
