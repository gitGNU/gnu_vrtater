/* tug.c: tend to any extra pertainant system bus i/o for implicit node.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include "progscope.h"
#include "generator.h"
#include "transform.h"

/* note: diagnostic modes may be available and required if suitable connected
   tug is present.  These tug features will eventually be supported herein. */

/* add transform recipie qualifiers below to match transform.c */
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

/* call a transform recipie. vob(s) will have already been selected. */
void
transform(int recipe)
{
	(*fparr[recipe])();
}

int
init_tug_io(void)
{
	return 0;
}
