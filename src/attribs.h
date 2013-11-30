/* attribs.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef ATTRIBS_H
#define ATTRIBS_H

#include "hmap.h"
#include "vectors.h"

hmapf_t *vohspace;
unsigned int attached_hmaps;  /* maintained by attach_hmapf, detach_hmapf */

/* Lod envelopes. */
unsigned int sort_perif_ratio; /* sort per n'th pass */
unsigned int sort_far_ratio;
float near_threshf;
float perif_threshf;

void init_vohspace(void);
void delete_vohspace(void);
hmapf_t *attach_hmapf(void);
void detach_hmapf(hmapf_t *);
void sort_proc_hmaps(vf_t *viewpoint);
void proc_hmapf(hmapf_t *, int lodval, int sort_ratio);
void nportf(hmapf_t *, vf_t *, int wander);

#endif /* ATTRIBS_H */
