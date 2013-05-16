/* tug.c: tend to any extra pertainant system bus i/o for implicit node.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include "progscope.h"
#include "generator.h"
#include "transform.h"

enum {
	VRT_HAPTIC_NORMILL,
	VRT_INTERSECTION
};

void call_hapticNormill();
void call_intersection();

void (*fparr[])() = {
	&call_hapticNormill,
	&call_intersection
};

/* given pre selected hmaps, call a transform */
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
call_hapticNormill(void)
{
	generator_hapticNormill();
}

void
call_intersection(void)
{
	generator_intersection();
}
