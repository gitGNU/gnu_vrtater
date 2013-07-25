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
session_t node_orgin;

/* selection buffer */
hmapf_t *selectf_a; /* external */
hmapf_t *selectf_b; /* external */

void init_selection_buffers(void);
void generate_vohspace(void);
void callback_close_vobspace(void);

/* diag partial */
#define DIAG_PARTIAL
#ifdef DIAG_PARTIAL
char thenode_message[] = "thenode.";
partial_t *thenode;
char nicenode_message[] = "the really nice node.";
partial_t *nicenode;
#endif

/* diag immediate feedback tests */
void test_select_partial_set(void);
void test_send_partial_changes(void);
void test_detach_all_partials(void);

/* diag */
void cphmaptest(hmapf_t *, hmapf_t *);
void hmapwrap_unwraptst(hmapf_t *);

void
init_selection_buffers(void)
{
	/* allocate hmapf selection buffers */
	selectf_a = NULL;
	if((selectf_a = (hmapf_t *) malloc((vrt_hmaps_max + 1) * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_a\n",
			__FILE__, __LINE__);
		abort();
	}
	selectf_b = NULL;
	if((selectf_b = (hmapf_t *) malloc((vrt_hmaps_max + 1) * sizeof(hmapf_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for selectf_b\n",
			__FILE__, __LINE__);
		abort();
	}
}

int
generate_node_orgin(void)
{
	int lval = -1;

	/* init generator */
	init_selection_buffers();

	/* init attribs.  hmap arrays, pointers thereto */
	init_vohspace();

	/* get session name defining node-orgin space */
	node_orgin = 0;
	if((lval = set_node_orgin(&node_orgin)) != 0) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Failed to generate node-orgin.\n",
			__FILE__, __LINE__);
		return lval;
	}
	__builtin_printf("generated node-orgin %x\n", (int)node_orgin);

	/* init partial list */
	partial_generator_list = NULL;
	partials_count = 0;

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

	/* add hmaps to node-orgin vs. cfg. should add test for vohspace full.
	   from file */
	/* ... */
	/* for now */
	for(i = 0; i < 1; i++)
		if((p = hmapf_icosahedron_c(&node_orgin, .01)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 2; i++)
		if((p = hmapf_cube_c(&node_orgin, 3, 3, 3)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 6; i++)
		if((p = hmapf_icosahedron_c(&node_orgin, 2)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 10; i++)
		if((p = hmapf_cube_c(&node_orgin, 100, 100, 100)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));
	for(i = 0; i < 1; i++)
		if((p = hmapf_cylinder_c(&node_orgin, 10, 25, 13.5, 0)) != NULL)
			nportf(p, sum_vf(&d, &ptl, &ptl));

#ifdef DIAG_PARTIAL
	/* make 2 partials then remove them below on close_node() */
	hmapf_t **buffer;

	/* outer hmap for node-orgin */
	if((p = hmapf_cylinder_c(&node_orgin, 80.5, 25, 112, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = .761799;
		set_vf(&(p->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->vpos), -200, 500, 0, 0));
	}

	/* inner hmap, to be attached to other nodes */
	if((p = hmapf_cylinder_c(&node_orgin, 80, 25, 111.5, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = .761799;
		set_vf(&(p->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->vpos), -200, 500, 0, 0));
	}
	buffer = (hmapf_t **)selectf_a;
	*buffer = p;
	select_t t = { 0, 0, (hmapf_t **)selectf_a, 0, NULL};
	surface_inv_hmapf(&t);

	thenode = mk_partial(thenode_message, p);

	/* next partial */
	if((p = hmapf_cylinder_c(&node_orgin, 80.5, 25, 112, 0)) != NULL) {
		p->ang_spd = 0;
		p->ang_dpl = -.761799;
		set_vf(&(p->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(p->vaxi), .5, 1, 0, 0));
		form_mag_vf(set_vf(&(p->vpos), 200, 500, 0, 0));
	}

	if((p = hmapf_cylinder_c(&node_orgin, 80, 25, 111.5, 0)) != NULL) {
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

void
regenerate_scene(vf_t *vpt)
{
	/* sort hmaps and cue them for drawing */
	sort_proc_hmaps(vpt);

	/* cycle network
	   fill selectf_a with ref's to hmaps to be transfered */
	session_sync();

	/* enumerate incoming hmaps that session.c retrieves from other nodes
	   and then references in selectf_b.  meanwhile reset any MODS bits and
	   write references for incoming hmaps with new dialog into selectf_a */
	/* ... */

	/* reading from selectf_a, send any new dialog off the network to
	   dialog().  any dialog introduced by modeling transforms is sent to
	   dialog() directly, and will have VRT_MASK_HMAP_MODELING set high.
	   along with any dialog representing the person running node-orgin and
	   written thru transform.c by code in dialog.c, these are the sum of
	   the ways that new dialog will be introduced.  for now, simulate case
	   where some node-orgin dialog, newly created by modeling transforms, is
	   generated in hmap 15 */
	hmapf_t **p = (hmapf_t **)selectf_a;
	static int recurrant = 0;
	if(!recurrant++) {
		*p = p_hmapf(15);
		select_t s = { 0, 1, (hmapf_t **)p, 0, NULL };
		char a_char[] = "dialog: pass here and "
			"everything is published unless encrypted\n";
		add_dialog(&s, a_char, strlen(a_char), 0);
		(*p)->attribs.sign |= (VRT_MASK_DIALOG | VRT_MASK_DIALOG_MODS);
		dialog(&s);
	}

	/* timer
	   issue:
	   given gnu timer lib giving back cycles fork_child_timer_callback()
	   disabled for now, see ifnode**.c for more */
	/* usleep(33400); */ /* @rsfreq 1000 fps = approx 27.8 to 28.6(+2.8%) */
}

/* partial space
   generate connection point to merge node vobspaces in a peer to peers network.
   given desc, a max 80 chars + NULL reference to a description for a partial
   vobspace, and map, a refrence to an hmap delimiting the partial space.
   return reference to partial.  note: should mabye rewrite as a linked list */
partial_t *
mk_partial(char *desc, hmapf_t *nodemap)
{
	int i, lval;
	partial_t *partial, **incpartial = (partial_t **)partial_generator_list;
	partial_t *swap, **incswap;
	select_t sel = { 0, 0, (hmapf_t **)selectf_a, 0, NULL };

	if((partial = (partial_t *) malloc(sizeof(partial_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial struct\n",
			__FILE__, __LINE__);
		abort();
	}

	/* get an empty node-partial session name */
	if((lval = set_node_partial(&(partial->session))) != 0) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Attemped addition of node-partial failed.\n"
			"Perhaps vrt_max_*_sessions needs adjustment...\n",
			__FILE__, __LINE__);
		free(partial);
		return NULL;
		
	} else
		__builtin_printf("generated node-partial %x\n",
			(int)partial->session);

	/* rewrite partial_generator_list, empty or not, in a swap buffer */
	if((swap = (partial_t *) malloc((partials_count + 1) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc swap for "
			"partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partial_generator_list;
	for(i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incswap = *incpartial;
	*incswap = partial;

	/* extend partial_generator_list alloc by space for one partial_t ref */
	free(partial_generator_list);
	partial_generator_list = NULL;
	if((partial_generator_list = (partial_t *) malloc((partials_count + 1) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* write extended partial_generator_list */
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partial_generator_list;
	for(i = 0; i < partials_count; i++, incpartial++, incswap++)
		*incpartial = *incswap;
	*incpartial = *incswap;

	/* form partial */
	(*incpartial)->desc = desc;
	(*incpartial)->nodemap = nodemap;
	(*incpartial)->list = mk_partial_maps_list(&(partial->session), desc);
	add_to_partial_maps_list((*incpartial)->list, (*incpartial)->nodemap);
	(*incpartial)->nodemap->name =
		(*incpartial)->session |
		(session_t)(*incpartial)->nodemap->index;
	(*incpartial)->nodemap->attribs.sign |=
		(VRT_MASK_PARTIAL | VRT_MASK_PARTIAL_MODS);

	select_partial_set((*incpartial)->list, &sel);
	diag_selection(&sel);
	__builtin_printf("  description:\n%s\n  nodemap: %x\n",
		(*incpartial)->desc, (int)((*incpartial)->nodemap)->name);

	free(swap);

	partials_count++;
	return partial;
}

/* given a node-partial reference partial, send any hmaps collected therein to
   recycle().  free any memory associated with the partial */
void
rm_partial(partial_t *partial)
{
	int i;
	partial_t **incpartial = (partial_t **)partial_generator_list;
	partial_t *swap, **incswap;
	select_t sel = { 0, 0, (hmapf_t **)selectf_a, 0, NULL };

	/* make a swap buffer of partials_count refrences */
	if((swap = (partial_t *) malloc((partials_count) * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* put any partial reference not given in the swap buffer, while
	   configuring for recycle() of given partial */
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partial_generator_list;
	for(i = 0; i < partials_count; i++, incpartial++) {
		if(*incpartial != partial) {
			*incswap++ = *incpartial;
		} else {
			__builtin_printf(" removing: partial %x (%i/%i)\n%s\n",
				(int)partial->list->session,
				(((int)(incpartial - i) -
				(int)partial_generator_list) /
				(int)sizeof(partial_t *)) + 1,
				partials_count, (*incpartial)->desc);
			/* remove partial list leaving maps selected */
			(&sel)->counta = select_partial_set(partial->list, &sel);
			rm_partial_maps_list(partial->list);
		}
	}

	/* test that the partial given was found */
	if((int)(incswap - partials_count) == (int)partial_generator_list) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Partial given not found in "
			"partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}

	/* recycle any selected partial hmaps */
	recycle(&sel);

	/* free partial and write reduced partial_generator_list from swap */
	__builtin_printf(" freeing %x\n", (int)partial->list->session);
	free(partial);
	partials_count--;
	free(partial_generator_list);
	if((partial_generator_list = (partial_t *)
		malloc(partials_count * sizeof(partial_t *))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc partial_generator_list\n",
			__FILE__, __LINE__);
		abort();
	}
	incswap = (partial_t **)swap;
	incpartial = (partial_t **)partial_generator_list;
	for(i = 0; i < partials_count; i++)
		*incpartial++ = *incswap++;
	free(swap);
}

list_t *
mk_partial_maps_list(session_t *session, char *desc)
{
	list_t *list;
	if((list = (list_t *) malloc(sizeof(list_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for partial_maps_list\n",
			__FILE__, __LINE__);
		abort();
	}
	list->session = session;
	list->last = NULL; /* first in list will have a null precursor */
	list->count = 0;
	__builtin_printf(" new partial maps list: %x\n", (int)*session);
	return list;
}

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

/* mabye add a diagnostic check for duplication */
listed_t *
add_to_partial_maps_list(list_t *list, hmapf_t *map)
{
	listed_t *listed;

	if((listed = (listed_t *) malloc(sizeof(listed_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for a partial_maps_list entry\n",
			__FILE__, __LINE__);
		abort();
	}
	listed->precursor = list->last;
	listed->map = map;
	list->last = listed;
	list->count++;

	if(listed->precursor) {
		__builtin_printf(" add_to_partial_maps_list: %i precursor %i\n",
		list->last->map->index, listed->precursor->map->index);
	} else {
		__builtin_printf(" add_to_partial_maps_list: %i %x precursor\n",
		list->last->map->index, (int)listed->precursor);
	}

	return listed;
}

int
subtract_from_partial_maps_list(list_t *list, hmapf_t *map)
{
	listed_t *current;
	listed_t *passed;

	if(list->last == NULL)
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
			/* increment */
			passed = current;
			current = current->precursor;

			if(current) {
				__builtin_printf(" adjusted: current %i, " 
					"passed %i, last %i\n",
					current->map->index,
					passed->map->index,
					list->last->map->index);
			}
		} else
			return -1;
	}

	/* subtract listed */
	if (current == passed) {

		/* last in list */
		if (!current->precursor) {

			/* one element list */
			list->last = NULL;

			__builtin_printf("  subtracting element from end of list: "
			"list now empty\n");

		} else {

			/* last element where more than one */
			__builtin_printf("  subtracting element from end of list: "
				"was %i, ",
				list->last->map->index);

			list->last = current->precursor; /* last matched, reset */

			__builtin_printf("end ref now %x\n",
				(int)list->last);

		}
	} else {

		/* not last, no need to reset last, needs relink */
		__builtin_printf("  subtracting list element:"
			"\tpassed precursor ref %x (self)\n",
			(int)passed->precursor);

		passed->precursor = current->precursor; /* relink */

		__builtin_printf("\tpassing precursor ref %x to (previous)\n",
			(int)passed->precursor);
	}

	__builtin_printf(" freeing allocation for listed\n");
	free(current);
	list->count--;

	return 0;
}

/* depreciated
   reference hmaps in partial_session as null terminated list in selectf_a */
void
select_hmaps_in_partial(session_t *partial_session)
{
	int i;
	hmapf_t *map, **selection;

	map = a_hmaps;
	selection = (hmapf_t **)selectf_a;

	for(i = 0; i < vrt_hmaps_max; i++, map++)
		if((map->name & 0xffff0000) == *partial_session)
			*(selection++) = map;
	*selection = NULL;
}

/* diagnostics for partial */
void
diag_ls_partials(int full)
{
	int i;
	partial_t **partial = (partial_t **)partial_generator_list;

	__builtin_printf(" partials: (list)\n");
	for(i = 0; i < partials_count; i++, partial++) {
		__builtin_printf("%s\n", (*partial)->desc);
		if(full)
			diag_hmaps_in_partial(&((*partial)->session));
	}
	if(!partials_count)
		__builtin_printf("--none--\n");
}

int
diag_hmaps_in_partial(session_t *session)
{
	int i;
	partial_t **partial;
	select_t sel = { 0, 0, (hmapf_t **)selectf_a, 0, NULL };

	partial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, partial++) {
		if (*session == (*partial)->session) {
			select_partial_set((*partial)->list, &sel);
			diag_selection(&sel);
			return (*partial)->list->count;
		}
	}
	return -1;
}

/* run diagnostic tests on keypress, f, g, h */
void
diag_generator_key_f(void)
{
	diag_ls_partials(1);
	test_send_partial_changes();
	diag_ls_partials(1);
}

void
diag_generator_key_g(void)
{
	test_select_partial_set();
}

void
diag_generator_key_h(void)
{
	select_t s = {  0, 0, (hmapf_t **)selectf_a, 0, NULL };
	__builtin_printf("there are %u attached hmaps\n", hmap_count());

	test_detach_all_partials();

	(&s)->counta = search_vohspace((&s)->seta, 0, VRT_MASK_ATTACHED);
	diag_selection(&s);
}

/* diagnostic tests */
void
test_select_partial_set(void)
{
	partial_t **partial;
	select_t sel = {  0, 0, (hmapf_t **)selectf_a, 0, NULL };

	partial = (partial_t **) partial_generator_list;
	select_partial_set(find_partial((&(*partial)->session)), &sel);
	diag_selection(&sel);
	partial++;
	select_partial_set(find_partial((&(*partial)->session)), &sel);
	diag_selection(&sel);
}

void
test_send_partial_changes(void)
{
	int i;
	partial_t **partial;
	hmapf_t *map;
	select_t sel = { 0, 0, (hmapf_t **)selectf_a, 0, NULL };

	/* nodemaps become listed when mk_partial is run
	   add some changed or new hmaps(for test add a cylinder) */
	partial = (partial_t **) partial_generator_list;
	if((map = hmapf_cylinder_c(&node_orgin, 40, 25, 55.75, 0)) != NULL) {
		map->ang_spd = 0;
		map->ang_dpl = .761799;
		set_vf(&(map->vvel), 0, 0, 0, 0);
		form_mag_vf(set_vf(&(map->vaxi), -.5, 1, 0, 0));
		form_mag_vf(set_vf(&(map->vpos), -200, 500, 0, 0));
	}
	map->name = (*partial)->session | (session_t)map->index;
	add_to_partial_maps_list((*partial)->list, map);
	map->attribs.sign |= (VRT_MASK_PARTIAL_MODS | VRT_MASK_PARTIAL);

	/* send all partials */
	partial = (partial_t **) partial_generator_list;
	for (i = 0; i < partials_count; i++, partial++) {
		(&sel)->counta = select_partial_set((*partial)->list, &sel);
		buffer_maps_to_peer_partial(&((*partial)->session), &sel);
	}
}

void
test_detach_all_partials(void)
{
	int i;
	partial_t **p = (partial_t **)partial_generator_list;

	for(i = partials_count; i > 0; i--) {
		rm_partial(*p);
	}
}

/* node maintainance */
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
close_node_orgin(void)
{
	int i;
	partial_t **p = (partial_t **)partial_generator_list;

	for(i = partials_count; i > 0; i--) {
		rm_partial(*p);
	}

	free_vohspace_memory();
}

/* resize vohspace hmap memory (while maintaining connections: todo) */
int
resize_node_orgin(int size, int keep_connected)
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
			close_node_orgin();
			vrt_hmaps_max = size;
			generate_node_orgin();
		}
		return(0);
	}
}

/* hapmap file support */
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
