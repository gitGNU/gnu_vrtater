/* hmap.c: calls to retreive and help build hmaps(vob attribute data struct).
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "progscope.h"
#include "attribs.h"

/* attach an empty hmap to given session */
hmapf_t *
hmapf(session_t *session)
{
	hmapf_t *p;

	/* attach, set hmap session name */
	if((p = attach_hmapf()) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: All hmaps are currently attached\n",
			__FILE__, __LINE__);
		/* dlg(VRT_VOBSPACE,
			"Error: Attempted to exceed hmap limit\n"); */
		return NULL;
	}
	/* append hmap index to session name */
	p->name = *session | (session_t)p->index;
	__builtin_printf("generated hmap %x (%i)\n",
		(int)p->name, p->index);
	return p;
}

hmapf_t *
p_hmapf(int i)
{
	return &a_hmaps[i];
}
