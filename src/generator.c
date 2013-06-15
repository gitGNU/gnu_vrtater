/* generator.c: create initial conditions. call for vobspace feedback.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for now */
#include "hmap.h"
#include "vectors.h"
#include "transform.h"
#include "attribs.h"
#include "session.h"
#include "stock.h"

#ifdef VRT_X_SUPPORT
#include "dialogX11.h"
#endif /* VRT_X_SUPPORT */

#ifdef VRT_RENDERER_GL
#include "rendergl.h"
#endif /* VRT_RENDER_GL */

genopts_t genopts;
unsigned int vrt_hmaps_max; /* external */

/* selection buffer */
hmapf_t *selectf_a; /* external */
hmapf_t *selectf_b; /* external */

/* current session */
session_t session;

void init_selection_buffers(void);
void generate_vohspace(void);
void callback_close_vobspace(void);

/* diag */
void cphmaptest(void);
void hmapwrap_unwraptst(void);
void test(void);

void
init_selection_buffers(void)
{
	/* allocate hmapf selection buffers
	   note: dont talk about double number calls :-| */
	selectf_a = NULL;
	if((selectf_a = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_a\n",
			__FILE__, __LINE__);
		abort();
	}
	selectf_b = NULL;
	if((selectf_b = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_b\n",
			__FILE__, __LINE__);
		abort();
	}
}

int
generate_node(void)
{
	int lval;

	/* init generator */
	init_selection_buffers();
	/* init attribs.  hmap arrays, pointers thereto */
	init_vohspace();

	/* get session '0'(primary 'in-node' session name) */
	if((lval = in_node_session(&session)) != SUCCESS) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped session failed to generate node\n",
			__FILE__, __LINE__);
		return lval;
	}
	__builtin_printf("generator: new session name %llu\n", session);

	/* put configured set of hmaps into vohspace */
	generate_vohspace();

	/* renderer is statefull based apon hmaps input */
	init_renderer();

	return 0;
}

void
generate_vohspace(void)
{
	/* for now */
	int i;
	hmapf_t *p;
	vf_t d, ptl = { 560, 560, 560, 969.94845 };

	set_vf(&d, 0, .01, -.03, 0);
	form_mag_vf(&d); 

	/* add hmaps to in-node vs. cfg. should add test for vohspace full.
	   from file */
	/* ... */
	/* for now */
	for(i=0;i<1;i++)
		if((p = hmapf_icosahedron_c(&session, .01)))
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i=0;i<2;i++)
		if((p = hmapf_cube_c(&session, 3, 3, 3)))
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i=0;i<6;i++)
		if((p = hmapf_icosahedron_c(&session, 2)))
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i=0;i<10;i++)
		if((p = hmapf_cube_c(&session, 100, 100, 100)))
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i=0;i<1;i++)
		if((p = hmapf_cylinder_c(&session, 10, 25, 13.5, 0)))
			nportf(p, sum_vf(&d, &ptl, &ptl));
}

void
regenerate_scene(vf_t *vpt)
{
	renderer_next_genopts(&genopts);

	/* generator options.  bits in vobspace_criteria have 2 forms.  some
	   like VRT_MASK_LODSET_EXTERNAL is always cleared after use or it would
	   cause repeated writing of lod parameters.  others like
	   VRT_MASK_SHUTDOWN define a relatively infrequent condition awaiting
	   a response.  this type is continually processed until the response
	   is realized or until the bit is unset by calling code */
	if((&genopts)->vobspace_criteria & (
		VRT_MASK_SHUTDOWN |
		VRT_MASK_HMAP_MODELING |
		VRT_MASK_LODSET_EXTERNAL))
	{
		if((&genopts)->vobspace_criteria & VRT_MASK_SHUTDOWN)
		{
			if((&genopts)->vobspace_criteria & VRT_MASK_DASHF)
				;
			else
				;
		}
		else if((&genopts)->vobspace_criteria & VRT_MASK_HMAP_MODELING)
		{
			/*  */
			;
		}
		else if((&genopts)->vobspace_criteria & VRT_MASK_LODSET_EXTERNAL)
		{
			set_lod_envelopef(
				(&genopts)->sort_perif_ratio,
				(&genopts)->sort_far_ratio,
				(&genopts)->near_threshf,
				(&genopts)->perif_threshf);
			(&genopts)->vobspace_criteria &= (-1 ^ VRT_MASK_LODSET_EXTERNAL);
		}
	}

	/* sort hmaps and cue them for drawing */
	sort_proc_hmaps(vpt);

	/* cycle network
	   fill selectf_a with ref's to hmaps to be transfered */
	session_sync();

	/* enumerate incoming vobs that session.c retrieves from other nodes
	   and then references in selectf_b.  meanwhile write references for
	   incoming hmaps with new dialog into selectf_a */
	/* ... */

	/* modeling
	   any dialog introduced by modeling functions is sent directly to dialog()
	   directly, and will have VRT_MASK_HMAP_MODELING set.  any dialog
	   written by code in dialog.c itself is implicit.  for now, simulate
	   case where some in-node dialog, newly created by modeling transforms,
	   has been generated in hmap 15 */
	hmapf_t **p = (hmapf_t **)selectf_a;
	static int recurrant = 0;
	if(!recurrant++) {
		*p = p_hmapf(15);
		select_t s = { 0, 1, (hmapf_t **)p, 0, NULL };
		char a_char[] = "dialog: pass here and "
			"everything is published unless encrypted\n";
		add_dialog(&s, a_char, strlen(a_char), 0);
		(*p)->attribs.bits |= (VRT_MASK_DIALOG | VRT_MASK_DIALOG_MODS);
		dialog(&s, &genopts);
	}

	/* timer
	   issue:
	   given gnu timer lib giving back cycles fork_child_timer_callback()
	   disabled for now, see ifnode**.c for more */
	/* usleep(33400); */ /* @rsfreq 1000 fps = approx 27.8 to 28.6(+2.8%) */
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
	p = (hmapf_t **)selectf_a;
	*p = p_hmapf(15);
	p = (hmapf_t **)selectf_b;
	*p = p_hmapf(1);
	select_t t = { 0, 0, (hmapf_t **)selectf_a, 0, (hmapf_t **)selectf_b};
	__builtin_printf("cphmaptest()\n");
	cp_hmapf(&t);
}

void
hmapwrap_unwraptst(void)
{
	hmapf_t **p = (hmapf_t **)selectf_a;
	*(p++) = p_hmapf(10);
	*p = NULL;
	select_t s = { VRT_MASK_NULL_TERMINATED, 0, (hmapf_t **)selectf_a, 0, NULL};
	__builtin_printf("hmapwrapf() test\n");
	hmapwrapf(&s); /* from selection buf to file */
	__builtin_printf("hmapunwrapf() test\n");
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
