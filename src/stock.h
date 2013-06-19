/* stock.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_STOCK_H
#define VRT_STOCK_H

#include "vectors.h"
#include "hmap.h"

/* building block face counts */
#define VRT_CAPC_FCOUNT 5
#define VRT_ICOSAHEDRON_C_FCOUNT 20
#define VRT_CUBE_C_FCOUNT 12

void add_tri_to_hmapf(vf_t *, vf_t **);
void add_vf_to_hmap(vf_t *, vf_t **);
hmapf_t *hmapf_cube_c(session_t *, float l, float w, float h);
hmapf_t *hmapf_cylinder_c(session_t *session, float r, int e, float l, int t);
hmapf_t *hmapf_icosahedron_c(session_t *, float r);

#endif /* VRT_STOCK_H */
