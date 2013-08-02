/* generator.c: Create initial conditions.  Tend to vobspace feedback.
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

session_t node_orgin;

/* External */
unsigned int vrt_hmaps_max;

/* Diagnostics for partial space. */
#define DIAG_PARTIAL
#ifdef DIAG_PARTIAL
char thenode_message[] = "thenode.";
partial_t *thenode;
char nicenode_message[] = "the really nice node.";
partial_t *nicenode;
#endif

void init_selection_buffers(void);
void generate_vohspace(void);
void callback_close_vobspace(void);

/* Tests. */
void test_select_partial_set(void);
void test_send_partial_changes(void);
void test_detach_all_partials(void);
void test_cphmap(hmapf_t *, hmapf_t *);
void test_hmapwrap_unwrap(hmapf_t *);

/* Allocate for a and b selection buffers. */
void
init_selection_buffers(void)
{
	selectf_a = NULL;
	if ((selectf_a = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_a\n",
			__FILE__, __LINE__);
		abort();
	}
	selectf_b = NULL;
	if ((selectf_b = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_b\n",
			__FILE__, __LINE__);
		abort();
	}
}

/* Form of the basis for hmap and further node interfacing.  note: If any
   problems are encountered, this should eventually cleanly exit with any
   explanation. */
int
generate_node_orgin(void)
{
	init_selection_buffers();
	init_vohspace();
	set_node_orgin(&node_orgin);
	__builtin_printf("generated node-orgin %x\n", (int) node_orgin);
	partial_generator_list = NULL;
	partials_count = 0;
	generate_vohspace();
	init_renderer();
	init_sessions();

	return 0;
}

/* Add preconfigured hmaps to node-orgin forming vohspace. */
void
generate_vohspace(void)
{
	int i;
	hmapf_t *p;
	vf_t d, portal = { 560, 560, 560, 969.94845 };

	set_vf(&d, 0, .01, -.03, 0);
	form_mag_vf(&d); 

	/* Here hmaps will be added to node-orgin vs. config file.  For now
	   add the following given vrt_hmaps_max. */
	for (i = 0; i < 1; i++)
		if ((p = hmapf_icosahedron_c(&node_orgin, .01)) != NULL)
			nportf(p, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 2; i++)
		if ((p = hmapf_cube_c(&node_orgin, 3, 3, 3)) != NULL)
			nportf(p, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 6; i++)
		if ((p = hmapf_icosahedron_c(&node_orgin, 2)) != NULL)
			nportf(p, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 10; i++)
		if ((p = hmapf_cube_c(&node_orgin, 100, 100, 100)) != NULL)
			nportf(p, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 1; i++)
		if ((p = hmapf_cylinder_c(&node_orgin, 10, 25, 13.5, 0)) != NULL)
			nportf(p, sum_vf(&d, &portal, &portal));

#ifdef DIAG_PARTIAL
	/* Make 2 partials then remove them below on close_node. */
	hmapf_t **buffer;

	/* Outer hmap for node-orgin. */
	if ((p = hmapf_cylinder_c(&node_orgin, 80.5, 25, 112, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = .761799;
		set_vf(&(p->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->vpos), -200, 500, 0, 0));
	}

	/* Inner hmap, to be attached to other nodes. */
	if ((p = hmapf_cylinder_c(&node_orgin, 80, 25, 111.5, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = .761799;
		set_vf(&(p->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->vpos), -200, 500, 0, 0));
	}
	buffer = (hmapf_t **) selectf_a;
	*buffer = p;
	select_t t = { 0, 0, (hmapf_t **) selectf_a, 0, NULL};
	surface_inv_hmapf(&t);

	thenode = mk_partial(thenode_message, p);

	/* Next partial. */
	if ((p = hmapf_cylinder_c(&node_orgin, 80.5, 25, 112, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = -.761799;
		set_vf(&(p->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->vaxi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->vpos), 200, 500, 0, 0));
	}

	if ((p = hmapf_cylinder_c(&node_orgin, 80, 25, 111.5, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = -.761799;
		set_vf(&(p->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->vaxi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->vpos), 200, 500, 0, 0));
	}
	*buffer = p;
	surface_inv_hmapf(&t);

	nicenode = mk_partial(nicenode_message, p);
#endif /* DIAG_PARTIAL */
}

/* Wrap state increment tasks including attribs humdrum, rendering, sessions,
   and dialog.  Scene for this frame is viewed from viewpoint vpt for dpy0. */
void
regenerate_scene(vf_t *vpt)
{
	/* Sort hmaps and cue them for drawing. */
	sort_proc_hmaps(vpt);

	/* For each partial write selectf_a with ref's to hmaps to be transfered
	   and call buffer_maps_to_peer_partial(still in testing). */

	sync_sessions();

	/* Reading from selectf_b, tend to any incoming hmaps that code in
	   session.c has retrieved from other nodes.  This implies first
	   calling may include calling recieve_maps_from_peer_partial(still in
	   the works) for all partials while calling node_partial_dialog per
	   partial after resetting VRT_MASK_DIALOG_MODS and writing those to
	   selectf_a. */

	/* For now, simulate dialog introduced through modeling by transforming
	   some node-orgin dialog to hmap 15 then calling node_orgin_dialog. */
	hmapf_t **map = (hmapf_t **) selectf_a;
	static int recurrant = 0;
	if (!recurrant++) {
		*map = p_hmapf(15);
		select_t sel = { 0, 1, (hmapf_t **) map, 0, NULL };
		char a_char[] = "dialog: pass here and "
			"everything is published unless encrypted\n";
		add_dialog(&sel, a_char, strlen(a_char), 0);
		(*map)->attribs.sign |= (VRT_MASK_DIALOG | VRT_MASK_DIALOG_MODS);
		node_orgin_dialog(&sel);
	}

	/* Tend to timer issues.
	   notes:
	   given gnu timer lib giving back cycles fork_child_timer_callback()
	   disabled for now, see ifnode**.c for more */
	/* usleep(33400); */ /* @rsfreq 1000 fps = approx 27.8 to 28.6(+2.8%) */
}

/* Generate a connection point to merge partial vobspace in a peer to peers
   network.  Given desc, a max 80 chars + NULL reference to a description for a
   partial vobspace, and map, a refrence to an hmap delimiting partial space,
   return reference to partial.  note: Should mabye rewrite as a linked list. */
partial_t *
mk_partial(char *desc, hmapf_t *nodemap)
{
	int i, lval;
	partial_t *partial, **incpartial = (partial_t **) partial_generator_list;
	partial_t *swap, **incswap;
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	if ((partial = (partial_t *) malloc(sizeof(partial_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial struct\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Get an empty node-partial session name. */
	if ((lval = set_node_partial(&(partial->session))) != 0) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped addition of node-partial failed.\n"
			"Perhaps vrt_max_*_sessions needs adjustment...\n",
			__FILE__, __LINE__);
		free(partial);
		return NULL;
	} else
		__builtin_printf("generated node-partial %x\n",
			(int) partial->session);

	/* Rewrite partial_generator_list, empty or not, in a swap buffer. */
	if ((swap = (partial_t *) malloc((partials_count + 1) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc swap for "
			"partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incswap = *incpartial;
	*incswap = partial;

	/* Extend partial_generator_list allocation by space for one partial_t
	   reference. */
	free(partial_generator_list);
	partial_generator_list = NULL;
	if ((partial_generator_list = (partial_t *) malloc((partials_count + 1) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Write extended partial_generator_list. */
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incpartial = *incswap;
	*incpartial = *incswap;

	/* Form partial. */
	(*incpartial)->desc = desc;
	(*incpartial)->nodemap = nodemap;
	(*incpartial)->list = mk_partial_maps_list(&(partial->session), desc);
	add_to_partial_maps_list((*incpartial)->list, (*incpartial)->nodemap);
	(*incpartial)->nodemap->name = (*incpartial)->session | (session_t) (*incpartial)->nodemap->index;
	(*incpartial)->nodemap->attribs.sign |= (VRT_MASK_PARTIAL | VRT_MASK_PARTIAL_MODS);

	(&sel)->counta = select_partial_set((*incpartial)->list, (&sel)->seta);
	diag_selection(&sel);
	__builtin_printf("  description:\n%s\n  nodemap: %x\n",
		(*incpartial)->desc, (int) ((*incpartial)->nodemap)->name);

	free(swap);

	partials_count++;
	return partial;
}

/* Given a node-partial reference partial, send any hmaps collected therein to
   recycle.  Free any memory associated with the partial. */
void
rm_partial(partial_t *partial)
{
	int i;
	partial_t **incpartial = (partial_t **) partial_generator_list;
	partial_t *swap, **incswap;
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	/* Make a swap buffer of partials_count refrences. */
	if ((swap = (partial_t *) malloc((partials_count) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Put any partial reference not given in the swap buffer, while
	   configuring for recycle of given partial. */
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, incpartial++) {
		if (*incpartial != partial) {
			*incswap++ = *incpartial;
		} else {
			__builtin_printf(" removing: partial %x (%i/%i)\n%s\n",
				(int) *(partial->list->session),
				(((int) (incpartial - i) - (int) partial_generator_list) / (int) sizeof(partial_t *)) + 1,
				partials_count, (*incpartial)->desc);
			/* remove partial list leaving maps selected */
			(&sel)->counta = select_partial_set(partial->list, (&sel)->seta);
			rm_partial_maps_list(partial->list);
		}
	}

	/* Test that the partial given was found. */
	if ((int) (incswap - partials_count) == (int) partial_generator_list) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Partial given not found in "
			"partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Recycle hmaps for given partial. */
	recycle(&sel);

	/* Free partial and write reduced partial_generator_list from swap. */
	__builtin_printf(" freeing %x\n", (int) partial->session);
	free(partial);
	partials_count--;
	free(partial_generator_list);
	if ((partial_generator_list = (partial_t *) malloc(partials_count * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++)
		*incpartial++ = *incswap++;
	free(swap);
}

/* Given a name session, and an 80 char + NULL brief description desc, create a
   linked list construct for a node-partial providing reference to the list of
   hmaps residing therein.  Return a reference to the list. */
list_t *
mk_partial_maps_list(session_t *session, char *desc)
{
	list_t *list;
	if ((list = (list_t *) malloc(sizeof(list_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial_maps_list\n",
			__FILE__, __LINE__);
		abort();
	}
	list->session = session;
	list->last = NULL; /* first in list will have a null precursor */
	list->count = 0;
	__builtin_printf(" new partial maps list: %x\n", (int) *session);
	return list;
}

/* Remove the node-partial linked list construct list, and all of it's element
   references. */
void
rm_partial_maps_list(list_t *list)
{
	__builtin_printf(" dissolving list...\n");
	while (list->last != NULL) {
		subtract_from_partial_maps_list(list, list->last->map);
	}
	__builtin_printf(" freeing allocation for list\n");
	free(list);
}

/* Add an element reference to hmap map in the linked list construct list.
   Return given reference to map.  note: Mabye add a diagnostic check for
   duplication. */
listed_t *
add_to_partial_maps_list(list_t *list, hmapf_t *map)
{
	listed_t *listed;

	if ((listed = (listed_t *) malloc(sizeof(listed_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for a partial_maps_list entry\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->precursor = list->last;
	listed->map = map;
	list->last = listed;
	list->count++;

	if (listed->precursor) {
		__builtin_printf(" add_to_partial_maps_list: %i precursor %i\n",
		list->last->map->index, listed->precursor->map->index);
	} else {
		__builtin_printf(" add_to_partial_maps_list: %i %x precursor\n",
		list->last->map->index, (int) listed->precursor);
	}

	return listed;
}

/* Subtract element reference to hmap map from linked list construct list. */
int
subtract_from_partial_maps_list(list_t *list, hmapf_t *map)
{
	listed_t *current;
	listed_t *passed;

	if (list->last == NULL)
		return -1;

	current = list->last; /* start at end */
	passed = list->last; /* keep a back reference */

	__builtin_printf(" subtract_from_partial_maps_list: map %i, "
		"current %i, passed %i, last %i\n",
		map->index, current->map->index,
		passed->map->index, list->last->map->index);

	while (1) {
		if (current != NULL) {
			if (current->map->name == map->name) {
				break; /* match */
			}
			/* Increment. */
			passed = current;
			current = current->precursor;

			if (current) {
				__builtin_printf(" adjusted: current %i, " 
					"passed %i, last %i\n",
					current->map->index, passed->map->index,
					list->last->map->index);
			}
		} else
			return -1;
	}

	/* Subtract listed. */
	if (current == passed) {

		/* Last in list. */
		if (!current->precursor) {

			/* One element list. */
			list->last = NULL;

			__builtin_printf("  subtracting element from "
				"end of list: list now empty\n");

		} else {

			/* Last element where more than one. */
			__builtin_printf("  subtracting element from "
				"end of list: was %i, ",
				list->last->map->index);

			list->last = current->precursor; /* reset */

			__builtin_printf("end ref now %x\n",
				(int) list->last);
		}
	} else {

		/* Not last.  No need to reset last, needs relink. */
		__builtin_printf("  subtracting list element:"
			"\tpassed precursor ref %x (self)\n",
			(int) passed->precursor);

		passed->precursor = current->precursor; /* relink */

		__builtin_printf("\tpassing precursor ref %x to (previous)\n",
			(int) passed->precursor);
	}

	__builtin_printf(" freeing allocation for listed\n");
	free(current);
	list->count--;

	return 0;
}

/* Shut down all calling connections to local nodes at when, /w timed messages.
   Meanwhile disconnect from any called nodes.  Still in template form. */
void
close_vobspace(double when)
{
	/* node-orgin closeing */
	/* dlg(VRT_VOBSPACE,
		"this node will be closing in" dlgprntime(when - timer(now));
	   what = VRT_CLOSE;
	   fork_child_timer_callback(when, what);
	*/
	;
}

/* Callback for close_vobspace.  Still in template form. */
void
callback_close_vobspace(void)
{
	/* dlg(VRT_VOBSPACE_CLOSED, "vobspace" dlg(VRT_NODEDSC) "closing now");
	   rndr(VRT_VOBSPACE_CLOSED);
	   cancel_sessions(&sessions);
	*/
	;
}

/* Remove all partials currently defined then free all allocated memory. */
void
close_node_orgin(void)
{
	int i;
	partial_t **p = (partial_t **) partial_generator_list;

	for (i = partials_count; i > 0; i--)
		rm_partial(*p);

	free_vohspace_memory();
}

/* Resize vohspace memory allowing for size hmaps.  When supported, if
   keep_connected is non-zero, maintain connections while doing so.  When
   supported also allow for decreasing of hmap memory. */
int
resize_node_orgin(int size, int keep_connected)
{
	if (size < vrt_hmaps_max) {
		__builtin_fprintf(stderr,  "vrtater: "
			"Error: Reducing hmap memory currently unsupported\n");
		return(-1);
	} else {
		if (keep_connected)
			;
		else {
			close_node_orgin();
			vrt_hmaps_max = size;
			generate_node_orgin();
		}
		return(0);
	}
}

/* Temporary diagnostic to list partials to stdout.  If full is nonzero, also
   list all hmaps present per each partial. */
void
diag_ls_partials(int full)
{
	int i;
	partial_t **partial = (partial_t **) partial_generator_list;

	__builtin_printf(" partials: (list)\n");
	for (i = 0; i < partials_count; i++, partial++) {
		__builtin_printf("%s\n", (*partial)->desc);
		if (full)
			diag_hmaps_in_partial(&((*partial)->session));
	}
	if (!partials_count)
		__builtin_printf("--none--\n");
}

/* Temporary diagnostic to list hmaps in given partial session session. */
int
diag_hmaps_in_partial(session_t *session)
{
	int i;
	partial_t **partial;
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	partial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, partial++) {
		if (*session == (*partial)->session) {
			(&sel)->counta = select_partial_set((*partial)->list, (&sel)->seta);
			diag_selection(&sel);
			return (*partial)->list->count;
		}
	}
	return -1;
}

/* Temporary diagnostic to run test on keypress f. (!when diag-text input on).*/
void
diag_generator_key_f(void)
{
	diag_ls_partials(1);
	test_send_partial_changes();
	diag_ls_partials(1);
}

/* Temporary diagnostic to run test on keypress g. */
void
diag_generator_key_g(void)
{
	test_select_partial_set();
}

/* Temporary diagnostic to run test on keypress h. */
void
diag_generator_key_h(void)
{
	select_t s = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };
	__builtin_printf("there are %u attached hmaps\n", hmap_count());

	test_detach_all_partials();

	(&s)->counta = search_vohspace((&s)->seta, 0, VRT_MASK_ATTACHED);
	diag_selection(&s);
}

/* Diagnostic test: Place first two defined partials in selectf_a. */
void
test_select_partial_set(void)
{
	partial_t **partial;
	select_t sel = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };

	partial = (partial_t **) partial_generator_list;
	(&sel)->counta = select_partial_set(find_partial((&(*partial)->session)), (&sel)->seta);
	diag_selection(&sel);
	partial++;
	(&sel)->counta = select_partial_set(find_partial((&(*partial)->session)), (&sel)->seta);
	diag_selection(&sel);
}

/* Diagnostic test: See that session receive's all needed maps per a partial. */
void
test_send_partial_changes(void)
{
	int i;
	partial_t **partial;
	hmapf_t *map;
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	/* Nodemaps become listed when mk_partial is run
	   add some changed or new hmaps(for test add a cylinder). */
	partial = (partial_t **) partial_generator_list;
	if ((map = hmapf_cylinder_c(&node_orgin, 40, 25, 55.75, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = .761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
	}
	map->name = (*partial)->session | (session_t)map->index;
	add_to_partial_maps_list((*partial)->list, map);
	map->attribs.sign |= (VRT_MASK_PARTIAL_MODS | VRT_MASK_PARTIAL);

	/* Send all partials. */
	partial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, partial++) {
		(&sel)->counta = select_partial_set((*partial)->list, (&sel)->seta);
		buffer_maps_to_peer_partial(&((*partial)->session), &sel);
	}
}

/* Diagnostic test: Remove all partials currently defined. */
void
test_detach_all_partials(void)
{
	int i;
	partial_t **p = (partial_t **) partial_generator_list;

	for (i = partials_count; i > 0; i--)
		rm_partial(*p);
}

/* Diagnostic test: Run transform cp_hmapf with hmap ref arg's a to b. */
void
test_cphmap(hmapf_t *a, hmapf_t *b)
{
	hmapf_t **p;

	/* test copy_hmap() */
	p = (hmapf_t **) selectf_a;
	*p = a;
	p = (hmapf_t **) selectf_b;
	*p = b;
	select_t t = { 0, 0, (hmapf_t **) selectf_a, 0, (hmapf_t **) selectf_b};
	__builtin_printf("cphmaptest()\n");
	cp_hmapf(&t);
}

/* Diagnostic test: Run transforms to wrap and then unwrap map from vohspace to
   a kind of file form, then back. */
void
test_hmapwrap_unwrap(hmapf_t *map)
{
	hmapf_t **p = (hmapf_t **) selectf_a;
	*(p++) = map;
	*p = NULL;
	select_t s = { VRT_MASK_NULL_TERMINATED, 0, (hmapf_t **) selectf_a, 0, NULL};
	__builtin_printf("hmapwrapf() test\n");
	hmapwrapf(&s); /* from selection buf to file */
	__builtin_printf("hmapunwrapf() test\n");
	hmapunwrapf(&s); /* from file to vobspace or selection buf */
}

/* Tug support: The following are a list of transform wrappers vs. bus input. */
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
