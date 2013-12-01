/* generator.c: Create/restore initial conditions.  Tend to vobspace feedback.
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
#include "stock.h"

#ifdef VRT_X_SUPPORT
#include "dialogX11.h"
#endif /* VRT_X_SUPPORT */

#ifdef VRT_RENDERER_GL
#include "rendergl.h"
#endif /* VRT_RENDER_GL */

unsigned int vrt_hmaps_max; /* extern */
session_t node_orgin = { { 0, 0, 0 }, 0 };
complextimate_t *orgin_cmplxt;

void init_selection_buffers(void);
void generate_orgin_cmplxt(void);
void generate_vohspace(void);
void callback_close_vobspace(void);

/* Tests. */
ptlgrp_t *test_add_group(session_t *);
ptlmbr_t *test_add_member(session_t *, session_t *);
void test_select_partial_set(void);
void test_send_partial_changes(void);
void test_cphmap(hmapf_t *, hmapf_t *);
void test_hmapwrap_unwrap(hmapf_t *);

/* Allocate for a and b selection buffers. */
void
init_selection_buffers(void)
{
  selectf_a = NULL;
  if ((selectf_a = (hmapf_t *) malloc(vrt_hmaps_max * sizeof(hmapf_t))) == NULL) {
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
struct session_desc *
generate_node_orgin(void)
{
  struct session_desc *orgin_desc;
  char filename[] = "/dev/zero";
  session_t null = { { 0, 0, 0 }, 0 };

  init_selection_buffers();
  init_vohspace();
  __builtin_printf("Reading vohspace from %s\n", filename);
  hash_session_name(&node_orgin, "");
  __builtin_printf(" node-orgin set to (%x %x %x)\n",
    (&node_orgin)->hash.h, (&node_orgin)->hash.m,
    (&node_orgin)->hash.l);
  generate_orgin_cmplxt();
  mk_session_desc_list();
  orgin_desc = add_session_desc(&node_orgin, &null, &null, 0, NULL,
  "node-orgin", orgin_cmplxt, NULL, NULL);
  mk_partial_list();
  generate_vohspace();
  init_renderer();
  session_nodemask = 0;
  login_cmplxt_max = 0xffffff;
  init_sessions();

  return orgin_desc;
}

/* Generate a complextimate for node-orgin. */
void
generate_orgin_cmplxt(void)
{
  if ((orgin_cmplxt = (complextimate_t *) malloc(sizeof(complextimate_t))) == NULL) {
    __builtin_fprintf(stderr,  "vrtater:%s:%d: "
      "Error: Could not malloc for orgin_complextimate\n",
      __FILE__, __LINE__);
    abort();
  }
  orgin_cmplxt->hmap_count = 0;
  orgin_cmplxt->tl_vdata = 0;
  orgin_cmplxt->tl_dialog = 0;
}

/* Return total size of all hmaps in cmplxt using sizeof returns as a basis. */
unsigned int
calc_cmplxt(complextimate_t *cmplxt)
{
  return sizeof(hmapf_t) * cmplxt->hmap_count + sizeof(int) * (cmplxt->tl_vdata + cmplxt->tl_dialog);
}

/* Add preconfigured hmaps and flexible's to node-orgin forming vohspace. */
void
generate_vohspace(void)
{
  int i;
  hmapf_t *map;
  vf_t d, portal = { 560, 560, 560, 969.94845 };

  set_vf(&d, 0, 10, -30, 0);
  form_mag_vf(&d);

  /* Here hmaps will be added to node-orgin vs. config file.  For now
     add the following given vrt_hmaps_max. */
  for (i = 0; i < 1; i++)
    if ((map = hmapf_icosahedron_c(&node_orgin, .01)) != NULL)
      nportf(map, sum_vf(&d, &portal, &portal), 1);
  for (i = 0; i < 2; i++)
    if ((map = hmapf_cuboid_c(&node_orgin, 3, 3, 3)) != NULL)
      nportf(map, sum_vf(&d, &portal, &portal), 1);
  for (i = 0; i < 6; i++)
    if ((map = hmapf_icosahedron_c(&node_orgin, 2)) != NULL)
      nportf(map, sum_vf(&d, &portal, &portal), 1);
  for (i = 0; i < 10; i++)
    if ((map = hmapf_cuboid_c(&node_orgin, 100, 100, 100)) != NULL)
      nportf(map, sum_vf(&d, &portal, &portal), 1);
  for (i = 0; i < 1; i++)
    if ((map = hmapf_cylinder_c(&node_orgin, 8, 25, 12, 0)) != NULL)
      nportf(map, sum_vf(&d, &portal, &portal), 1);
}

/* Wrap state increment tasks including attribs humdrum, rendering, sessions,
   and dialog.  Scene for this frame is viewed from viewpoint vpt for dpy0. */
void
regenerate_scene(vf_t *vpt)
{
  hmapf_t **map;

  /* Sort hmaps in vohspace and sequence them for proc_hmapf where sequence is
     determinate, yet when no new hmaps are added during a frametime(state
     increment) treated as an unsorted co-domain of refrences after bijection
     into range during tframe vs. tframe+1 composed of hmaps within a lod
     envelope then rendered.  hmaps holding any of fov* arrive twice as args to
     proc_hmapf, first as the full set of those with a lod value that is then
     masked by vrt_mask_fov_lod (will be added when more fov's are) and then
     here when sort_proc_hmaps is called.  For each of fov*, vpt* may then be
     set before rendering those when proc_hmapf calls render_hmapf. */
  sort_proc_hmaps(vpt);

  /* For each partial write selectf_a with ref's to hmaps to be
     transfered and call send_maps(in testing). */
  sync_sessions();

  /* Reading from selectf_b, tend to any incoming hmaps that code in session.c
     has retrieved from other nodes. This may include calling 
     receive_maps_from_peer_partial(still in the works) beforehand, for all
     partials, while calling node_partial_dialog per partial after resetting
     VRT_MASK_DIALOG_MODS and writing those to selectf_a.  note: Bits to always
     turn off on inbound maps are VRT_MASK_LOD_INF, ... */

#ifdef DIAG_DIALOG
  /* For now, apply a test simulating dialog introduced through modeling
     by transforming some node-orgin dialog to hmap 15 then calling
     node_orgin_dialog. */
  static int recurrant = 0;
  if (!recurrant++) {
    map = (hmapf_t **) selectf_a;
    *map = p_hmapf(15);
    char chars[] = "dialog: GNU Free Software vrtater pre-alpha "
      "on Savannah...\n\n";
    add_dialog(*map, chars, 0, 0);
    (*map)->attribs.sign |= (VRT_MASK_DIALOG | VRT_MASK_DIALOG_MODS);
    select_t sel = { 0, 1, map, 0, NULL };
    node_orgin_dialog(&sel);
  }
#endif

  /* Tend to timer issues.  note: Given gnu timer lib giving back cycles
     fork_child_timer_callback().  see ifnode**.c for more */
  /* usleep(33400); */ /* @rsfreq 1000 fps = aprox 27.8 to 28.6(+2.8%) */
}

/* Create linked list construct to maintain all created nodes. */
void
mk_partial_list(void)
{
  partials = NULL; /* partials declare is in partial.h */
  if ((partials = (struct partial_list *) malloc(sizeof(struct partial_list))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for partial node list\n",
      __FILE__, __LINE__);
    abort();
  }
  partials->last = NULL;
  partials->count = 0;
}

/* Remove linked list construct and all created nodes therein. */
void
rm_partial_list(void)
{
  while (partials->last != NULL)
    rm_partial(partials->last);
  free(partials);
}

/* Generate a connection point to merge partial vobspace in a peer to peers
   network.  If session non-zero, this will be a continuing partial as a
   flexible takes it's nodemaps name.  maps provides reference for nodemap hmap
   delimiting the partial's representation of space, followed by count hmap
   references for making a flexible initially, if any.  Return refrence to
   partial or NULL apon any error.  note: When a partial node is made, any maps
   following nodemap always take on the derived session regardless of their
   name.  When session is NULL, nodemap and groups are stored locally and
   maintained, and a session description exist's with only the session hash
   and a oneliner as reference, until the allowing of vrtlogins is desired.
   When session is non-NULL, maps initially refrences the nodemap sent from a
   flexible node, and thus has the session of the node one is logging into, yet
   this gets overwritten for the continuing node created, providing unique
   sessions for both sides of partial.  In this case session description for
   flexible node that sent the nodemap will already exist, providing
   continuity.  Reputations  are maintained in any created node through
   loginkeys, however, on a flexible node these are stored with the partial,
   while on all continuing nodes, they maintained in the continuing yet
   cumulatively stored alongside node-orgin.  Also, for now any maps following
   nodemap are ignored for simplicity. */
struct partial *
mk_partial(session_t *session, hmapf_t **maps, unsigned int count, complextimate_t *cmplxt)
{
  struct partial *listed = NULL;

  if ((listed = (struct partial *) malloc(sizeof(struct partial))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for partials entry\n",
      __FILE__, __LINE__);
    abort();
  }
  listed->precursor = partials->last;
  partials->last = listed;
  partials->count++;

  listed->ptlbits = 0xffffffff;
  if (session) {
    cp_mapname(session, &(listed->session));
    listed->ptlgrps = NULL;
  } else {
    set_nodename(&(listed->session), &((*maps)->name));
    listed->ptlgrps = mk_ptlgrps_list(&(listed->session));
  }
  listed->reputed = mk_ptlrepute_list(&(listed->session));
  listed->ptlmaps = mk_ptlmaps_list(&(listed->session));
  listed->nodemap = *maps;
  listed->nodemap->attribs.mode |= VRT_MASK_NODEMAP;
  cmplxt->hmap_count += count;
  cmplxt->tl_vdata += (*maps)->vmap_total;
  cmplxt->tl_dialog += (*maps)->dialog_len;
  add_ptlmap(listed->ptlmaps, listed->nodemap);

  __builtin_printf("  nodemap: (%x %x %x) %i\n",
    listed->nodemap->name.hash.h, listed->nodemap->name.hash.m,
    listed->nodemap->name.hash.l, listed->nodemap->name.seq);

  if (session)
    __builtin_printf(" new continuing with session name "
      "(%x %x %x)\n", session->hash.h, session->hash.m,
      session->hash.l);
  else
    __builtin_printf(" new flexible with session name "
      "(%x %x %x)\n", (*maps)->name.hash.h,
      (*maps)->name.hash.m, (*maps)->name.hash.l);

  return listed;
}

/* Given a node-partial reference partial, send any hmaps collected therein to
   recycle.  Free any memory associated with the partial. */
void
rm_partial(struct partial *ptl)
{
  struct partial *current, *passed;
  select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

  current = partials->last;
  passed = partials->last;
  while (1) {
    if (current != NULL) {
      if (match_mapname(&(current->session), &(ptl->session)))
        break;
      passed = current;
      current = current->precursor;
    } else
      return;
  }
  __builtin_printf(" removing partial (%x %x %x)\n",
    current->session.hash.h, current->session.hash.m,
    current->session.hash.l);

  /* Remove partial list, recycling maps therein. */
  rm_ptlgrps_list(ptl->ptlgrps);
  rm_ptlrepute_list(ptl->reputed);
  (&sel)->counta = select_partial_set(ptl->ptlmaps, (&sel)->seta);
  rm_ptlmaps_list(ptl->ptlmaps);
  if (current == passed) {
    if (!current->precursor)
      partials->last = NULL;
    else
      partials->last = current->precursor;
  } else
    passed->precursor = current->precursor;
  free(current);
  partials->count--;
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
diag_ls_partials(void)
{
  struct partial *current, *passed;

  current = partials->last;
  passed = partials->last;
  if (current == NULL) {
    __builtin_printf("--no partials listed--\n");
    return;
  }
  while (current != NULL) {
    diag_hmaps_in_partial(current);
    passed = current;
    current = current->precursor;
  }
}

/* Temporary diagnostic to list hmaps partial returning count thereof. */
int
diag_hmaps_in_partial(struct partial *ptl)
{
  select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

  (&sel)->counta = select_partial_set(ptl->ptlmaps, (&sel)->seta);
  diag_selection(&sel);
  return ptl->ptlmaps->count;
}

/* Temporary diagnostic to run test on keypress f. (!if diag-text input on).*/
void
diag_generator_key_f(void)
{
#ifdef DIAG_FEED_SESSION
  test_send_partial_changes();
#endif

#ifdef DIAG_GROUPS_0FF
  test_add_group(&(welcome->name));
  test_add_member(&(avatar555->name), &(welcome->name));
  test_add_member(&(chartoon->name), &(welcome->name));
#endif
}

/* Temporary diagnostic to run test on keypress g. */
void
diag_generator_key_g(void)
{
#ifdef DIAG_DIALOG
  hmapf_t *diagtext1 = p_hmapf(18); /* hmap to receive text entry */
  char diagtextmsg[] = "Dialog test.\n";
  add_dialog(diagtext1, diagtextmsg, diagtext1->dialog_len, 0);
#endif

#ifdef DIAG_HMAPWRAP
  test_hmapwrap_unwrap(p_hmapf(18));
#endif
}

/* Temporary diagnostic to run test on keypress h. */
void
diag_generator_key_h(void)
{
#ifdef DIAG_SEARCH_VOHSPACE
  select_t s = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };
  (&s)->counta = search_vohspace((&s)->seta, 0, VRT_MASK_ATTACHED);
  diag_selection(&s);
#endif
}

/* Diagnostic test: Add a group to local flexible partial. */
ptlgrp_t *
test_add_group(session_t *groupmap_name)
{
  struct partial *ptl;
  session_t session;

  cp_session(groupmap_name, &session);
  if ((ptl = find_partial(&session)) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: test_add_group\n",
      __FILE__, __LINE__);
    return NULL;
  }
  __builtin_printf(" adding group in partial (%x %x %x) vs. groupmap "
    "(%x %x %x) %i\n", (&ptl->session)->hash.h,
    (&ptl->session)->hash.m, (&ptl->session)->hash.l,
    groupmap_name->hash.h, groupmap_name->hash.m,
    groupmap_name->hash.l, groupmap_name->seq);

  return add_ptlgrp(ptl->ptlgrps, groupmap_name);
}

/* Diagnostic test: Add a member to locally hosted group. */
ptlmbr_t *
test_add_member(session_t *sign_in, session_t *groupmap_name)
{
  struct partial *ptl;
  session_t session;
  ptlgrp_t *group;

  cp_session(&session, groupmap_name);
  if ((ptl = find_partial(&session)) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: session non-existant in test_add_member\n",
      __FILE__, __LINE__);
    abort();
  }
  if ((group = find_group(ptl->ptlgrps, groupmap_name)) == NULL) {
    __builtin_printf("Group with groupmap (%x %x %x) %i not "
      "found\n", groupmap_name->hash.h,
      groupmap_name->hash.m, groupmap_name->hash.l,
      groupmap_name->seq);
    return NULL;
  }
  __builtin_printf(" adding member for group with groupmap (%x %x %x) "
    "%i\n", groupmap_name->hash.h, groupmap_name->hash.m,
    groupmap_name->hash.l, groupmap_name->seq);

  return add_ptlmbr(group->members, sign_in);
}

/* Diagnostic test: Place first two defined partials in selectf_a. */
void
test_select_partial_set(void)
{
  struct partial **ptl;
  select_t sel = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };

  ptl = (struct partial **) partials;
  (&sel)->counta = select_partial_set((find_partial((&(*ptl)->session))->ptlmaps), (&sel)->seta);
  diag_selection(&sel);
  ptl++;
  (&sel)->counta = select_partial_set((find_partial((&(*ptl)->session))->ptlmaps), (&sel)->seta);
  diag_selection(&sel);
}

/* Diagnostic test: Place n test hmaps to node represented by session, adding
   them to complextimate cmplxt, and leaving them selected in selectf_a. */
void
test_add_maps(unsigned int n, int mapstock, session_t *session, vf_t *portal, select_t *sel, complextimate_t *cmplxt)
{
  int i;
  struct partial *node;
  hmapf_t **map;
  vf_t d = { 10., 0., 0., 10. }; /* ... */

  /* some stuff is broken till ... */

  if ((node = find_partial(session)) == NULL) {
    __builtin_printf("Cant find node with given session to add "
      "maps to.\n");
    return;
  }

  map = sel->seta;
  /* For testing add n more hmaps to node with session, new maps
     representing node changes. */
  for (i = 0; i < n; i++) {
    if ((*map = hmapf_cylinder_c(&node_orgin, 40, 25, 55.75, 0)) != NULL) {
      (*map)->ang_spd = 0;
      (*map)->ang_dpl = .761799;
      set_vf(&((*map)->vvel), 0, 0, 0, 0);
      form_mag_vf(set_vf(&((*map)->vaxi), -.5, 1, 0, 0));
      /* for now */
      form_mag_vf(set_vf(&((*map)->vpos), -200, 500, 0, 0));
    } else {
      __builtin_printf("Ran out of maps while running "
        "test_add_diagnostic_maps\n");
      return;
    }
    cp_session(&(node->session), &((*map)->name));
    add_ptlmap(node->ptlmaps, *map);
#ifdef DIAG_FLEXIBLE_ENABLE
    nportf(*map, portal, 0);
#endif
#ifdef DIAG_UNDEFINED
    nportf(*map, sum_vf(portal, &d, &d), 0);
    tele_magz_vf(&d, .8, &d); /* ... */
#endif
    cmplxt->hmap_count++;
    cmplxt->tl_vdata += (*map)->vmap_total;
    cmplxt->tl_dialog += (*map)->dialog_len;
    (*map)->attribs.sign |= (VRT_MASK_PARTIAL_MODS | VRT_MASK_PARTIAL);

    __builtin_printf("  hmap now (%x %x %x) %i changed node"
      "\n", (*map)->name.hash.h, (*map)->name.hash.m,
      (*map)->name.hash.l, (*map)->name.seq);
  }
}

/* Diagnostic test: Send all relevant partial maps in vohspace to respective
   connected nodes. */
void
test_send_partial_changes(void)
{
  struct partial *current, *passed;
  select_t sel = { 0, 0, (hmapf_t **) selectf_a, 0, NULL };

  current = partials->last;
  passed = partials->last;
  while (current != NULL) {
    (&sel)->counta = select_partial_set(current->ptlmaps, (&sel)->seta);
    send_maps(&(current->session), &sel);
    passed = current;
    current = current->precursor;
  }
}

/* Diagnostic test: Run transform cp_hmapf copying a to b. */
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
   a .vrtater, then back. */
void
test_hmapwrap_unwrap(hmapf_t *map)
{
  int i, sz, *buffer = NULL; /* ref for allocated .vrtater int data */
  hmapf_t **testmap, **maps_in = (hmapf_t **) selectf_a;
  hmapf_t **maps_out = (hmapf_t **) selectf_b;
  select_t s = { 0, 0, maps_in, 0, maps_out };
  hmapf_t *extra;
  vf_t extra_location = { 0, 10000, 0, 10000 };
  session_t testsession = { { 0xa37507, 0x5519, 0x3a9 }, 0 } ;
  *(maps_in++) = map;
  (&s)->counta += 1;
  if ((extra = hmapf_icosahedron_c(&node_orgin, .01)) != NULL)
    nportf(extra, &extra_location, 1);
  *maps_in = extra;

  __builtin_printf("\nhmapwrapf test\n");

#ifdef DIAG_STF
  __builtin_printf("Single to file\n");
  (&s)->counta = 1;
  char *filename = "temp.hmap";
  sz = hmapwrapf(&s, 0, filename, NULL);
  __builtin_printf("returned after wrap %i\n", sz);
#endif

#ifdef DIAG_CTF
  __builtin_printf("Compounded to file\n");
  (&s)->counta = 2;
  char *filename = "temp.hmap";
  sz = hmapwrapf(&s, VRT_MASK_OPT_COMPOUNDED, filename, NULL);
  __builtin_printf("returned after wrap %i\n", sz);
#endif

#ifdef DIAG_STB
  __builtin_printf("Single to buffer\n");
  (&s)->counta = 1;
  sz = hmapwrapf(&s, 0, NULL, (int **) &buffer);
  __builtin_printf("returned after wrap %i\n", sz);
#endif

#ifdef DIAG_CTB
  __builtin_printf("Compounded to buffer\n");
  (&s)->counta = 2;
  sz = hmapwrapf(&s, VRT_MASK_OPT_COMPOUNDED, NULL, (int **) &buffer);
  __builtin_printf("returned after wrap %i\n", sz);
#endif

  __builtin_printf("\nhmapunwrapf test\n");

#ifdef DIAG_FF
  __builtin_printf("From file\n");
  sz = hmapunwrapf(&s, NULL, filename, NULL);
  __builtin_printf("returned after unwrap %i\n", sz);
  /* Test that hmap is selected. */
  testmap = (&s)->setb;
  for (i = 0; i < (&s)->countb; i++, testmap++) {
    (*testmap)->ang_spd += .05;
    __builtin_printf("map %i index %i\n", i, (*testmap)->index);
  }
#endif

#ifdef DIAG_FFS
  __builtin_printf("From file using given session name\n");
  sz = hmapunwrapf(&s, (session_t *) &testsession, filename, NULL);
  __builtin_printf("returned after unwrap %i\n", sz);
  /* Test that hmap is selected. */
  testmap = (&s)->setb;
  for (i = 0; i < (&s)->countb; i++, testmap++) {
    (*testmap)->ang_spd += .05;
    __builtin_printf("map %i index %i\n", i, (*testmap)->index);
  }
#endif

#ifdef DIAG_FB
  __builtin_printf("From buffer\n");
  /* Use session name testsession set above vs. any in data. */
  sz = hmapunwrapf(&s, (session_t *) &testsession, NULL, buffer);
  __builtin_printf("returned after unwrap %i\n", sz);
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

/* Tug support: Following are a list of transform wrappers vs. bus input. */
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
