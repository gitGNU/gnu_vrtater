/* dialogX11.c: represent dialog for one who runs vrtater.  still pre-alpha.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <string.h>
#include "progscope.h"
#include "hmap.h"
#include "transform.h"

#ifdef VRT_RENDERER_GL
#include "rendergl.h"
#endif /* VRT_RENDER_GL */

int *dialog_with_local_user(void);
void proc_remote_dialog(int *);
void proc_local_dialog(int *);
void cfg_recycler(void);
void cfg_balance(void);
int read_dialog_set(select_t *);

/* passthrough function per frame
   given sel(dialog in hmapf's via selectf_a[] or b[]) and o(generator opts). */
void
dialog(select_t *sel)
{
	/* ui: retrieve local dialog */
	dialog_with_local_user();

	/* generator.c passes in given dialog from in-node and in-node partial
	   that has been changed by the modeling functions, as well as dialog
	   from on-node vobs that came in from running remote nodes.  here
	   presented are simulated test case's of varied in-node vob's affected
	   by modeling.  for now function as a pre-alpha version diagnostic for
	   dialog and simply write first element of selectf_a to stdout. */
#define DIAG_OFF
#ifdef DIAG
	select_t totty = { 0, 1, (hmapf_t **)selectf_a, 0, NULL };
	read_dialog_set(&totty);
#endif

	/* retreive dialog with remote users via given dialog in their
	   hmapf's(if any).  hmapf's have count and arrive in seta.
		proc_remote_dialog()
		cfg_recycler()
		cfg_balance()
		set_groups()
	*/
}

/* start/inc local user interface.  child proc.  tended incrementally.
   produce in vob(s) desired action.  also produce output suitable for
   dialog with attached remote node(s).  also call source provided features
   herein and in scope. */
int *
dialog_with_local_user(void)
{
	return 0;
}

/* support remote user vob(s) dialog/options in 'partial vobspace' */
void
proc_remote_dialog(int *p)
{
	/* proc remote options eg: VRT_MASK_HOLD */
	;
}

/* support local vob(s) desired action */
void
proc_local_dialog(int *p)
{
	;
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

/* for now write formatted sel->seta dialog to stdout.  readable if usleep() in
   transform.c is given a fitting delay and the console can handle it */
int
read_dialog_set(select_t *sel)
{
	int i, j;
	hmapf_t **m;

	/* for now, print all selected dialog from seta */
	m = (sel->seta);
	for(i=0;i<sel->counta;i++, m++)
		for(j=0;j<(*m)->dialog_len;j++)
			__builtin_printf("%s", (char *)
				(*m)->p_dialog + (j * sizeof(int)));
	__builtin_printf("\n");
	return 0;
}
