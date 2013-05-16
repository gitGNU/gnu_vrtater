/* dialogX11.c: represent dialog for one who runs vrtater.  still pre-alpha.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <unistd.h>
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
void read_dialog_set(select_t *);
void write_dialog(select_t *, char *, int);

/* passthrough function per frame
   given sel(dialog in hmapf's via selectf_a[] or b[]) and o(generator opts). */
void
dialog(select_t *sel, gen_opts_t *o)
{
	/* ui: retrieve local dialog */
	dialog_with_local_user();

	/* for now, test write_dialog(), assume ui for now.
	   one empty dialog element is passed in for testing
	   normally generator.c would pass in given dialog from remote nodes
	   however, here tested is dialog read and write capability */
	char a_char[] = "dialog: pass here and everything is published unless encrypted\n";
	write_dialog(sel, a_char, 64); /* n = STRLEN where a_char[STRLEN] */
	/* for now this write to terminal.  assume ui */
	read_dialog_set(sel);

	/*
	   retreive dialog with remote users via given dialog in their
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

/* for now writes to terminal.  assume ui */
void
read_dialog_set(select_t *sel)
{
	int i, j;

	/* for now, print all selected dialog from seta */
	for(i=0;i<sel->counta;i++, (sel->seta)++)
		for(j=0;j<(*sel->seta)->dialog_total;j++)
			__builtin_printf("%s", (char *) \
				(*sel->seta)->p_dialog + (j * sizeof(int)));
}

void
write_dialog(select_t *sel, char *chars, int l)
{

	int i, *d;
	hmapf_t *h;
	char *p;

	p = chars;
	h = *(sel->seta);
	(*sel->seta)->dialog_total = l; /* becomes correct after alloc */
	alloc_dialog(sel);
	d = (*sel->seta)->p_dialog;
	for(i=0;i<l;i++)
		*d++ = *p++;
}
