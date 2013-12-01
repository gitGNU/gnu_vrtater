/* rotation.c: Rotation of vectors.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include "vectors.h"

/* Given a 3d vector orgin be that place where it would have 0 magnitude,
   rotate a around axi by dpl, where a and axi share the same orgin, then
   placing vector result in b.  Return vector refrence to b.
   desires: Cycle saving optimizations. */
vf_t
*rotate_vf(vf_t *a, vf_t *axi, float dpl, vf_t *b)
{
  vf_t b2, b1, v_new, temp;
  cprod_vf(a, axi, &b2);
  cprod_vf(axi, &b2, &b1);
  dif_vf(a, &b1, &v_new);
  factor_vf(&b1, cos(dpl), &temp);
  sum_vf(&v_new, &temp, &v_new);
  factor_vf(&b2, sin(dpl), &temp);
  return(sum_vf(&v_new, &temp, b));
}
