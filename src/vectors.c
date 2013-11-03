/* vectors.c: A vector library for vrtater.  Full testing not completed.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include "vectors.h"

/* Set and return refrence to vector result a, with components x, y, z and m. */
vf_t *
set_vf(vf_t *a, float x, float y, float z, float m)
{
	a->x = x;
	a->y = y;
	a->z = z;
	a->m = m;
	return a;
}

/* Copy vector a into vector b.  Return refrence to vector b. */
vf_t *
cp_vf(vf_t *a, vf_t *b)
{
	b->x = a->x;
	b->y = a->y;
	b->z = a->z;
	b->m = a->m;
	return b;
}

/* Set and return reference to vector result a, with complimentary magnitude via
   direction components.  (x, y, z, any) -- (x, y, z, m') */
vf_t *
form_mag_vf(vf_t *a)
{
	a->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
	return a;
}

/* Given vector a and desired magnitude m, set and return reference to vector
   result b with desired m and proportionally scaled direction.  If given a->m
   is 0, b directional terms will reflect divide by zero return values. */
vf_t *
tele_mag_vf(vf_t *a, float m, vf_t *b)
{
	b->x = (a->x / a->m) * m;
	b->y = (a->y / a->m) * m;
	b->z = (a->z / a->m) * m;
	b->m = m;
	return b;
}

/* Given vector a and desired multiplier n, set and return reference to vector
   result b with desired b->m and proportionally scaled direction.  Where given
   n is negative, b will be the scaled inverse of a and b->m will be the scaled
   a->m.  Where a->m is 0, the value of b->m will be determined from vector a
   direction components, returning the null vector if null. */
vf_t *
tele_magz_vf(vf_t *a, float n, vf_t *b)
{
	if (a->m) {
		b->x = (a->x / a->m) * n;
		b->y = (a->y / a->m) * n;
		b->z = (a->z / a->m) * n;
		b->m = n < .0 ? -n : n;
	} else {
		/* Vector a has 0 magnitude; try to produce magnitude. */
		if((b->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z))) {
			b->x = (a->x / a->m) * n;
			b->y = (a->y / a->m) * n;
			b->z = (a->z / a->m) * n;
			b->m *= n < .0 ? -n : n;
		}
		/* Else a and b are the null vector. */
	}
	return b;
}

/* Calculate vector a, where scaled by given factor f.  Set and return reference
   to vector result b. */
vf_t *
factor_vf(vf_t *a, float f, vf_t *b)
{
	b->x = a->x * f;
	b->y = a->y * f;
	b->z = a->z * f;
	b->m = a->m * f;
	return b;
}

/* Calculate the inverse value of vector a.  Set and return reference to vector
   result b. */
vf_t *
inv_vf(vf_t *a, vf_t *b)
{
	b->x = -a->x;
	b->y = -a->y;
	b->z = -a->z;
	b->m = a->m;
	return b;
}

/* Calculate the normal vector for vector a.  Set and return reference to result
   b.  If vector a has 0 magnitude, division by zero results occur in b for
   all non-zero directional terms.  If vector a magnitude is < 0, all vector b
   directional terms are inverted vs. vector a.  b->m will always be 1. */
vf_t *
norm_vf(vf_t *a, vf_t *b)
{
	b->x = a->x / a->m;
	b->y = a->y / a->m;
	b->z = a->z / a->m;
	b->m = 1.0;
	return b;
}

/* Calculate the normal vector for vector a.  Set and return reference to vector
   result b.  If a has 0 direction, b gets 0 direction and 0 magnitude.  If
   vector a magnitude is < 0, all non-zero directional terms are inverted.
   b->m will always be 1 unless vector a has 0 direction. */
vf_t *
normz_vf(vf_t *a, vf_t *b)
{
	if (a->m) {
		b->x = a->x / a->m;
		b->y = a->y / a->m;
		b->z = a->z / a->m;
		b->m = 1.0;
	} else {
		/* Zero magnitude given; try to produce a magnitude. */
		if ((b->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z))) {
			b->x = a->x / b->m;
			b->y = a->y / b->m;
			b->z = a->z / b->m;
			b->m = 1.0;
		}
		/* Else a and b are the null vector. */
	}
	return b;
}

/* Return the dot product of vectors a and b. */
float
dprod_vf(vf_t *a, vf_t *b)
{
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

/* Calculate the cross product of vectors a and b.  Set and return refrence to
   vector result c. */
vf_t *
cprod_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->y * b->z - a->z * b->y;
	c->y = a->z * b->x - a->x * b->z;
	c->z = a->x * b->y - a->y * b->x;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* Calculate the sum of vectors a and b.  Set and return refrence to vector
   result c. */
vf_t *
sum_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x + b->x;
	c->y = a->y + b->y;
	c->z = a->z + b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* Return the sum of the magnitudes of vectors a and b. */
float
sum_mf(vf_t *a, vf_t *b)
{
	return a->m + b->m;
}

/* Set vector c so that c->m represents the endpoint distance(difference)
   between vectors a and b.  Vector orgin is implicitly shared.  Return
   vector reference c. */
vf_t *
dif_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x - b->x;
	c->y = a->y - b->y;
	c->z = a->z - b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* Return the difference of the magnitudes of vectors a and b. */
float
dif_mf(vf_t *a, vf_t *b)
{
	return a->m - b->m;
}

/* Calculate the product of vector a, factored by vector b, deriving magnitude
   accordingly.  Set and return reference to vector result c. */
vf_t *
mult_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x * b->x;
	c->y = a->y * b->y;
	c->z = a->z * b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* Return the product of the magnitudes of vectors a and b. */
float
mult_mf(vf_t *a, vf_t *b)
{
	return a->m * b->m ;
}

/* Calculate the quotient of vector a divided by vector b, deriving magnitude
   accordingly.  Set and return reference to vector result c. */
vf_t *
div_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = b->x ? a->x / b->x : .0;
	c->y = b->y ? a->y / b->y : .0;
	c->z = b->z ? a->z / b->z : .0;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* Return the quotient of the magnitude of vector a divided by the magnitude
   of vector b. */
float
div_mf(vf_t *a, vf_t *b)
{
	return b->m ? a->m / b->m : .0;
}
