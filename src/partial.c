/* partial.c: Calls returning partial set for hmaps.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <stdlib.h>
#include <stdio.h>
#include "partial.h"
#include "progscope.h"

/* Return partial reference vs. session or NULL if not found. */
struct partial *
find_partial(session_t *session)
{
  struct partial *current, *passed;

  current = partials->last;
  passed = partials->last;
  while (1) {
    if (current != NULL) {
      if (match_mapname(&(current->session), session))
        return current;
      passed = current;
      current = current->precursor;
    } else
      return NULL;
  }
}

/* Copy any refrences for partial in list to allocation referenced by maps. */
int
select_partial_set(struct ptlmap_list *list, hmapf_t **maps)
{
  int i;
  struct ptlmap *listed;
  hmapf_t **map;

  map = maps;
  listed = list->last;
  for (i = 0; i < list->count; i++, map++) {
    *map = listed->map;
    listed = listed->precursor;
  }

  return i;
}

/* Create a linked list construct for session returning reference to an empty
   list of hmaps residing therein. */
struct ptlmap_list *
mk_ptlmap_list(session_t *session)
{
  struct ptlmap_list *list = NULL;
  if ((list = (struct ptlmap_list *) malloc(sizeof(struct ptlmap_list))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for ptlmap_list\n",
      __FILE__, __LINE__);
    abort();
  }
  list->last = NULL; /* first in list will have a NULL precursor */
  list->count = 0;

  list->session = session;

  return list;
}

/* Remove list and all of it's element references. */
void
rm_ptlmap_list(struct ptlmap_list *list)
{
  while (list->last != NULL)
    subtract_ptlmap(list, list->last->map);
  free(list);
}

/* Add an element reference to partial hmap map in the linked list construct
   list.  Set VRT_MASK_PARTIAL.  Also set VRT_MASK_PARTIAL_MODS informing code
   in session.c that this map should be sync'd with remote nodes.  Return
   reference to element listed. */
struct ptlmap *
add_ptlmap(struct ptlmap_list *list, hmapf_t *map)
{
  struct ptlmap *listed = NULL;

  if ((listed = (struct ptlmap *) malloc(sizeof(struct ptlmap))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for ptlmap_list entry\n",
      __FILE__, __LINE__);
    abort();
  }
  listed->precursor = list->last;
  list->last = listed;
  list->count++;

  listed->map = map;
  listed->map->attribs.sign |= (VRT_MASK_PARTIAL | VRT_MASK_PARTIAL_MODS);

  return listed;
}

/* Subtract linked list element referencing hmap map from list.  Unset
   VRT_MASK_PARTIAL.  */
void
subtract_ptlmap(struct ptlmap_list *list, hmapf_t *map)
{
  struct ptlmap *current, *passed;

  current = list->last; /* start at end */
  passed = list->last; /* keep a back reference */
  while (1) {
    if (current != NULL) {
      if (match_mapname(&(current->map->name), &(map->name)))
        break;
      passed = current;
      current = current->precursor;
    } else
      return;
  }
  if (current == passed) {
    if (!current->precursor)
      list->last = NULL;
    else
      list->last = current->precursor; /* reset */
  } else
    passed->precursor = current->precursor; /* relink */
  map->attribs.sign &= (0xffffffff ^ VRT_MASK_PARTIAL);
  free(current);
  list->count--;
}

/* Try to find the linked list element for sign_in in list.  Return reference
   to element or NULL if not found. */
struct modlg *
find_member(struct modlg_list *list, session_t *sign_in)
{
  struct modlg *current, *passed;

  current = list->last;
  passed = list->last;
  while (1) {
    if (current != NULL) {
      if (match_mapname(&(current->sign_in), sign_in))
      {
        __builtin_printf("  member with sign-in (%x %x %x) %i was found\n",
          current->sign_in.hash.h, current->sign_in.hash.m,
          current->sign_in.hash.l, current->sign_in.seq);
        return current;
      }
      passed = current;
      current = current->precursor;
    } else
      return NULL;
  }
}

/* Create a linked list construct for group returning reference to an empty
   list of members residing therein. */
struct modlg_list *
mk_modlg_list(struct ptlgrp *group)
{
  struct modlg_list *list = NULL;
  if ((list = (struct modlg_list *) malloc(sizeof(struct modlg_list))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for modlg_list\n",
      __FILE__, __LINE__);
    abort();
  }
  list->last = NULL;
  list->count = 0;

  group->holdmaps = list;

  return list;
}

/* Remove list and all of it's elements and element references from memory. */
void
rm_modlg_list(struct modlg_list *list)
{
  while (list->last != NULL)
    subtract_modlg(list, list->last);
  free(list);
}

/* Add an element reference to sign_in in linked list construct list.  Return
   reference to element listed. */
struct modlg *
add_modlg(struct modlg_list *list, session_t *sign_in)
{
  struct modlg *listed = NULL;

  if ((listed = (struct modlg *) malloc(sizeof(struct modlg))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for modlg_list entry\n",
      __FILE__, __LINE__);
    abort();
  }
  __builtin_printf("  member with sign_in (%x %x %x) %i added...\n",
    sign_in->hash.h, sign_in->hash.m, sign_in->hash.l, sign_in->seq);
  listed->precursor = list->last;
  list->last = listed;
  list->count++;

  listed->sign_in = *sign_in;

  return listed;
}

/* Subtract linked list element referenced by member. */
void
subtract_modlg(struct modlg_list *list, struct modlg *member)
{
  struct modlg *current, *passed;

  current = list->last;
  passed = list->last;
  while (1) {
    if (current != NULL) {
      if (match_mapname(&(current->sign_in), &(member->sign_in)))
        break;
      passed = current;
      current = current->precursor;
    } else
      return;
  }
  __builtin_printf("  member with sign-in (%x %x %x) %i left group...\n",
    current->sign_in.hash.h, current->sign_in.hash.m,
    current->sign_in.hash.l, current->sign_in.seq);
  if (current == passed) {
    if (!current->precursor)
      list->last = NULL;
    else
      list->last = current->precursor;
  } else
    passed->precursor = current->precursor;
  free(current);
  list->count--;
}

/* Try to find the linked list element for group groupmap with mapname in list.
   Return reference to element or NULL if not found. */
struct ptlgrp *
find_group(struct ptlgrps_list *list, session_t *mapname)
{
  struct ptlgrp *current, *passed;

  current = list->last;
  passed = list->last;
  while (1) {
    if (current != NULL) {
      if (match_mapname(&(current->groupmap), mapname)) {
        __builtin_printf(" group with map (%x %x %x) %i was added/found\n",
          current->groupmap.hash.h, current->groupmap.hash.m,
          current->groupmap.hash.l, current->groupmap.seq);
        return current;
      }
      passed = current;
      current = current->precursor;
    } else
      return NULL;
  }
}

/* For partial referencing list, set each ptlmbr_t member sign_in equaling
   lastsign_in to given argument sign_in for all groups therein.  note: On
   vrtlogin, when a keymap's reputation key in a partial is shifted becoming
   bkpkey, any ptlmbr_t member sign_in for given partial, must be re-referenced
   vs. that bkpkey to the newly generated key so that dialog based groups
   become maintained with most recent keynames for keymaps.  This also applies
   to each entry from node_orgin into any partial. */
void
sync_groups(struct ptlgrps_list *list, session_t *last, session_t *new)
{
  ;
}

/* Create a linked list construct for session returning reference to an empty
   list of groups residing therein. */
struct ptlgrps_list *
mk_ptlgrps_list(session_t *session)
{
  struct ptlgrps_list *list = NULL;
  if ((list = (struct ptlgrps_list *) malloc(sizeof(struct ptlgrps_list))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for ptlgrps_list\n",
      __FILE__, __LINE__);
    abort();
  }
  list->last = NULL;
  list->count = 0;

  list->session = session;

  return list;
}

/* Remove list and all of it's elements and element references from memory. */
void
rm_ptlgrps_list(struct ptlgrps_list *list)
{
  if (list) {
    while (list->last != NULL) {
      subtract_ptlgrp(list, list->last);
    }
    free(list);
  }
}

/* Add an element reference to partial group groupmap with mapname in the
   linked list construct list.  Return reference to element listed. */
struct ptlgrp *
add_ptlgrp(struct ptlgrps_list *list, session_t *mapname)
{
  struct ptlgrp *listed = NULL;

  if ((listed = (struct ptlgrp *) malloc(sizeof(struct ptlgrp))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for ptlgrps_list entry\n",
      __FILE__, __LINE__);
    abort();
  }
  listed->precursor = list->last;
  list->last = listed;
  list->count++;

  cp_mapname(mapname, &(listed->groupmap));
  listed->holdmaps = mk_modlg_list(listed);
  __builtin_printf("  group with groupmap (%x %x %x) %i added...\n",
    mapname->hash.h, mapname->hash.m, mapname->hash.l, mapname->seq);

  return listed;
}

/* Subtract linked list element referenced by group. */
void
subtract_ptlgrp(struct ptlgrps_list *list, struct ptlgrp *group)
{
  struct ptlgrp *current, *passed;

  __builtin_printf(" message for group with groupmap (%x %x %x) %i\n",
    group->groupmap.hash.h, group->groupmap.hash.m,
    group->groupmap.hash.l, group->groupmap.seq);

  current = list->last;
  passed = list->last;
  while (1) {
    if (current != NULL) {
      if (match_mapname(&(current->groupmap), &(group->groupmap)))
        break;
      passed = current;
      current = current->precursor;
    } else
      return;
  }
  rm_modlg_list(current->holdmaps);
  __builtin_printf("  group with groupmap (%x %x %x) %i abandon this "
    "partial...\n", current->groupmap.hash.h, current->groupmap.hash.m,
    current->groupmap.hash.l, current->groupmap.seq);
  if (current == passed) {
    if (!current->precursor)
      list->last = NULL;
    else
      list->last = current->precursor;
  } else
    passed->precursor = current->precursor;
  free(current);
  list->count--;
}
