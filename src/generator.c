/* generator.c: Create/restore initial conditions.  Tend to vobspace feedback.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "generator.h"
#include "global.h"
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

session_t node_orgin = 0;

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
void test_add_repute(hmapf_t *);
void test_groups(void);
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
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_a\n",
			__FILE__, __LINE__);
		abort();
	}
	selectf_b = NULL;
	if ((selectf_b = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_b\n",
			__FILE__, __LINE__);
		abort();
	}
}

/* Form the basis for hmap and further node interfacing. */
int
generate_node_orgin(void)
{
	init_selection_buffers();
	init_vohspace();
	__builtin_printf("Reading vohspace from\n"); /* for now */
	set_node_orgin(&node_orgin, ""); /* for now */
	__builtin_printf("node-orgin set to %x\n", node_orgin);
	partial_list = NULL;
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
	hmapf_t *map;
	vf_t d, portal = { 560, 560, 560, 969.94845 };

	set_vf(&d, 0, .01, -.03, 0);
	form_mag_vf(&d); 

	/* Here hmaps will be added to node-orgin vs. config file.  For now
	   add the following given vrt_hmaps_max. */
	for (i = 0; i < 1; i++)
		if ((map = hmapf_icosahedron_c(&node_orgin, .01)) != NULL)
			nportf(map, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 2; i++)
		if ((map = hmapf_cube_c(&node_orgin, 3, 3, 3)) != NULL)
			nportf(map, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 6; i++)
		if ((map = hmapf_icosahedron_c(&node_orgin, 2)) != NULL)
			nportf(map, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 10; i++)
		if ((map = hmapf_cube_c(&node_orgin, 100, 100, 100)) != NULL)
			nportf(map, sum_vf(&d, &portal, &portal));
	for (i = 0; i < 1; i++)
		if ((map = hmapf_cylinder_c(&node_orgin, 8, 25, 12, 0)) != NULL)
			nportf(map, sum_vf(&d, &portal, &portal));

#ifdef DIAG_PARTIAL
	/* Make 2 partials then remove them below on close_node. */
	hmapf_t **buffer = (hmapf_t **) selectf_a;
	select_t t = { 0, 1, buffer, 0, NULL};

	/* Outer hmap for node-orgin. */
	if ((map = hmapf_cylinder_c(&node_orgin, 80.5, 25, 112, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = .761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
		map->attribs.mode |= VRT_MASK_FIXED_FORM;
	}

	/* Inner hmap, to be attached to other nodes. */
	if ((map = hmapf_cylinder_c(&node_orgin, 80, 25, 111.5, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = .761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
		map->attribs.mode |= VRT_MASK_FIXED_FORM;
	}
	*buffer = map;
	surface_inv_hmapf(&t);

	thenode = mk_partial(thenode_message, map);

	/* Next partial. */
	if ((map = hmapf_cylinder_c(&node_orgin, 80.5, 25, 112, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = -.761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), 200, 500, 0, 0));
		map->attribs.mode |= VRT_MASK_FIXED_FORM;
	}

	if ((map = hmapf_cylinder_c(&node_orgin, 80, 25, 111.5, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = -.761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), 200, 500, 0, 0));
		map->attribs.mode |= VRT_MASK_FIXED_FORM;
	}
	*buffer = map;
	surface_inv_hmapf(&t);

	nicenode = mk_partial(nicenode_message, map);
#endif /* DIAG_PARTIAL */
}

/* Wrap state increment tasks including attribs humdrum, rendering, sessions,
   and dialog.  Scene for this frame is viewed from viewpoint vpt for dpy0. */
void
regenerate_scene(vf_t *vpt)
{
	hmapf_t **map;

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

	/* For now, apply a test simulating dialog introduced through modeling
	   by transforming some node-orgin dialog to hmap 15 then calling
	   node_orgin_dialog. */
	static int recurrant = 0;
	if (!recurrant++) {
		map = (hmapf_t **) selectf_a;
		*map = p_hmapf(15);
		char chars[] = "dialog: pass here and "
			"everything is published unless encrypted\n";
		add_dialog(*map, chars, 0, 0);
		(*map)->attribs.sign |= (VRT_MASK_DIALOG | VRT_MASK_DIALOG_MODS);
		select_t sel = { 0, 1, map, 0, NULL };
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
   partial vobspace, and map, a refrence to an hmap delimiting partial space.
   Return reference to partial.  note: Should mabye rewrite as a linked list. */
partial_t *
mk_partial(char *desc, hmapf_t *nodemap)
{
	int i, lval;
	partial_t *partial, **incpartial = (partial_t **) partial_list;
	partial_t *swap, **incswap;

	if ((partial = (partial_t *) malloc(sizeof(partial_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial struct\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Get an empty node-partial session name, for now. */
	partial->session = 0;
	if ((lval = set_node_partial(&(partial->session), "")) != 0) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped addition of node-partial failed.\n"
			"Perhaps vrt_max_*_sessions needs adjustment...\n",
			__FILE__, __LINE__);
		free(partial);
		return NULL;
	} else
		__builtin_printf("generated node-partial %x\n",
			partial->session);

	/* Rewrite partial_list, empty or not, in a swap buffer. */
	if ((swap = (partial_t *) malloc((partials_count + 1) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc swap for "
			"partial_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_list;
	for (i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incswap = *incpartial;
	*incswap = partial;

	/* Extend partial_list allocation by space for one partial_t
	   reference. */
	free(partial_list);
	partial_list = NULL;
	if ((partial_list = (partial_t *) malloc((partials_count + 1) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc partial_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Write extended partial_list. */
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_list;
	for (i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incpartial = *incswap;
	*incpartial = *incswap;

	/* Form partial. */
	(*incpartial)->desc = desc;
	(*incpartial)->nodemap = nodemap;
	(*incpartial)->complexity.hmap_count = 1;
	(*incpartial)->complexity.tl_vdata = nodemap->vmap_total * sizeof(vf_t);
	(*incpartial)->complexity.tl_dialog = nodemap->dialog_len * sizeof(int);
	(*incpartial)->ptlreps = mk_ptlreps_list(&(partial->session));
	(*incpartial)->ptlgrps = mk_ptlgrps_list(&(partial->session));
	(*incpartial)->ptlmaps = mk_ptlmaps_list(&(partial->session));
	add_ptlmap((*incpartial)->ptlmaps, (*incpartial)->nodemap);

	(*incpartial)->nodemap->name = (*incpartial)->session | (*incpartial)->nodemap->index;
	(*incpartial)->nodemap->attribs.sign |= (VRT_MASK_PARTIAL | VRT_MASK_PARTIAL_MODS);

	__builtin_printf("  description:\n%s\n  nodemap: %x\n",
		(*incpartial)->desc, ((*incpartial)->nodemap)->name);

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
	partial_t **incpartial = (partial_t **) partial_list;
	partial_t *swap, **incswap;
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	/* Make a swap buffer of partials_count refrences. */
	if ((swap = (partial_t *) malloc((partials_count) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc partial_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Put any partial reference not given in the swap buffer, while
	   configuring for recycle of given partial. */
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_list;
	for (i = 0; i < partials_count; i++, incpartial++) {
		if (*incpartial != partial) {
			*incswap++ = *incpartial;
		} else {
			__builtin_printf(" removing: partial %x (%i/%i)\n%s\n",
				*(partial->ptlmaps->session),
				(((int) (incpartial - i) - (int) partial_list) / sizeof(partial_t *)) + 1,
				partials_count, (*incpartial)->desc);
			/* remove partial list leaving maps selected */
			(&sel)->counta = select_partial_set(partial->ptlmaps, (&sel)->seta);
			rm_ptlmaps_list(partial->ptlmaps);
			rm_ptlgrps_list(partial->ptlgrps);
			rm_ptlreps_list(partial->ptlreps);
		}
	}

	/* Test that the partial given was found. */
	if ((int) (incswap - partials_count) == (int) partial_list) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Partial given not found in "
			"partial_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* Recycle hmaps for given partial. */
	recycle(&sel);

	/* Free partial and write reduced partial_list from swap. */
	free(partial);
	partials_count--;
	free(partial_list);
	if ((partial_list = (partial_t *) malloc(partials_count * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc partial_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **) swap;
	incpartial = (partial_t **) partial_list;
	for (i = 0; i < partials_count; i++)
		*incpartial++ = *incswap++;
	free(swap);
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
	partial_t **partial = (partial_t **) partial_list;

	for (i = partials_count; i > 0; i--)
		rm_partial(*partial);

	free_vohspace_memory();
}

/* Resize vohspace memory allowing for size hmaps.  When supported, if
   keep_connected is non-zero, maintain connections while doing so.  When
   supported also allow for decreasing of hmap memory.  note: A low memory
   warning vs. complextimate would be usefull, then possibly resorting to
   balance_filter. */
int
resize_node_orgin(int size, int keep_connected)
{
	if (size < vrt_hmaps_max) {
		__builtin_fprintf(stderr, "vrtater: "
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
	partial_t **partial = (partial_t **) partial_list;

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

	partial = (partial_t **) partial_list;
	for (i = 0; i < partials_count; i++, partial++) {
		if (*session == (*partial)->session) {
			(&sel)->counta = select_partial_set((*partial)->ptlmaps, (&sel)->seta);
			diag_selection(&sel);
			return (*partial)->ptlmaps->count;
		}
	}
	return -1;
}

/* Temporary diagnostic to run test on keypress f. (!if diag-text input on).*/
void
diag_generator_key_f(void)
{
	test_send_partial_changes();
	test_add_repute(p_hmapf(21));
	test_add_repute(p_hmapf(24));
	test_groups();
}

/* Temporary diagnostic to run test on keypress g. */
void
diag_generator_key_g(void)
{
	hmapf_t *diagtext1 = p_hmapf(15); /* hmap to recieve text entry */
	char diagtextmsg[] = "Dialog test.\n";
	add_dialog(diagtext1, diagtextmsg, diagtext1->dialog_len, 0);
	test_hmapwrap_unwrap(diagtext1);
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

/* Diagnostic test: Add a reputation, diagnose.  Partial hosted locally. */
void
test_add_repute(hmapf_t *map)
{
	ptlrep_t *reputed;
	partial_t *partial;
	session_t locally_hosted_partial = 0x50940000; /* nicenode */
	ptlreps_list_t *list;
	char url[] = "protocol://192.168.0.2";

	map->attribs.sign |= VRT_MASK_KEYMAP;

	partial = find_partial(&locally_hosted_partial);
	list = partial->ptlreps;

	reputed = add_ptlrep(list, &(map->name), url);

	reputed = find_repute(list, &(map->name), VRT_MAPKEY);

#ifdef DIAG_GROUPS
	__builtin_printf("  key %x\n", (int) reputed->key);
	__builtin_printf("  url %s\n", reputed->url);
#endif /* DIAG_GROUPS */
}

/* Diagnostic test: Add a reputation, diagnose.  Partial hosted locally. */
void
test_groups(void)
{
	partial_t *partial;
	session_t locally_hosted_partial = 0x50940000;
	ptlgrp_t *group;
	session_t groupmap_name = 0x50940019;
	session_t member1 = 0x50940018;
	session_t member2 = 0x50940015;

	partial = find_partial(&locally_hosted_partial);
	__builtin_printf(" adding group in partial %x vs. groupmap %x\n",
		(int) partial->session, (int) groupmap_name);
	group = add_ptlgrp(partial->ptlgrps, &groupmap_name);
	__builtin_printf(" adding members for group with groupmap %x\n",
		(int) groupmap_name);
	add_ptlmbr(group->members, &member1);
	find_member(group->members, &member1);
	add_ptlmbr(group->members, &member2);
	find_member(group->members, &member2);
}
/* Diagnostic test: Place first two defined partials in selectf_a. */
void
test_select_partial_set(void)
{
	partial_t **partial;
	select_t sel = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };

	partial = (partial_t **) partial_list;
	(&sel)->counta = select_partial_set((find_partial((&(*partial)->session))->ptlmaps), (&sel)->seta);
	diag_selection(&sel);
	partial++;
	(&sel)->counta = select_partial_set((find_partial((&(*partial)->session))->ptlmaps), (&sel)->seta);
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
	partial = (partial_t **) partial_list;
	if ((map = hmapf_cylinder_c(&node_orgin, 40, 25, 55.75, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = .761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
	}
	map->name = (*partial)->session | map->index;
	add_ptlmap((*partial)->ptlmaps, map);
	map->attribs.sign |= (VRT_MASK_PARTIAL_MODS | VRT_MASK_PARTIAL);
	(*partial)->complexity.hmap_count += 1;
	(*partial)->complexity.tl_vdata += map->vmap_total * sizeof(vf_t);
	(*partial)->complexity.tl_dialog += map->dialog_len * sizeof(int);

	__builtin_printf("added hmap %x to node-partial %x\n",
		map->name, (*partial)->session);

	/* For testing groups add another cylinder). */
	if ((map = hmapf_cylinder_c(&node_orgin, 40, 25, 55.75, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = .761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
	}
	map->name = (*partial)->session | map->index;
	add_ptlmap((*partial)->ptlmaps, map);
	map->attribs.sign |= (VRT_MASK_PARTIAL_MODS | VRT_MASK_PARTIAL);
	(*partial)->complexity.hmap_count += 1;
	(*partial)->complexity.tl_vdata += map->vmap_total * sizeof(vf_t);
	(*partial)->complexity.tl_dialog += map->dialog_len * sizeof(int);

	__builtin_printf("added hmap %x to node-partial %x\n",
		map->name, (*partial)->session);

	/* Send all partials. */
	partial = (partial_t **) partial_list;
	for (i = 0; i < partials_count; i++, partial++) {
		(&sel)->counta = select_partial_set((*partial)->ptlmaps, (&sel)->seta);
		buffer_maps_to_peer_partial(&((*partial)->session), &sel);
	}
}

/* Diagnostic test: Remove all partials currently defined. */
void
test_detach_all_partials(void)
{
	int i;
	partial_t **p = (partial_t **) partial_list;

	for (i = partials_count; i > 0; i--)
		rm_partial(*p);
}

/* Diagnostic test: Run transform cp_hmapf with hmap ref arg's a to b. */
void
test_cphmap(hmapf_t *a, hmapf_t *b)
{
	hmapf_t **p;

	/* test cp_hmapf */
	p = (hmapf_t **) selectf_a;
	*p = a;
	p = (hmapf_t **) selectf_b;
	*p = b;
	select_t t = { 0, 0, (hmapf_t **) selectf_a, 0, (hmapf_t **) selectf_b};
	__builtin_printf("cp_hmapf test\n");
	cp_hmapf(&t);
}

/* Diagnostic test: Run transforms to wrap and then unwrap map from vohspace to
   a kind of file form, then back. */
void
test_hmapwrap_unwrap(hmapf_t *map)
{
	int i, *buffer = NULL; /* ref for allocated .vrtater int data */
	hmapf_t **testmap, **maps_in = (hmapf_t **) selectf_a;
	hmapf_t **maps_out = (hmapf_t **) selectf_b;
	select_t s = { 0, 0, maps_in, 0, maps_out };
	hmapf_t *extra;
	vf_t extra_location = { 0, 10000, 0, 10000 };
	session_t testsession = 0x80860000;

	*(maps_in++) = map;
	(&s)->counta += 1;
	if ((extra = hmapf_icosahedron_c(&node_orgin, .01)) != NULL)
		nportf(extra, &extra_location);
	*maps_in = extra;

	__builtin_printf("\nhmapwrapf test\n");

#ifdef DIAG_STF
	/* Single to file. */
	__builtin_printf("Single to file\n");
	(&s)->counta = 1;
	char *filename = "temp.hmap";
	hmapwrapf(&s, 0, filename, NULL);
#endif
#ifdef DIAG_CTF
	/* Compounded to file. */
	__builtin_printf("Compounded to file\n");
	(&s)->counta = 2;
	char *filename = "temp.hmap";
	hmapwrapf(&s, VRT_MASK_OPT_COMPOUNDED, filename, NULL);
#endif
#ifdef DIAG_STB
	/* Single to buffer.  This would be used by code in session.c */
	__builtin_printf("Single to buffer\n");
	(&s)->counta = 1;
	hmapwrapf(&s, 0, NULL, (int **) &buffer);
#endif
#ifdef DIAG_CTB
	/* Compounded to buffer.  This would be used by code in session.c */
	__builtin_printf("Compounded to buffer\n");
	(&s)->counta = 2;
	hmapwrapf(&s, VRT_MASK_OPT_COMPOUNDED, NULL, (int **) &buffer);
#endif

	__builtin_printf("\nhmapunwrapf test\n");

#ifdef DIAG_FF
	/* From file. */
	__builtin_printf("From file\n");
	hmapunwrapf(&s, NULL, filename, NULL);
	/* Test that hmap is selected. */
	testmap = (&s)->setb;
	for (i = 0; i < (&s)->countb; i++, testmap++) {
		(*testmap)->ang_spd += .05;
		__builtin_printf("map %i index %i\n", i, (*testmap)->index);
	}
#endif
#ifdef DIAG_FFS
	/* From file using given session name. */
	__builtin_printf("From file using given session name\n");
	hmapunwrapf(&s, (session_t *) &testsession, filename, NULL);
	/* Test that hmap is selected. */
	testmap = (&s)->setb;
	for (i = 0; i < (&s)->countb; i++, testmap++) {
		(*testmap)->ang_spd += .05;
		__builtin_printf("map %i index %i\n", i, (*testmap)->index);
	}
#endif
#ifdef DIAG_FB
	/* From buffer (allocated int data). */
	__builtin_printf("From buffer\n");
	/* Use reported session name vs. any in data. */
	hmapunwrapf(&s, (session_t *) &testsession, NULL, buffer);
	/* Test that hmap is selected. */
	testmap = (&s)->setb;
	for (i = 0; i < (&s)->countb; i++, testmap++) {
		(*testmap)->ang_spd += .05;
		__builtin_printf("map %i index %i\n", i, (*testmap)->index);
	}
#endif
	if (buffer)
		free(buffer);
}

/* Tug support: The following are a list of transform wrappers vs. bus input. */
void
generator_join_hmaps(void)
{
	;
}

void
generator_intersection(void)
{
	;
}
