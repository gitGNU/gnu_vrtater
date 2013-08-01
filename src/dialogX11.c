/* dialogX11.c: Represent dialog for one who runs vrtater.  Still a template.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <string.h>
#include "dialogX11.h"
#include "progscope.h"
#include "transform.h"

#ifdef VRT_RENDERER_GL
#include "rendergl.h"
#endif /* VRT_RENDER_GL */

void cfg_recycler(void);
void cfg_balance(void);
void set_groups(void);
int read_dialog_set(select_t *);


/* Passthrough function per state increment. */
void
refresh_dialog_interfaces(void)
{
	cfg_recycler();
	cfg_balance();
	set_groups();
}

/* Given selection sel->seta and sel->counta, present dialog per partial
   referenced.  Also support node-partial and node-orgin partial dialog/options.
   Caller generator.c sends any newly arrived hmaps with dialog, and any ones
   with locally modified dialog, as a series of sets per partial per call. */
void
node_partial_dialog(select_t *sel, session_t *partial)
{
	/* For now function as a pre-alpha version diagnostic and simply write
	   first element of selectf_a to stdout. */
	select_t totty = { 0, 1, (hmapf_t **) selectf_a, 0, NULL };

	read_dialog_set(&totty);
	; /* proc remote options eg: VRT_MASK_HOLD */
}

/* Given selection sel->seta and sel->counta, display local dialog for the
   person running vrtater.  Also support node-orgin dialog features affecting
   hmaps and acessable through transform, and dialog interface features herein. */
int *
node_orgin_dialog(select_t *sel)
{
	select_t totty = { 0, 1, (hmapf_t **) selectf_a, 0, NULL };

	read_dialog_set(&totty);
	; /* features */
		
	return 0;
}

void
cfg_recycler(void)
{
	;
}

void
cfg_balance(void)
{
	;
}

void
set_groups()
{
	;
}

/* Depreciated.  Now a diagnostic to read and present any dialog in selectf_a to
   stdout. */
int
read_dialog_set(select_t *sel)
{
	int i, j;
	hmapf_t **m;

	m = (sel->seta);
	for (i = 0; i < sel->counta; i++, m++)
		for (j = 0; j < (*m)->dialog_len; j++)
			__builtin_printf("%s",
				(char *) (*m)->dialog + (j * sizeof(int)));
	__builtin_printf("\n");
	return sel->counta;
}
