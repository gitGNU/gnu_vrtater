/* generator.c: create initial conditions. call for vobspace feedback.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "generator.h"
#include "progscope.h"
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

unsigned int vrt_hmaps_max; /* external */
session_t in_node;
partial_t *partials_list = NULL;
int partials_count = 0;

/* selection buffer */
hmapf_t *selectf_a; /* external */
hmapf_t *selectf_b; /* external */

void init_selection_buffers(void);
void generate_vohspace(void);
void callback_close_vobspace(void);

/* diag */
void cphmaptest(hmapf_t *, hmapf_t *);
void hmapwrap_unwraptst(hmapf_t *);

void
init_selection_buffers(void)
{
	/* allocate hmapf selection buffers */
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
	int lval = -1;

	/* init generator */
	init_selection_buffers();

	/* init attribs.  hmap arrays, pointers thereto */
	init_vohspace();

	/* get 'session 0' name defining 'in-node' space */
	if((lval = in_node_session(&in_node)) != 0) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped session failed to generate node\n",
			__FILE__, __LINE__);
		return lval;
	}
	__builtin_printf("generated in-node %x\n", (int)in_node);

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
	for(i = 0; i < 1; i++)
		if((p = hmapf_icosahedron_c(&in_node, .01)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 2; i++)
		if((p = hmapf_cube_c(&in_node, 3, 3, 3)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 6; i++)
		if((p = hmapf_icosahedron_c(&in_node, 2)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 10; i++)
		if((p = hmapf_cube_c(&in_node, 100, 100, 100)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 1; i++)
		if((p = hmapf_cylinder_c(&in_node, 10, 25, 13.5, 0)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));

#define DIAG
#ifdef DIAG
	/* make 2 partials then remove them */
	hmapf_t **buffer;

	/* outer hmap for in-node */
	if((p = hmapf_cylinder_c(&in_node, 80.5, 25, 112, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = .761799;
		set_vf(&(p->v_vel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->v_axi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->v_pos), -200, 500, 0, 0));
	}

	/* inner hmap, to be attached to other nodes */
	if((p = hmapf_cylinder_c(&in_node, 80, 25, 111.5, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = .761799;
		set_vf(&(p->v_vel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->v_axi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->v_pos), -200, 500, 0, 0));
	}
	buffer = (hmapf_t **)selectf_a;
	*buffer = p;
	select_t t = { 0, 0, (hmapf_t **)selectf_a, 0, NULL};
	surfinv_hmapf(&t);

	char message[] = "thenode.";
	partial_t *thenode;
	thenode = mk_partial(message, p);

	/* next partial */
	if((p = hmapf_cylinder_c(&in_node, 80.5, 25, 112, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = -.761799;
		set_vf(&(p->v_vel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->v_axi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->v_pos), 200, 500, 0, 0));
	}

	if((p = hmapf_cylinder_c(&in_node, 80, 25, 111.5, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = -.761799;
		set_vf(&(p->v_vel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->v_axi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->v_pos), 200, 500, 0, 0));
	}
	*buffer = p;
	surfinv_hmapf(&t);

	char hellostring[] = "the really nice node.";
	partial_t *nicenode;
	nicenode = mk_partial(hellostring, p);

	test_ls_partial();
	rm_partial(thenode);
	test_ls_partial();
	rm_partial(nicenode);
	test_ls_partial();
#undef DIAG
#endif
}

void
regenerate_scene(vf_t *vpt)
{
	/* sort hmaps and cue them for drawing */
	sort_proc_hmaps(vpt);

	/* cycle network
	   fill selectf_a with ref's to hmaps to be transfered */
	session_sync();

	/* enumerate incoming hmaps that session.c retrieves from other nodes
	   and then references in selectf_b.  meanwhile write references for
	   incoming hmaps with new dialog into selectf_a */
	/* ... */

	/* modeling
	   any dialog introduced by modeling functions is sent directly to
	   dialog() directly, and will have VRT_MASK_HMAP_MODELING set.  any
	   dialog written by code in dialog.c itself is implicit.  for now,
	   simulate case where some in-node dialog, newly created by modeling
	   transforms, has been generated in hmap 15 */
	hmapf_t **p = (hmapf_t **)selectf_a;
	static int recurrant = 0;
	if(!recurrant++) {
		*p = p_hmapf(15);
		select_t s = { 0, 1, (hmapf_t **)p, 0, NULL };
		char a_char[] = "dialog: pass here and "
			"everything is published unless encrypted\n";
		add_dialog(&s, a_char, strlen(a_char), 0);
		(*p)->attribs.bits |= (VRT_MASK_DIALOG | VRT_MASK_DIALOG_MODS);
		dialog(&s);
	}

	/* timer
	   issue:
	   given gnu timer lib giving back cycles fork_child_timer_callback()
	   disabled for now, see ifnode**.c for more */
	/* usleep(33400); */ /* @rsfreq 1000 fps = approx 27.8 to 28.6(+2.8%) */
}
/* generate connection point to merge node vobspaces in a peer to peers network.
   given desc, a max 80 chars + NULL reference to a description for a partial
   vobspace, and map, a refrence to an hmap delimiting the partial space.
   return reference to partial.  note: should mabye rewrite as a linked list */
partial_t *
mk_partial(char *desc, hmapf_t *map)
{
	int i, lval;
	partial_t *partial, **incpartial = (partial_t **)partials_list;
	partial_t *swap, **incswap;
	hmapf_t **maps;

	if((partial = (partial_t *) malloc(sizeof(partial_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial struct\n",
			__FILE__, __LINE__);
		abort();
	}

	if((partial->hmaps = (hmapf_t *)
		malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL)
	{
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial hmap references\n",
			__FILE__, __LINE__);
		abort();
	}

	/* get an empty in-node partial session name */
	if((lval = in_node_session((&(partial->session)))) != 0) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped in-node partial session failed\n",
			__FILE__, __LINE__);
		free(partial->hmaps);
		free(partial);
		return NULL;
		
	} else
		__builtin_printf("generated partial %x\n",
			(int)partial->session);

	/* rewrite partials_list, empty or not, in a swap buffer */
	if((swap = (partial_t *)
		malloc((partials_count + 1) * sizeof(partial_t *))) == NULL)
	{
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc swap for partials_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partials_list;
	for(i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incswap = *incpartial;
	*incswap = partial;

	/* extend partials_list allocation by space for one partial_t ref */
	free(partials_list);
	partials_list = NULL;
	if((partials_list = (partial_t *)
		malloc((partials_count + 1) * sizeof(partial_t *))) == NULL)
	{
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partials_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* write extended partials_list */
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partials_list;
	for(i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incpartial = *incswap;
	*incpartial = *incswap;

	/* fill in values for created partial */
	(*incpartial)->desc = desc;
	(*incpartial)->nodemap = map;
	maps = (hmapf_t **)(*incpartial)->hmaps;
	*maps = map; /* place bound map reference first in list */
	(*maps)->attribs.bits |= VRT_MASK_PARTIAL;
	(*maps)->name = (*incpartial)->session | (session_t)(*maps)->index;
	(*incpartial)->partial_hmaps = 1;

	__builtin_printf(" added:\n%s\n  nodemap %x\n  "
		"contents\n\t%x\n  total (%i)\n",
		(*incpartial)->desc, (int)((*incpartial)->nodemap)->name,
		(int)(*maps)->name, (*incpartial)->partial_hmaps);

	free(swap);

	partials_count++;
	return partial;
}

/* given a node vobspace reference partial, send all hmaps therein directly to
   the recycler() and free any memory associated with the partial */
void
rm_partial(partial_t *partial)
{
	int i;
	partial_t **incpartial = (partial_t **)partials_list;
	partial_t *swap, **incswap;
	hmapf_t **maps, **contents;
	select_t t;

	/* make a swap buffer of partials_count refrences */
	if((swap = (partial_t *)
		malloc((partials_count) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partials_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* put any partial reference not given in the swap buffer, while
	   configuring for recycle() and freeing of allocations of partial */
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partials_list;
	for(i = 0; i < partials_count; i++, incpartial++) {
		if(*incpartial != partial) {
			*incswap++ = *incpartial;
		} else {
			__builtin_printf(" removing: partial %x (%i/%i)\n%s\n",
				(int)partial->session,
				(((int)(incpartial - i) - (int)partials_list)
					/ (int)sizeof(partial_t *)) + 1,
				partials_count,
				(*incpartial)->desc);
			contents = (hmapf_t **)(*incpartial)->hmaps;
			maps = (hmapf_t **)selectf_a;
			*maps = *contents;
			(&t)->counta = partial->partial_hmaps;
			(&t)->seta = (hmapf_t **)selectf_a;
		}
	}

	/* test that the partial given was found */
	if((int)(incswap - partials_count) == (int)partials_list) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Partial given not found in partials_list\n",
			__FILE__, __LINE__);
		abort();
	}
	/* recycle partial maps and free allocations of this partial */
	recycle(&t);
	__builtin_printf(" freeing %x\n", (int)partial->session);
	free(partial);
	partials_count--;

	/* rewrite partials_list */
	free(partials_list);
	if((partials_list = (partial_t *)
		malloc(partials_count * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partials_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partials_list;
	for(i = 0; i < partials_count; i++)
		*incpartial++ = *incswap++;
	free(swap);
}

void
test_ls_partial(void)
{
	int i;
	partial_t **p = (partial_t **)partials_list;

	__builtin_printf(" partials: (list)\n");
	for(i = 0; i < partials_count; i++, p++)
		__builtin_printf("%s\n", (*p)->desc);
	if(!partials_count)
		__builtin_printf("--none--\n");
}

/* called thru ifnode */
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
cphmaptest(hmapf_t *a, hmapf_t *b)
{
	hmapf_t **p;

	/* test copy_hmap() */
	p = (hmapf_t **)selectf_a;
	*p = a;
	p = (hmapf_t **)selectf_b;
	*p = b;
	select_t t = { 0, 0, (hmapf_t **)selectf_a, 0, (hmapf_t **)selectf_b};
	__builtin_printf("cphmaptest()\n");
	cp_hmapf(&t);
}

void
hmapwrap_unwraptst(hmapf_t *map)
{
	hmapf_t **p = (hmapf_t **)selectf_a;
	*(p++) = map;
	*p = NULL;
	select_t s = { VRT_MASK_NULL_TERMINATED, 0, (hmapf_t **)selectf_a, 0, NULL};
	__builtin_printf("hmapwrapf() test\n");
	hmapwrapf(&s); /* from selection buf to file */
	__builtin_printf("hmapunwrapf() test\n");
	hmapunwrapf(&s); /* from file to vobspace or selection buf */
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
