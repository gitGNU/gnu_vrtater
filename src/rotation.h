/* rotation.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_ROTATION_H
#define VRT_ROTATION_H

#include "progscope.h"

#ifdef VRT_RENDERER_GL
#define rxy_vf(v, r) rxy_vrt_vf(v, r)
#define rzx_vf(v, r) rzx_gl_vf(v, r)
#define rxz_vf(v, r) rzx_gl_vf(v, r)
#define ryz_vf(v, r) ryz_gl_vf(v, r)
#endif /* VRT_RENDERER_GL */

vf_t *rxy_vrt_vf(vf_t *, float ang_dpl);
vf_t *rzx_gl_vf(vf_t *, float ang_dpl);
vf_t *ryz_gl_vf(vf_t *, float ang_dpl);
vf_t *rotate_vf(vf_t *v, vf_t *axi, float ang_dpl);
float deg_to_radf(float deg);

#endif /* VRT_ROTATION_H */
