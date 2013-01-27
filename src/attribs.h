/* attribs.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_ATTRIBS_H
#define VRT_ATTRIBS_H

#include "progscope.h"

void init_vohspace(void);
#define attach_hmap(hmap) attach_hmapf() /* for now */
hmapf_t *attach_hmapf(void);
#define detach_hmap(hmap) detach_hmapf() /* for now */
void detach_hmapf(hmapf_t *);
void sort_proc_hmaps(void);
void free_dynamic(void);
void nportf(hmapf_t *, vf_t *);

hmapf_t *a_hmaps;

#endif /* VRT_ATTRIBS_H */
