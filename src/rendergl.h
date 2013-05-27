/* rendergl.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_RENDERGL_H
#define VRT_RENDERGL_H

#include "progscope.h"
#include "hmap.h"

/* diag palette for now */
#define ORN() glColor3f(3, .9, 0)
#define BLU() glColor3f( 0,  0, .9)
#define GRN() glColor3f(.1, .9,  0)
#define YEL() glColor3f(.9, .8,  0)
#define VLT() glColor3f(.7, .2,  1)
#define RED() glColor3f(.9,  0,  0)

void init_renderer(void);
void init_next_buffer(void);
void renderer_next_genopts(genopts_t *genopts);
void render_hmapf(hmapf_t *, int lodval);
void render_vobspace(int);

#endif /* VRT_RENDERGL_H */
