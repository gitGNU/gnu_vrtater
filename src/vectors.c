/* vectors.c: a vector library for vrtater. full testing not completed.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include "vectors.h"

/* set and return refrence to vector a, enscribing components x, y, z, and m */
vf_t *
set_vf(vf_t *a, float x, float y, float z, float m)
{
	a->x = x;
	a->y = y;
	a->z = z;
	a->m = m;
	return a;
}

/* copy vector a into vector b.  return refrence to vector b */
vf_t *
cp_vf(vf_t *a, vf_t *b)
{
	b->x = a->x;
	b->y = a->y;
	b->z = a->z;
	b->m = a->m;
	return b;
}

/* set and return reference to vector a, with complimentary magnitude via
   direction components.  (x, y, z, any) -- (x, y, z, m') */
vf_t *
form_mag_vf(vf_t *a)
{
	a->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
	return a;
}

/* given vector a and desired magnitude m, set and return reference to vector
   result in b with desired m and proportionally scaled direction.  if given
   m = 0, direction will be lost.  fast version: for a valid result a->m is a
   valid and non zero magnitude */
vf_t *
tele_mag_vf(vf_t *a, vf_t *b, float m)
{
	b->x = (a->x / a->m) * m;
	b->y = (a->y / a->m) * m;
	b->z = (a->z / a->m) * m;
	b->m = m;
	return b;
}

/* given vector a and desired magnitude m, set and return reference to vector
   result in b with desired m and proportionally scaled direction.  if a->m = 0,
   a->m is set vs. vector direction components */
vf_t *
tele_magz_vf(vf_t *a, vf_t *b, float m)
{
	if(a->m) {
		b->x = (a->x / a->m) * m;
		b->y = (a->y / a->m) * m;
		b->z = (a->z / a->m) * m;
		b->m = m;
	} else {
		/* a->m has zero magnitude; try to produce a magnitude */
		if((a->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z))) {
			b->x = (a->x / a->m) * m;
			b->y = (a->y / a->m) * m;
			b->z = (a->z / a->m) * m;
			b->m = m;
		}
	}
	return b;
}

/* calculate vector a, where scaled by given factor f.  set and return reference
   to vector result in b */
vf_t *
factor_vf(vf_t *a, vf_t *b, float f)
{
	b->x = a->x * f;
	b->y = a->y * f;
	b->z = a->z * f;
	b->m = a->m * f;
	return b;
}

/* calculate the inverse value of vector a.  set and return reference to vector
   result in b */
vf_t *
inv_vf(vf_t *a, vf_t *b)
{
	b->x = -a->x;
	b->y = -a->y;
	b->z = -a->z;
	b->m = a->m;
	return b;
}

/* calculate the normal vector for vector a.  set and return reference to result
   in b.  fast ver: assumes non 0 magnitude.  fed a 0 magnitude, this throws a
   divide by zero error */
vf_t *
norm_vf(vf_t *a, vf_t *b)
{
	b->x = a->x / a->m;
	b->y = a->y / a->m;
	b->z = a->z / a->m;
	b->m = 1.0f;
	return b;
}

/* calculate the normal vector for vector a.  set and return reference to vector
   result in b.  if a has 0 direction, b gets 0 direction and 0 magnitude */
vf_t *
normz_vf(vf_t *a, vf_t *b)
{
	if(a->m) {
		b->x = a->x / a->m;
		b->y = a->y / a->m;
		b->z = a->z / a->m;
		b->m = 1.0f;
	} else {
		/* zero magnitude given; try to produce a magnitude */
		if((b->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z))) {
			b->x = a->x / b->m;
			b->y = a->y / b->m;
			b->z = a->z / b->m;
			b->m = 1.0f;
		}
	}
	return b;
}

/* return the dot product of vectors a and b */
float
dprod_vf(vf_t *a, vf_t *b)
{
	return(a->x * b->x + a->y * b->y + a->z * b->z);
}

/* calculate the cross product of vectors a and b.  set and return refrence to
   vector result in c */
vf_t *
cprod_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->y * b->z - a->z * b->y;
	c->y = a->z * b->x - a->x * b->z;
	c->z = a->x * b->y - a->y * b->x;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* calculate the sum of vectors a and b.  set and return refrence to vector
   result in c */
vf_t *
sum_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x + b->x;
	c->y = a->y + b->y;
	c->z = a->z + b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the sum of the magnitudes of vectors a and b */
float
sum_mf(vf_t *a, vf_t *b)
{
	return(a->m + b->m);
}

/* calculate for c, a vector reference representing the endpoint distance
   (difference) between vectors a and b where their orgins need not be shared
   and where their length's represent endpoint's vs. those orgins.  set and
   return reference to vector result in c */
vf_t *
dif_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x - b->x;
	c->y = a->y - b->y;
	c->z = a->z - b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the difference of the magnitudes of vectors a and b */
float
dif_mf(vf_t *a, vf_t *b)
{
	return(a->m - b->m);
}

/* calculate the product of vector a, factored by vector b, deriving magnitude
   accordingly.  set and return reference to vector result in c */
vf_t *
mult_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x * b->x;
	c->y = a->y * b->y;
	c->z = a->z * b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the product of the magnitudes of vectors a and b */
float
mult_mf(vf_t *a, vf_t *b)
{
	return(a->m * b->m);
}

/* calculate the quotient of vector a divided by vector b, deriving magnitude
   accordingly.  set and return reference to vector result in c */
vf_t *
div_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = b->x ? a->x / b->x : 0.0f;
	c->y = b->y ? a->y / b->y : 0.0f;
	c->z = b->z ? a->z / b->z : 0.0f;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the quotient of the magnitude of vector a divided by the magnitude
   of vector b */
float
div_mf(vf_t *a, vf_t *b)
{
	return(b->m ? a->m / b->m : 0.0f);
}
