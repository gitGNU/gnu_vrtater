/* attribs.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_ATTRIBS_H
#define VRT_ATTRIBS_H

#include "progscope.h"

hmapf_t *a_hmaps;

void init_vohspace(void);
hmapf_t *attach_hmapf(void);
void detach_hmapf(hmapf_t *);
void sort_proc_hmaps(void);
void free_vohspace_memory(void);
void nportf(hmapf_t *, vf_t *);

#endif /* VRT_ATTRIBS_H */
