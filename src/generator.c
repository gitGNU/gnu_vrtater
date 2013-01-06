/* generator.c: create initial conditions. call for vobspace feedback.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "progscope.h"
#include "hmap.h"
#include "generator.h" /* !! */
#include "vectors.h"
#include "rotation.h"
#include "rendergl.h"
#include "transform.h"
#include "attribs.h"
#include "session.h"
#include "dialogX11.h"
#include "stock.h"

gen_opts_t genopts;

/* for incoming vobs with dialog */
hmapf_t *ap_hmapf_dialog[VRT_HMAPS_MAX];

/*test*/
void cphmaptest(void);
void hmapwrap_unwraptst(void);
void test(void);

/* main fov */
vf_t view_pos; /* position */
vf_t view_dir; /* direction and zoom */
vf_t view_roll; /* spin and depth */

/* current session */
session_t session;

/* startup configuration */
int
init_node(void)
{
	int lval;

	init_vohspace();

	/* get session '0'(primary 'in node' session name) */
	if((lval = in_node_session(&session)) != SUCCESS) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped session failed to generate node\n",
			__FILE__, __LINE__);
		return lval;
	}
	__builtin_printf("generator: new session name %llu\n", session);

	generate_vobspace();
	init_renderer();

	return 0;
}

void
generate_vobspace(void)
{
	/* for now */
	int i;
	hmapf_t *p;
	vf_t t; (&t)->x=.03; (&t)->y=.03; (&t)->z=.03;
	vf_t ptl; cp_vf(&t, &ptl);

	/* hmaps from file */
	/* test(); */

	/* from stock */
	for(i=0;i<1;i++)
		if((p = hmapf_icosahedron_b(&session)))
			nportf(p, &ptl);
	for(i=0;i<2;i++)
		if((p = hmapf_cube_b(&session, .1, .1, .1)))
			nportf(p, sum_vf(&t, &ptl, &ptl));
	for(i=0;i<17;i++)
		if((p = hmapf_icosahedron_b(&session)))
			nportf(p, sum_vf(&t, &ptl, &ptl));
}

void
regenerate_scene(int *quit)
{
	set_main_vobfov(&view_pos, &view_dir, &view_roll);

	/* cycle network
	   pass on node hmaps with modified dialog to dialogf()
	   pass all in node and in node partial hmaps to dialog if
	   VRT_MASK_HMAP_MODELING set
	   for now */
	static int recurrant = 0;
	selectf_a[0] = p_hmapf(15); /* test set of one empty dialog element */
	select_t s = { 0, 1, selectf_a, 0, NULL };
	if(!recurrant++)
		dialogf(&s, &genopts);

	session_sync();
	/* collect_off_node_vobs() */
	/* ... */
	/* sort hmaps and cue them for drawing */
	sort_proc_hmaps();
	/* given gnu timer lib giving back cycles fork_child_timer_callback()
	   for now: */
	usleep(20000);
}

/* a called function has set gen_opts, or was set herein via caller */
void
close_vobspace(double when)
{
	/* last in node session 'session 0' closeing */
	/* dlg(VRT_VOBSPACE,
		"this node will be closing in" dlgprntime(when - timer(now));
	   what = VRT_CLOSE;
	   fork_child_timer_callback(when, what);
	*/
}

void
callback_close_vobspace(void)
{
	/* dlg(VRT_VOBSPACE_CLOSED, "vobspace" dlg(VRT_NODEDSC) "closing now");
	   rndr(VRT_VOBSPACE_CLOSED);
	   cancel_sessions(&sessions);
	*/
	;
}

void
close_node(void)
{
	/* datsync() */
	free_dynamic();
}

void
cphmaptest(void)
{
	/* test copy_hmap() */
	static int foo = 100;
	__builtin_printf("%i\n", foo);
	if(!foo--) {
		hmapf_t *p = p_hmapf(10);
		hmapf_t *q = p_hmapf(1);
		selectf_a[0] = p;
		selectf_b[0] = q;
		select_t t = { 0, 0, selectf_a, 0, selectf_b };
		copy_hmapf(&t);
	}
}

void
hmapwrap_unwraptst(void)
{

	hmapf_t *p = p_hmapf(0);
	selectf_a[0] = p;
	selectf_a[1] = NULL;
	select_t s = { VRT_MASK_NULL_TERMINATED, 0, selectf_a, 0, NULL };
	hmapwrapf(&s); /* from selection buf to file */
	hmapunwrapf(&s); /* from file to vobspace or selection buf */
}

void
test(void)
{
	hmapwrap_unwraptst();
	cphmaptest();
}

/* support tug input */
void
generator_hapticNormill(void)
{
	;
}

void
generator_intersection(void)
{
	;
}
