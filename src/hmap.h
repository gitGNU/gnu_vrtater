/* hmap.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_HMAP_H
#define VRT_HMAP_H

/* hmapf selection buffers */
extern hmapf_t *selectf_a;
extern hmapf_t *selectf_b;

void init_selection_buffers(void);
hmapf_t *hmapf(session_t *);
void add_tri_to_hmapf(vf_t *, vf_t **);
void add_vf_to_hmap(vf_t *, vf_t **);
hmapf_t *p_hmapf(int i);

#endif /* VRT_HMAP_H */
