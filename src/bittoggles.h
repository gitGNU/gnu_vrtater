/* bittoggles.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#if !(defined VRT_BITTOGGLES_H)
#define VRT_BITTOGGLES_H

typedef unsigned int btoggles_t; 

int test_sw(btoggles_t, btoggles_t *);
void set_sw(btoggles_t, btoggles_t *);
void unset_sw(btoggles_t, btoggles_t *);
void set_mask(btoggles_t, btoggles_t *);
void unset_mask(btoggles_t, btoggles_t *);
int true_mask(btoggles_t, btoggles_t *);
int test_sw_p(btoggles_t *, btoggles_t *);
void set_sw_p(btoggles_t *, btoggles_t *);
void unset_sw_p(btoggles_t *, btoggles_t *);
void set_mask_p(btoggles_t *, btoggles_t *);
void unset_mask_p(btoggles_t *, btoggles_t *);
int true_mask_p(btoggles_t *, btoggles_t *);
void print_toggles(char *message, btoggles_t *);

#endif /* !(defined VRT_BITTOGGLES_H) */
