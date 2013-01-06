/* rendergl.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_RENDERGL_H
#define VRT_RENDERGL_H

void init_renderer(void);
void set_main_vobfov(vf_t *view_pos, vf_t *view_dir, vf_t *view_roll);
#define draw_hmap(p) draw_hmapf(p) /* for now */
void draw_hmapf(hmapf_t *, int lodval);

#endif /* VRT_RENDERGL_H */
