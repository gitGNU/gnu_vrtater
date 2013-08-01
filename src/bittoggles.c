/* bittoggles.c: Abstraction for bit flipping /w print function.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include "bittoggles.h"

/* Test ordinal bit set or clear in integral
   1 if true, 0 if false. */
int
test_sw(btoggles_t givenbit, btoggles_t *btoggles)
{
	return((*btoggles & givenbit) ? 1 : 0);
}

/* Set ordinal bit in integral. */
void
set_sw(btoggles_t givenbit, btoggles_t *btoggles)
{
	*btoggles |= givenbit;
}

/* Clear ordinal bit in integral. */
void
unset_sw(btoggles_t givenbit, btoggles_t *btoggles)
{
	*btoggles &= ~givenbit;
}

/* Set masked in integral. */
void
set_mask(btoggles_t givenbits, btoggles_t *btoggles)
{
	*btoggles |= givenbits;
}

/* Clear masked in integral. */
void
unset_mask(btoggles_t givenbits, btoggles_t *btoggles)
{
	*btoggles &= ~givenbits;
}

/* Determine if given andmask true or false. */
int
true_mask(btoggles_t givenbits, btoggles_t *btoggles)
{
	return((givenbits == (*btoggles & givenbits)) ? 1 : 0);
}

/* Versions using pointer to mask. */
int
test_sw_p(btoggles_t *givenbit, btoggles_t *btoggles)
{
	return((*btoggles & *givenbit) ? 1 : 0);
}

void
set_sw_p(btoggles_t *givenbit, btoggles_t *btoggles)
{
	*btoggles |= *givenbit;
}

void
unset_sw_p(btoggles_t *givenbit, btoggles_t *btoggles)
{
	*btoggles &= ~*givenbit;
}

void
set_mask_p(btoggles_t *givenbits, btoggles_t *btoggles)
{
	*btoggles |= *givenbits;
}

void
unset_mask_p(btoggles_t *givenbits, btoggles_t *btoggles)
{
	*btoggles &= ~*givenbits;
}

int
true_mask_p(btoggles_t *givenbits, btoggles_t *btoggles)
{
	return((*givenbits == (*btoggles & *givenbits)) ? 1 : 0);
}

/* Visual feedback for given toggles activity. */
void
print_toggles(char *message, btoggles_t *btoggles)
{
	__builtin_printf("%s", message);
	int i, l;
	l = 8 * sizeof(btoggles_t);
	for (i = (l-1); i >= 0; --i) {
		if ((!((i + 1) % 8)) && (i != (l-1)))
			__builtin_printf(":");
		__builtin_printf("%s", (*btoggles & (1 << i) ? "1" : "0"));
	}
	__builtin_printf("\n");
}
