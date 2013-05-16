/* rendergl.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_RENDERGL_H
#define VRT_RENDERGL_H

void init_renderer(void);
void init_next_buffer(void);
void draw_hmapf(hmapf_t *, int lodval);
void render_vobspace(int);

#endif /* VRT_RENDERGL_H */
