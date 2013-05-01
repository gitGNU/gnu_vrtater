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
#include "transform.h"
#include "attribs.h"
#include "session.h"
#include "dialogX11.h"
#include "stock.h"

#ifdef VRT_RENDERER_GL
#include "rendergl.h"
#endif /* VRT_RENDER_GL */

gen_opts_t genopts;
unsigned int vrt_hmaps_max; /* external */

/* selection buffer */
hmapf_t *selectf_a; /* external */
hmapf_t *selectf_b; /* external */

void generate_vohspace(void);

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

int
generate_node(void)
{
	int lval;

	/* init generator */
	init_selection_buffers();
	/* init attribs.  hmap arrays, pointers thereto */
	init_vohspace();

	/* get session '0'(primary 'in node' session name) */
	if((lval = in_node_session(&session)) != SUCCESS) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped session failed to generate node\n",
			__FILE__, __LINE__);
		return lval;
	}
	__builtin_printf("generator: new session name %llu\n", session);

	/* put configured set of hmaps into vobspace */
	generate_vohspace();

	/* renderer is statefull based on hmaps input */
	init_renderer();

	return 0;
}

void
generate_vohspace(void)
{
	/* for now */
	int i;
	hmapf_t *p;
	vf_t t; (&t)->x=.0; (&t)->y=.01; (&t)->z=-.03;
	vf_t ptl; cp_vf(&t, &ptl);

	/* hmaps from file */
	/* test(); */

	/* from stock.  any too many for vrt_hmaps_max will simply fail */
	for(i=0;i<1;i++)
		if((p = hmapf_icosahedron_b(&session)))
			nportf(p, &ptl);
	for(i=0;i<2;i++)
		if((p = hmapf_cube_b(&session, .1, .1, .1)))
			nportf(p, sum_vf(&t, &ptl, &ptl));
	for(i=0;i<6;i++)
		if((p = hmapf_icosahedron_b(&session)))
			nportf(p, sum_vf(&t, &ptl, &ptl));
	for(i=0;i<11;i++)
		if((p = hmapf_cube_b(&session, .78, .78, .78)))
			nportf(p, sum_vf(&t, &ptl, &ptl));
}

void
regenerate_scene(int *quit)
{
	set_main_vohfov(&view_pos, &view_dir, &view_roll);

	/* cycle network */
	session_sync();
	/* collect_off_node_vobs() */
	/* ... */

	/* pass on node hmaps with modified dialog to dialogf()
	   pass all in node and in node partial hmaps to dialog if
	   VRT_MASK_HMAP_MODELING set
	   for now
	   test set of one empty dialog element
	   example here pretends inbound hmap 15 has dialog.  calls dialogf() */
	static int recurrant = 0;
	hmapf_t **p = (hmapf_t **)selectf_a;
	*p = p_hmapf(15);
	select_t s = { 0, 1, (hmapf_t **)selectf_a, 0, NULL };
	if(!recurrant++)
		dialogf(&s, &genopts);

	/* sort hmaps and cue them for drawing */
	sort_proc_hmaps();
	/* given gnu timer lib giving back cycles fork_child_timer_callback()
	   for now */
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
	;
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
	free_vohspace_memory();
}

/* resize vohspace hmap memory (while maintaining connections: todo) */
int
resize_node(int size, int keep_connected)
{
	/* for now keep_connected unsupported
	   allow for increasing of hmap memory only, for now */
	if(size < vrt_hmaps_max) {
		__builtin_fprintf(stderr,  "vrtater: "
			"Error: Reducing hmap memory currently unsupported\n");
		return(-1);
	} else {
		if(keep_connected)
			/* for now unsupported.  will later implement resizing
			   of potential vohspace while maintaining sessions
			   using a dynamic allocation for contents of a_hmaps */
			;
		else {
			close_node();
			vrt_hmaps_max = size;
			generate_node();
		}
		return(0);
	}
}

void
cphmaptest(void)
{
	hmapf_t **p;

	/* test copy_hmap() */
	static int foo = 100;
	__builtin_printf("%i\n", foo);
	if(!foo--) {
		p = (hmapf_t **)selectf_a;
		*p = p_hmapf(10);
		p = (hmapf_t **)selectf_b;
		*p = p_hmapf(1);
		select_t t = { 0, 0, (hmapf_t **)selectf_a, 0, (hmapf_t **)selectf_b};
		copy_hmapf(&t);
	}
}

void
hmapwrap_unwraptst(void)
{
	hmapf_t **p = (hmapf_t **)selectf_a;
	*(p++) = p_hmapf(0);
	*p = NULL;
	select_t s = { VRT_MASK_NULL_TERMINATED, 0, (hmapf_t **)selectf_a, 0, NULL};
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
