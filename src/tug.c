/* tug.c: Tend to any extra pertainant system bus i/o for implicit node.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include "generator.h"

/* notes: Some bus devices are bi-directional.  These may implement a tiny
   version of partial thereapon, still recieving nodemap.  These may therefore
   be designed as autonomous if unplugged, thus safely shutting down.  Given
   this said devices can always override any instruction from vrtater, only
   using vrtater input as a recommendation or warning. */

enum {
	VRT_JOIN_HMAPS,
	VRT_INTERSECTION
};

void call_join_hmaps();
void call_intersection();

void (*fparr[])() = {
	&call_join_hmaps,
	&call_intersection
};

/* Given pre selected hmaps, call function out of transform.c */
void
transform(int t)
{
	(*fparr[t])();
}

int
init_tug_io(void)
{
	return 0;
}

void
call_join_hmaps(void)
{
	generator_join_hmaps();
}

void
call_intersection(void)
{
	generator_intersection();
}
