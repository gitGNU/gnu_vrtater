/* stock.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_STOCK_H
#define VRT_STOCK_H

#include "progscope.h"
#include "vectors.h"
#include "hmap.h"

/* building block face counts */
#define VRT_CAPC_FCOUNT 5
#define VRT_CUBE_B_FCOUNT 12
#define VRT_ICOSAHEDRON_B_CFCOUNT 10

void add_tri_to_hmapf(vf_t *, vf_t **);
void add_vf_to_hmap(vf_t *, vf_t **);
hmapf_t *hmapf_icosahedron_b(session_t *, float r);
hmapf_t *hmapf_cylinder_b(session_t *session, float r, int e, float l, int t);
hmapf_t *hmapf_cube_b(session_t *, float l, float w, float h);

#endif /* VRT_STOCK_H */
