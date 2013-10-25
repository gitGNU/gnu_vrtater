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

void init_selection_buffers(void);
void generate_vohspace(void);
void callback_close_vobspace(void);

/* Tests. */
partial_t *test_cue_determined_node(char *, complextimate_t *, hmapf_t **);
int test_form_determined_partial(partial_t *, hmapf_t *keymap, ptlrep_t *, char *url);
void test_add_repute(hmapf_t *);
void test_add_group(session_t *);
void test_add_member(session_t *, session_t *);
void test_select_partial_set(void);
void test_add_diagnostic_maps(session_t *, unsigned int);
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
	char filename[] = "/dev/zero";
	__builtin_printf("Reading vohspace from %s\n", filename); /* for now */
	mk_session_desc_list();
	hash_session_name(&node_orgin, ""); /* for now */
	__builtin_printf(" node-orgin set to %x\n", node_orgin);
	mk_partial_list();
	generate_vohspace();
	init_renderer();
	answer_accept = 0;
	init_sessions();

	return 0;
}

/* Add preconfigured hmaps and flexible's to node-orgin forming vohspace. */
void
generate_vohspace(void)
{
	int i;
	hmapf_t *map;
	vf_t d, portal = { 560, 560, 560, 969.94845 };
	session_t session = 0;

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
	__builtin_printf(" adding unformed flexible partial in node-orgin\n");

	/* Make a partial then remove it below on close_node. */
	hmapf_t **buf = (hmapf_t **) selectf_a;
	select_t t = { 0, 1, buf, 0, NULL};

	/* Outside map. */
	if ((map = hmapf_cylinder_c(&node_orgin, 80.5, 25, 112, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = -.761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), 200, 500, 0, 0));
		map->attribs.mode |= VRT_MASK_FIXED_FORM;
	}
	/* Inside map. */
	if ((map = hmapf_cylinder_c(&node_orgin, 80, 25, 111.5, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = -.761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), 200, 500, 0, 0));
		map->attribs.mode |= VRT_MASK_FIXED_FORM;
	}
	*buf = map;
	surface_inv_hmapf(&t);

	hash_session_name(&session, "");
	map->name = session | (0x0000ffff & map->name);
	mk_partial(&map, 1, VRT_FLEXIBLE);
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

/* Create a linked list construct for partial_list returning reference to an
   empty list of partials residing therein. */
void
mk_partial_list(void)
{
	partial_list = NULL;
	if ((partial_list = (partial_list_t *) malloc(sizeof(partial_list_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_list\n",
			__FILE__, __LINE__);
		abort();
	}
	partial_list->last = NULL;
	partial_list->count = 0;
}

/* Remove partial_list and all of it's element references. */
void
rm_partial_list(void)
{
	while (partial_list->last != NULL)
		rm_partial(partial_list->last);
	free(partial_list);
}

/* Generate a connection point to merge partial vobspace in a peer to peers
   network.  maps provides reference for nodemap hmap delimiting the partial's
   representation of space, followed by count-1 hmap references if any.  When
   a partial is made, any maps following nodemap always take on the nodemaps
   session regardless of their name.  When flexible is true, nodemap and groups
   are locally stored and maintained, and a session description does not exist
   until the allowing of logins is desired.  When the flexible flag is false,
   maps references the determined nodemap and session of a peer node.  Session
   description for peer node will exist and be called and cued.  In either
   case, reputations are maintained in any created partial, however, on a
   flexible partial they are stored with the partial, while on all determined
   partials, they are cumulatively stored with node-orgin. Successfull function
   completion return's reference to the partial or NULL apon any error.  note:
   For now any maps following nodemap are being ignored for simplicity. */
partial_t *
mk_partial(hmapf_t **maps, unsigned int count, int flexible)
{
	partial_t *listed = NULL;
	session_t session;

	session = (*maps)->name & 0xffff0000;
	__builtin_printf(" initializing node-partial with session %x\n",
		session);

	if ((listed = (partial_t *) malloc(sizeof(partial_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for partial_list entry\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->session = session;
	listed->ptlbits = 0xffffffff;
	listed->nodemap = *maps;
	if (flexible) {
		listed->ptlgrps = mk_ptlgrps_list(&(listed->session));
	} else {
		listed->ptlgrps = NULL;
	}
	listed->ptlreps = mk_ptlreps_list(&(listed->session));
	listed->ptlmaps = mk_ptlmaps_list(&(listed->session));
	listed->nodemap->attribs.mode |= VRT_MASK_NODEMAP;

	listed->precursor = partial_list->last;
	partial_list->last = listed;
	partial_list->count++;

	add_ptlmap(listed->ptlmaps, listed->nodemap);

	__builtin_printf("  nodemap: %x\n", listed->nodemap->name);

	return listed;
}

/* Given a node-partial reference partial, send any hmaps collected therein to
   recycle.  Free any memory associated with the partial. */
void
rm_partial(partial_t *partial)
{
	partial_t *current, *passed;
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	current = partial_list->last;
	passed = partial_list->last;
	while (1) {
		if (current != NULL) {
			if (current->session == partial->session)
				break;
			passed = current;
			current = current->precursor;
		} else
			return;
	}
	__builtin_printf(" removing partial %x\n",
		(int) current->session);

	/* Remove partial list, recycling maps therein. */
	rm_ptlgrps_list(partial->ptlgrps);
	rm_ptlreps_list(partial->ptlreps);
	(&sel)->counta = select_partial_set(partial->ptlmaps, (&sel)->seta);
	rm_ptlmaps_list(partial->ptlmaps);
	if (current == passed) {
		if (!current->precursor)
			partial_list->last = NULL;
		else
			partial_list->last = current->precursor;
	} else
		passed->precursor = current->precursor;
	free(current);
	partial_list->count--;
	recycle(&sel);
}

/* Shut down all calling connections to local nodes at when, with timed
   messages. */
void
close_vobspace(double when)
{
	/* dlg(VRT_VOBSPACE,
		"this node will be closing in" dlgprntime(when - timer(now));
	   what = VRT_CLOSE;
	   fork_child_timer_callback(when, what);
	*/
	callback_close_vobspace(); /* for now */
}

/* Remove all partials currently defined while informing one whom runs vrtater
   locally, and connected nodes. */
void
callback_close_vobspace(void)
{
	/* dlg(VRT_VOBSPACE_CLOSED, "vobspace" dlg(VRT_NODEDSC) "closing now");
	   rndr(VRT_VOBSPACE_CLOSED);
	*/
	rm_partial_list();
}

/* Free all allocated memory while informing one whom runs vrtater. */
void
close_node_orgin(void)
{
	/* dlg(VRT_NODEORGIN_CLOSED, "orgin" dlg(VRT_NODEDSC) "closing now");
	   rndr(VRT_VOBSPACE_CLOSED);
	*/
	delete_vohspace();
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
	partial_t *current, *passed;

	current = partial_list->last;
	passed = partial_list->last;
	if (current == NULL) {
		__builtin_printf("--no partials listed--\n");
		return;
	}
	while (current != NULL) {
		__builtin_printf("%x\n", (int) current->session);
		if (full)
			diag_hmaps_in_partial(current);
		passed = current;
		current = current->precursor;
	}
}

/* Temporary diagnostic to list hmaps partial returning count thereof. */
int
diag_hmaps_in_partial(partial_t *partial)
{
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	(&sel)->counta = select_partial_set(partial->ptlmaps, (&sel)->seta);
	diag_selection(&sel);
	return partial->ptlmaps->count;
}

/* Temporary diagnostic to run test on keypress f. (!if diag-text input on).*/
void
diag_generator_key_f(void)
{
	partial_t *cued = NULL;
	hmapf_t *welcome, *avatar8, *avatar8_hold, *chartoon;
	ptlrep_t *repute;
	int rval;

#ifdef DIAG_FEED_SESSION
	test_send_partial_changes();
#endif
#ifdef DIAG_GROUPS
	test_add_group(&(welcome->name));
	test_add_member(&(avatar8->name), &(welcome->name));
	test_add_member(&(chartoon->name), &(welcome->name));
#endif
#ifdef DIAG_DETERMINED_SESSION
	/* Pretend to log hwnode, a flexible remote node. */
	char url[] = "protocol://192.168.0.2"; /* hwnode */
	complextimate_t lcomplex; /* simulate local complexity */
	(&lcomplex)->hmap_count = 555;
	(&lcomplex)->tl_vdata = 4051;
	(&lcomplex)->tl_dialog = 358;
	hmapf_t **sela = (hmapf_t **) selectf_a; /* returned nodemap ref. */
	cued = test_cue_determined_node(url, &lcomplex, sela);

	/* Some usefull maps for testing logins. */
	test_add_diagnostic_maps(&(cued->session), 4);
	welcome = p_hmapf(22);
	welcome->attribs.mode |= VRT_MASK_GROUPMAP;
	avatar8 = p_hmapf(23);
	avatar8->attribs.sign |= VRT_MASK_KEYMAP;
	avatar8_hold = p_hmapf(24);
	avatar8_hold->attribs.sign |= VRT_MASK_HOLD;
	chartoon = p_hmapf(25);
	chartoon->attribs.sign |= VRT_MASK_KEYMAP;

	/* Locally stored with node-orgin for avatar8. */
	session_t lastkey = 0xb0de0008;
	session_t validkey = 0xface0008;
	session_t heldmaps = avatar8_hold->name;

	repute = add_ptlrep(cued->ptlreps, &lastkey, url);
	repute->bkpkey = validkey; /* assumed established */
	repute->holdkey = heldmaps;
	if ((rval = test_form_determined_partial(cued, avatar8, repute, url)) != 0)
		__builtin_printf("Error: %i on test_form_determined_partial\n",
			rval);
#endif
}

/* Temporary diagnostic to run test on keypress g. */
void
diag_generator_key_g(void)
{

#ifdef DIAG_HMAPWRAP
	hmapf_t *diagtext1 = p_hmapf(18); /* hmap to recieve text entry */
	char diagtextmsg[] = "Dialog test.\n";
	add_dialog(diagtext1, diagtextmsg, diagtext1->dialog_len, 0);
	test_hmapwrap_unwrap(diagtext1);
#endif
}

/* Temporary diagnostic to run test on keypress h. */
void
diag_generator_key_h(void)
{
	select_t s = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };
	(&s)->counta = search_vohspace((&s)->seta, 0, VRT_MASK_ATTACHED);
	diag_selection(&s);
}

/* Diagnostic test: Call flexible remote node for a determined nodemap and
   session description allowing for local complextimate at url.  Given nodemap
   then available through maps, make an unformed determined partial. */
partial_t *
test_cue_determined_node(char *url, complextimate_t *lcomplex, hmapf_t **maps)
{
	/* Simulate case where one whom is running vrtater calls peer node. */
	partial_t *partial = NULL;

	if ((call_session(url, lcomplex, maps)) != 0)
		return NULL;
	if (*maps) {
		/* Session was cued. */
		__builtin_printf(" adding unformed determined partial in "
			"node-orgin\n");
		partial = mk_partial(maps, 1, VRT_DETERMINED);
	} else
		__builtin_printf("Error: node failed to cue at %s.  Suspect "
			"DIAG_DETERMINED_SESSION_OFF.\n", url);

	return partial;
}

/* Diagnostic test: Accept determined session where keymap with keyname last
   has entered unformed partial desiring login. */
int
test_form_determined_partial(partial_t *forming, hmapf_t *keymap, ptlrep_t *repute, char *url)
{
	int rval;

	/* Using repute for keymap, form determined partial. */
	if ((rval = form_determined_session(forming, &(repute->key), &(keymap->name), url)) != 0) {
		__builtin_printf("Error: A disk error may have occured either "
			"locally or on the remote node.  Retrying login with "
			"a previously authenticated reputation key.\n");
		if ((rval = form_determined_session(forming, &(repute->bkpkey), &(keymap->name), url)) != 0) {
			__builtin_printf("Error: vrtater lost reputation sync "
			"due to a disk sync anomoly re-occurance.  Hopefully "
			"the maintainer of the node you were logging into can "
			"restore any held maps and your reputation on that  "
			"node.\n");
			return rval;
		}
	}
	add_ptlmap(forming->ptlmaps, keymap);

	return rval;
}

/* Diagnostic test: Add a group to locally hosted partial. */
void
test_add_group(session_t *groupmap_name)
{
	partial_t *partial;
	session_t partial_session = *groupmap_name & 0xffff0000;
	ptlgrp_t *group;

	if ((partial = find_partial(&partial_session)) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: rand has moved nicenode on test_groups\n",
			__FILE__, __LINE__);
		return;
	}
	__builtin_printf(" adding group in partial %x vs. groupmap %x\n",
		(int) partial->session, (int) *groupmap_name);
	group = add_ptlgrp(partial->ptlgrps, groupmap_name);
}

/* Diagnostic test: Add a member to locally hosted group. */
void
test_add_member(session_t *sign_in, session_t *groupmap_name)
{
	partial_t *partial;
	session_t partial_session = *groupmap_name & 0xffff0000;
	ptlgrp_t *group;

	if ((partial = find_partial(&partial_session)) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: rand has moved nicenode on test_groups\n",
			__FILE__, __LINE__);
		return;
	}
	if ((group = find_group(partial->ptlgrps, groupmap_name)) == NULL) {
		__builtin_printf("Group with groupmap %x not found\n",
			(int) groupmap_name);
		return;
	}
	__builtin_printf(" adding member for group with groupmap %x\n",
		(int) *groupmap_name);
	add_ptlmbr(group->members, sign_in);
	find_member(group->members, sign_in);
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

/* Diagnostic test: Place quantity test hmaps in partial session
   partial_session. */
void
test_add_diagnostic_maps(session_t *partial_session, unsigned int quantity)
{
	int i;
	hmapf_t *map;
	partial_t *partial;

	if ((partial = find_partial(partial_session)) == NULL) {
		__builtin_printf("Cant find session to add maps to.\n");
		return;
	}

	/* Nodemaps become listed when mk_partial is run.  For testing add n
	   more hmaps to partial, new maps representing partial changes. */
	for (i = 0; i < quantity; i++) {
		if ((map = hmapf_cylinder_c(&node_orgin, 40, 25, 55.75, 0)) != NULL) {
			map->ang_spd = 0;
			map->ang_dpl = .761799;
			set_vf(&(map->vvel), 0, 0, 0, 0);
			form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
			form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
		} else {
			__builtin_printf("Ran out of maps while running "
				"test_add_diagnostic_maps\n");
			return;
		}

		map->name = partial->session | map->index;
		add_ptlmap(partial->ptlmaps, map);
		map->attribs.sign |= (VRT_MASK_PARTIAL_MODS | VRT_MASK_PARTIAL);

		__builtin_printf("  added hmap %x to node-partial %x\n",
			map->name, partial->session);
	}
}

/* Diagnostic test: Send all relevant partial maps in vohspace to respective
   connected nodes. */
void
test_send_partial_changes(void)
{
	partial_t *current, *passed;
	select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

	current = partial_list->last;
	passed = partial_list->last;
	while (current != NULL) {
		(&sel)->counta = select_partial_set(current->ptlmaps, (&sel)->seta);
		buffer_maps_to_peer_partial(&(current->session), &sel);
		passed = current;
		current = current->precursor;
	}
}

/* Diagnostic test: Remove all partials currently defined. */
void
test_detach_all_partials(void)
{
	partial_t *current, *passed;

	current = partial_list->last;
	passed = partial_list->last;
	while (current != NULL) {
		rm_partial(current);
		passed = current;
		current = current->precursor;
	}
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
