/* vectors.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_VECTORS_H
#define VRT_VECTORS_H

struct vf {
	float x;
	float y;
	float z;
	float m;
};
typedef struct vf vf_t;

vf_t * set_vf(vf_t *, float x, float y, float z, float m);
vf_t * cp_vf(vf_t *, vf_t *);
vf_t * form_mag_vf(vf_t *);
vf_t * tele_mag_vf(vf_t *, float m);
vf_t * tele_magz_vf(vf_t *, float m);
vf_t * factor_vf(vf_t *, vf_t *, float f);
vf_t * inv_vf(vf_t *, vf_t *);
vf_t * norm_vf(vf_t *, vf_t *);
vf_t * normz_vf(vf_t *, vf_t *);
float dprod_vf(vf_t *, vf_t *);
vf_t * cprod_vf(vf_t *, vf_t *, vf_t *);
vf_t * icprod_vf(vf_t *, vf_t *, vf_t *);
vf_t * sum_vf(vf_t *, vf_t *, vf_t *);
float sum_mf(vf_t *, vf_t *);
vf_t * dif_vf(vf_t *, vf_t *, vf_t *);
float dif_mf(vf_t *, vf_t *);
vf_t * mult_vf(vf_t *, vf_t *, vf_t *);
float mult_mf(vf_t *, vf_t *);
vf_t * div_vf(vf_t *, vf_t *, vf_t *);
float div_mf(vf_t *, vf_t *);

#endif /* VRT_VECTORS_H */
