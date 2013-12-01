/* generator.h 
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include "vectors.h"
#include "hmap.h"
#include "partial.h"
#include "session.h"
#include "loginkeys.h"

/* These options are all set by ifnode**.c, some through data maintained in
   dialog.c.  why and what are represented in vobspace_criteria.  when is
   divisions of timer data.  These also effect flexible node dialog messages.
   balance_criteria is still in the works, and may be proposed as
   some hybrid of bits vs. a quantity once needed. */
struct genopts_s {
  int balance_criteria; /* balance_filter options */
  int vobspace_criteria; /* tested context options */
  int why; /* dialog*.c issue */
  int what; /* for why */
  double when; /* for what */
};
typedef struct genopts_s genopts_t;

/* genopts_t vobspace_criteria.  Effective while held high, some of these
   where noted, will be cleared after use. */
enum {
  VRT_ORDINAL_SHUTDOWN,
#define VRT_MASK_SHUTDOWN (1 << VRT_ORDINAL_SHUTDOWN)
  VRT_ORDINAL_DASHF
#define VRT_MASK_DASHF (1 << VRT_ORDINAL_DASHF)
};

struct session_desc *generate_node_orgin(void);
unsigned int calc_cmplxt(complextimate_t *cmplxt);
void regenerate_scene(vf_t *);
void mk_partial_list(void);
void rm_partial_list(void);
struct partial *mk_partial(session_t *, hmapf_t **, unsigned int count, complextimate_t *cmplxt);
void rm_partial(struct partial *);
void close_vobspace(double time_till_closed);
void close_node_orgin(void);
int resize_node_orgin(int, int);
/* Diagnostics. */
void diag_ls_partials(void);
int diag_hmaps_in_partial(struct partial *);
void diag_generator_key_f(void);
void diag_generator_key_g(void);
void diag_generator_key_h(void);
/* Supported calls for bus input. */
void generator_join_hmaps(void);
void generator_intersection(void);
/* Tests. */
void test_add_maps(unsigned int n, int mapstock, session_t *, vf_t *portal, select_t *, complextimate_t *);

#endif /* GENERATOR_H */
