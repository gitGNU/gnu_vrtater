/* vectors.c: a vector library for vrtater. full testing not completed.
   Copyright (C) 2012 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include "vectors.h" /* !! */

/* set vector a */
vf_t *
set_vf(vf_t *a, float x, float y, float z, float m)
{
	a->x = x;
	a->y = y;
	a->z = z;
	a->m = m;
	return a;
}

/* copy vector a to b */
vf_t *
cp_vf(vf_t *a, vf_t *b)
{
	b->x = a->x;
	b->y = a->y;
	b->z = a->z;
	b->m = a->m;
	return b;
}

/* set a complimentary magnitude via direction components
   (x, y, z, any) -- (x, y, z, m') */
vf_t *
form_mag_vf(vf_t *a)
{
	a->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
	return a;
}

/* given desired magnitude m, set magnitude and scale direction components,
   meanwhile maintaining direction. accepts for m any float including 0.
   fast version: for a valid result a->m is a valid and non zero magnitude */
vf_t *
tele_mag_vf(vf_t *a, float m)
{
	a->x = (a->x / a->m) * m;
	a->y = (a->y / a->m) * m;
	a->z = (a->z / a->m) * m;
	a->m = m;
	return a;
}

/* given desired magnitude m, set magnitude and scale direction components
   meanwhile maintaining direction. accepts for m any float including 0.
   for a->m=0 and a->x=0 and a->y=0 and a->z=0, any or all are valid args */
vf_t *
tele_magz_vf(vf_t *a, float m)
{
	if(a->m) {
		a->x = (a->x / a->m) * m;
		a->y = (a->y / a->m) * m;
		a->z = (a->z / a->m) * m;
		a->m = m;
	} else {
		/* zero magnitude given; try to produce a magnitude */
		if((a->m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z))) {
			a->x = (a->x / a->m) * m;
			a->y = (a->y / a->m) * m;
			a->z = (a->z / a->m) * m;
			a->m = m;
		}
	}
	return a;
}

/* factor_vf:	factors magnitude and direction components */
/* scale a vector by given factor */
vf_t *
factor_vf(vf_t *a, float f)
{
	a->x *= f;
	a->y *= f;
	a->z *= f;
	a->m *= f;
	return a;
}

/* obtain inverse of a vector, set vector result in b */
vf_t *
inv_vf(vf_t *a, vf_t *b)
{
	b->x = -a->x;
	b->y = -a->y;
	b->z = -a->z;
	b->m = a->m;
	return b;
}

/* normalize a vector(set to unit length 1). set result in b.
   fast ver: assumes non 0 magnitude.  fed a 0, throws divide by zero error */
vf_t *
norm_vf(vf_t *a, vf_t *b)
{
	b->x = a->x / a->m;
	b->y = a->y / a->m;
	b->z = a->z / a->m;
	b->m = 1.0f;
	return b;
}

/* normalize a vector(set to unit length 1). 0 magnitude safe. if given vector
   has 0 direction, result vector, b, is given 0 direction and 0 magnitude */
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

/* obtain dot product of two vectors */
float
dprod_vf(vf_t *a, vf_t *b)
{
	return(a->x * b->x + a->y * b->y + a->z * b->z);
}

/* obtain cross product of two vectors, set vector result in c */
vf_t *
cprod_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->y * b->z - a->z * b->y;
	c->y = a->z * b->x - a->x * b->z;
	c->z = a->x * b->y - a->y * b->x;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* obtain inverse of cross product of two vectors, set vector result in c */
vf_t *
icprod_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = -(a->y * b->z - a->z * b->y);
	c->y = -(a->z * b->x - a->x * b->z);
	c->z = -(a->x * b->y - a->y * b->x);
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* obtain sum of two vectors, set vector result in c */
vf_t *
sum_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x + b->x;
	c->y = a->y + b->y;
	c->z = a->z + b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the sum of the magnitudes of two vectors */
float
sum_mf(vf_t *a, vf_t *b)
{
	return(a->m + b->m);
}

/* obtain difference(endpoint dist) of two vectors, set vector result in c */
vf_t *
dif_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x - b->x;
	c->y = a->y - b->y;
	c->z = a->z - b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the difference of the magnitudes of two vectors */
float
dif_mf(vf_t *a, vf_t *b)
{
	return(a->m - b->m);
}

/* obtain product of two vectors, set vector result in c */
vf_t *
mult_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = a->x * b->x;
	c->y = a->y * b->y;
	c->z = a->z * b->z;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the product of the magnitudes of two vectors */
float
mult_mf(vf_t *a, vf_t *b)
{
	return(a->m * b->m);
}

/* obtain quotient of two vectors, set vector result in c */
vf_t *
div_vf(vf_t *a, vf_t *b, vf_t *c)
{
	c->x = b->x ? a->x / b->x : 0.0f;
	c->y = b->y ? a->y / b->y : 0.0f;
	c->z = b->z ? a->z / b->z : 0.0f;
	c->m = sqrtf(c->x * c->x + c->y * c->y + c->z * c->z);
	return c;
}

/* return the quotient of the magnitudes of two vectors */
float
div_mf(vf_t *a, vf_t *b)
{
	return(b->m ? a->m / b->m : 0.0f);
}
