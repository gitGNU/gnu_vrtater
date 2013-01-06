/* rotation.c: rotate voh type vobs.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include "rotation.h" /* !! */
#include "progscope.h" /* !! */


/* given a 3d vector orgin be that place where it would have 0 magnitude,
   rotate its direction thru its xy plane about its orgin by ang_dpl radians */
vf_t *
rxy_vrt_vf(vf_t *v, float ang_dpl)
{
	/* needs better method. will be calling with v_axi */
	float planar_radius, scale, dx, dy, theta;
	/* get planar radius(2d normalize) for deriving scale */
	planar_radius = sqrt(v->x * v->x + v->y * v->y);
	scale = 1 / planar_radius;
	dx = v->x * scale;
	dy = v->y * scale;
	/* determine new angle from displacement and vector vs quadrant sign */
	theta =	ang_dpl + \
		((dy >= 0) \
			? ((dx >= 0) ? asinf(dy) : asinf(-dx) + M_PI_2) \
			: ((dx >= 0) ? asinf(dy) : asinf(dx) - M_PI_2));
	v->x = planar_radius * cosf(theta);
	v->y = planar_radius * sinf(theta);
	return(v);
}

/* rotate in zx plane about orgin by ang_dpl radians */
vf_t *
rzx_gl_vf(vf_t *v, float ang_dpl)
{
	float planar_radius, scale, dz, dx, theta;
	planar_radius = sqrt(v->z * v->z + v->x * v->x);
	scale = 1 / planar_radius;
	dz = v->z * scale;
	dx = v->x * scale;
	/* gl quadrant layout
   	   subtracting 90 degrees z becomes inverted, quadrants are adjusted */
	theta = ang_dpl + \
		((dx >= 0) \
			? ((dz >= 0) ? asinf(dx) - M_PI_2 : asinf(-dz)) \
			: ((dz >= 0) ? asinf(dx) - M_PI_2 : asinf(dz) - M_PI));
	/* adjust sine and cosine for adjusted quadrants */
	v->z = planar_radius * sinf(theta);
	v->x = planar_radius * cosf(theta);
	return(v);
}

/* rotate yz plane about orgin by ang_dpl radians */
vf_t *
ryz_gl_vf(vf_t *v, float ang_dpl)
{
	float planar_radius, scale, dy, dz, theta;
	planar_radius = sqrt(v->y * v->y + v->z * v->z);
	scale = 1 / planar_radius;
	dy = v->y * scale;
	dz = v->z * scale;
	/* gl quadrant layout
   	   adding 90 degrees z becomes inverted and quadrants are adjusted */
	theta =	ang_dpl + \
		((dz >= 0) \
			? ((dy >= 0) ? asinf(dz) + M_PI_2 : asinf(-dy) + M_PI) \
			: ((dy >= 0) ? asinf(dz) + M_PI_2 : asinf(dy)));
	/* adjust sine and cosine for adjusted quadrants */
	v->y = planar_radius * sinf(theta);
	v->z = planar_radius * cosf(theta);
	return(v);
}

/* given a 3d vector orgin be that place where it would have 0 magnitude,
   rotate v around axi by dpl, where v and axi share the same orgin
   desires: cycle saving optimizations */
vf_t
*rotate_vf(vf_t *v, vf_t *axi, float dpl )
{
	vf_t b2, b1, v_new, temp;
	cprod_vf(v, axi, &b2);
	cprod_vf(axi, &b2, &b1);
	dif_vf(v, &b1, &v_new);
	factor_vf(&b1, &temp, cos(dpl));
	sum_vf(&v_new, &temp, &v_new);
	factor_vf(&b2, &temp, sin(dpl));
	return(sum_vf(&v_new, &temp, v));
}

/* convert un/signed degrees to radian representation */
float
deg_to_radf(float deg)
{
	return(deg * 0.0174532925199);
}
