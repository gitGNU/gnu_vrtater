/* stock.c: Stock like basic forms rolled into hmaps.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "stock.h"

/* icosahedron_c */
#define WKY 0.26286555606 /* width of key triangle */
#define LNK 0.809016994375 /* length of key triangle */
#define ATIP 0.850650808352 /* aligned tip */
#define AEDG 0.688190960236 /* aligned edge */
#define ETIP 0.5 /* edge tip */
#define ENDH 0.525731112119 /* height of ends */
#define ISL 0.948537775762 /* inside layer */
#define ISL2 0.474268887881 /* inside layer / 2 */
#define ICOSAHEDRON_C_BOUND0 1

/* cuboid_c. */
#define QDR_SZ 0.57735026919 /* derived from opposite corners at distance 2 */
#define CUBE_C_BOUND0 QDR_SZ

/* Enscribe an icosahedron_c VRT_DRAWGEOM_TRIANGLES hmap attached to session,
   with a radius of r.  Return NULL if vohspace full. */
hmapf_t *
hmapf_icosahedron_c(session_t *session, float r)
{
  int i, j;
  vf_t av[3], *tri, *vmap, *pd, **ppd = &pd;
  hmapf_t *map;

  int icosahedron_c_idx[VRT_ICOSAHEDRON_C_FCOUNT][3] = {{0, 1, 3}, {0, 3, 5}, {0, 5, 4}, {0, 4, 2}, {0, 2, 1}, {1, 7, 3}, {3, 7, 9}, {3, 9, 5}, {5, 9, 10}, {4, 5, 10}, {4, 10, 8}, {2, 4, 8}, {2, 8, 6}, {1, 2, 6}, {1, 6, 7}, {6, 11, 7}, {7, 11, 9}, {9, 11, 10}, {8, 10, 11}, {6, 8, 11}};

  float a_icosahedron_c[12][3] = {{0, ISL2+ENDH, 0}, {0, ISL2, ATIP}, {-LNK, ISL2, WKY}, {LNK, ISL2, WKY}, {-ETIP, ISL2, -AEDG}, {ETIP, ISL2, -AEDG}, {-ETIP, -ISL2, AEDG}, {ETIP, -ISL2, AEDG}, {-LNK, -ISL2, -WKY}, {LNK, -ISL2, -WKY}, {0, -ISL2, -ATIP}, {0, -ISL2-ENDH, 0}};

  /* Attach an hmap. */
  if (!(map = hmapf(session)))
    return NULL;

  /* hmap attribs. */
  map->draw.geom = VRT_DRAWGEOM_TRIANGLES;
  map->attribs.mode |= VRT_MASK_SYNC_VERTICES;
  map->envelope.geom = VRT_BOUND_SPHERE;
  map->envelope.vsz.x = ICOSAHEDRON_C_BOUND0 * r;
  form_mag_vf(&(map->envelope.vsz));
  map->attribs.kg = 4.18879020479 * r * r * r;

  /* Allocate for hmap data. */
  map->vmap_total = VRT_ICOSAHEDRON_C_FCOUNT * 3;
  if ((vmap = (vf_t *) malloc(map->vmap_total * sizeof(vf_t))) == NULL) {
    __builtin_fprintf(stderr,  "vrtater:%s:%d: "
      "Error: Could not malloc for hmap %i\n",
      __FILE__, __LINE__, map->index);
    abort();
  }
  map->vmap = vmap; /* maintain vmap in hmap */

  /* Fill in hmap data. */
  *ppd = map->vmap;
  for (i = 0; i < VRT_ICOSAHEDRON_C_FCOUNT; i++) {
    for (j=0, tri=av; j<3; j++, tri++) {
      tri->x = a_icosahedron_c[icosahedron_c_idx[i][j]][0] * r;
      tri->y = a_icosahedron_c[icosahedron_c_idx[i][j]][1] * r;
      tri->z = a_icosahedron_c[icosahedron_c_idx[i][j]][2] * r;
      form_mag_vf(tri);
    }
    add_tri_to_hmapf(av, ppd);
  }

  return map;
}

/* Enscribe a cuboid_c VRT_DRAWGEOM_TRIANGLES hmap attached to session, with
   length l, width w, and height h.  Return NULL if vohspace full. */
hmapf_t *
hmapf_cuboid_c(session_t *session, float l, float w, float h)
{
  int i, j;
  vf_t av[3], *tri, *vmap, *pd, **ppd = &pd;
  hmapf_t *map;

  int cube_c_idx[VRT_CUBE_C_FCOUNT][3] = {{2, 3, 1}, {1, 0, 2}, {6, 7, 3}, {3, 2, 6}, {7, 5, 1}, {1, 3, 7}, {5, 4, 0}, {0, 1, 5}, {4, 6, 2}, {2, 0, 4}, {4, 5, 7}, {7, 6, 4}};

  float a_cube_c[8][3] = {{-QDR_SZ, QDR_SZ, -QDR_SZ}, {QDR_SZ, QDR_SZ, -QDR_SZ}, {-QDR_SZ, QDR_SZ, QDR_SZ}, {QDR_SZ, QDR_SZ, QDR_SZ}, {-QDR_SZ, -QDR_SZ, -QDR_SZ}, {QDR_SZ, -QDR_SZ, -QDR_SZ}, {-QDR_SZ, -QDR_SZ, QDR_SZ}, {QDR_SZ, -QDR_SZ, QDR_SZ}};

  if (!(map = hmapf(session)))
    return NULL;

  map->draw.geom = VRT_DRAWGEOM_TRIANGLES;
  map->attribs.mode |= VRT_MASK_SYNC_VERTICES;
  map->envelope.geom = VRT_BOUND_RCUBOID;
  map->envelope.vsz.x = CUBE_C_BOUND0 * w / 2;
  map->envelope.vsz.y = CUBE_C_BOUND0 * h / 2;
  map->envelope.vsz.z = CUBE_C_BOUND0 * l / 2;
  form_mag_vf(&(map->envelope.vsz));
  map->attribs.kg = l * w * h;

  map->vmap_total = VRT_CUBE_C_FCOUNT * 3;
  if ((vmap = (vf_t *) malloc(map->vmap_total * sizeof(vf_t))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for hmap %i\n",
      __FILE__, __LINE__, map->index);
    abort();
  }
  map->vmap = vmap; /* maintain vmap in hmap */

  *ppd = map->vmap;
  for (i = 0; i < VRT_CUBE_C_FCOUNT; i++) {
    for (j=0, tri=av; j<3; j++, tri++) {
      tri->x = a_cube_c[cube_c_idx[i][j]][0] * l;
      tri->y = a_cube_c[cube_c_idx[i][j]][1] * w;
      tri->z = a_cube_c[cube_c_idx[i][j]][2] * h;
      form_mag_vf(tri);
    }
    add_tri_to_hmapf(av, ppd);
  }

  return map;
}

/* Enscribe a cylinder_c VRT_DRAWGEOM_TRIANGLES hmap attached to session, with
   radius r, capedges e, length l, and threads t per that length.  t may be 0.
   The first and last threads have end taper's adding an extra thread length.
   n length/(threads + 1) sized lengths are created, the threads and the taper
   set contained therein them.  Return NULL if vohspace full. */
hmapf_t *
hmapf_cylinder_c(session_t *session, float r, int e, float l, int t)
{
  int i, j;
  vf_t a, b, c, *tri, *itr, *vmap;
  hmapf_t *map;
  float yoffset = l / 2; /* scaling vs. y axis of hmap */
  float halfa = 2 * M_PI / e; /* half corner angle */
  float lpthread = l / (t + 1); /* cylinder length per thread */
  float incthread = lpthread / e; /* increment sum for thread */

#define CYL_MORE_IMPLEMENTED_SOMETIME
#ifdef CYL_MORE_IMPLEMENTED
  float ledge = r * 2 * sin(M_PI / e); /* length of polygon edge */
  float tprhyp = ledge * e; /* taper hyptoenuse */
  float pitcha = atan(lpthread / tprhyp); /* pitch angle of thread */
  float pitchhyp = tprhyp / cos(pitcha); /* pitch hypotenuse */
  float tpradj = tprhyp * cos(pitcha); /* taper adjacent */
  float spanbase = e * (1 /(e - .5)); /* span triangle base */
  float threadspan = tprhyp * sin(pitcha); /* thread span */
  float tprend = sin(spanbase * e) - tprhyp; /* taper endpiece */
  float tproffs = tprend / e; /* taper offset */
#endif

  if (!(map = hmapf(session)))
    return NULL;

  map->draw.geom = VRT_DRAWGEOM_TRIANGLES;
  map->attribs.mode |= VRT_MASK_SYNC_VERTICES;
  map->envelope.geom = VRT_BOUND_CYLINDER;
  map->envelope.vsz.x = r;
  map->envelope.vsz.y = l / 2;
  map->envelope.vsz.z = 0;
  form_mag_vf(&(map->envelope.vsz));

  map->vmap_total = 3 * ((e * 2 * (3 + t)) - 2);
  if ((vmap = (vf_t *) malloc(map->vmap_total * sizeof(vf_t))) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not malloc for hmap %i\n",
      __FILE__, __LINE__, map->index);
    abort();
  }
  map->vmap = vmap; /* maintain vmap in hmap */

  /* Cylinder top. */
  itr = tri = (vf_t *) vmap;
  form_mag_vf(set_vf(&a, 0, yoffset, 0, 0));
  form_mag_vf(set_vf(&b, 0, yoffset, r, 0));
  for (i = 1; i <= e; i++) {
    form_mag_vf(set_vf(&c, sin(i * halfa) * r, yoffset, cos(i * halfa) * r, 0));
    form_mag_vf(cp_vf(&a, tri++));
    form_mag_vf(cp_vf(&b, tri++));
    form_mag_vf(cp_vf(&c, tri++));
    cp_vf(&c, &b);
  }

  /* Top taper. */
  for (i = 0; i < e; i++) {

    /* Top taper top face. */
    itr++; /* b */
    cp_vf(itr, tri++); /* alignment off top cap */

    itr++; /* c */
    form_mag_vf(set_vf(tri++, itr->x,
      itr->y - ((i + 1) * incthread),
      itr->z, 0));

    cp_vf(itr, tri++);
    itr++; /* nxt a */

    if (i > 0) { /* first has only a compound top face */

      /* Top taper bottom face. */
      itr -= 2; /* b */
      cp_vf(itr, tri++);

      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - (i * incthread),
        itr->z, 0));

      itr++; /* c */
      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - ((i + 1) * incthread),
        itr->z, 0));
      itr++; /* nxt a */
    }
  }

  /* Thread. */
  for (i = 0; i < t; i++) {
    itr = (vf_t *) vmap;
    for (j = 0; j < e; j++) {

      /* top thread face */
      itr++; /* b */
      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - (i * lpthread + j * incthread),
        itr->z, 0));

      itr++; /* c */
      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - ((i + 1) * lpthread + (j + 1) * incthread),
        itr->z, 0));

      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - (i * lpthread + (j + 1) * incthread),
        itr->z, 0));

      /* bottom thread face */
      itr--; /* b */
      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - (i * lpthread + j * incthread),
        itr->z, 0));

      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - ((i + 1) * lpthread + j * incthread),
        itr->z, 0));

      itr++; /* c */
      form_mag_vf(set_vf(tri++, itr->x,
        itr->y - ((i + 1) * lpthread + (j + 1) * incthread),
        itr->z, 0));

      itr++; /* nxt edge */
    } 
  }

  /* Bottom taper. */
  itr = (vf_t *) vmap;
  j = e - 1;
  for (i = 0; i < e; i++) {
    if (i < j) { /* last has only bottom face */

      /* bottom taper top face */
      itr++; /* b */
      form_mag_vf(set_vf(tri++, itr->x,
        -yoffset + ((e - i) * incthread),
        itr->z, 0));

      itr++; /* c */
      set_vf(tri++, itr->x, -yoffset, itr->z, itr->m);

      form_mag_vf(set_vf(tri++, itr->x,
        -yoffset + ((e - (i + 1)) * incthread),
        itr->z, 0));

      itr -= 2; /* a */
    }

    /* Bottom taper bottom face. */
    itr++; /* b */
    form_mag_vf(set_vf(tri++, itr->x,
      -yoffset + ((e - i) * incthread),
      itr->z, 0));

    set_vf(tri++, itr->x, -yoffset, itr->z, itr->m);

    itr++; /* c */
    set_vf(tri++, itr->x, -yoffset, itr->z, itr->m);

    itr++; /* nxt a */
  }

  /* Cylinder bottom. */
  itr = (vf_t *) vmap;
  for (i = 0; i < e; i++) {

    itr++; /* b */
    set_vf(tri++, itr->x, -yoffset, itr->z, itr->m);

    itr--; /* a */
    set_vf(tri++, itr->x, -yoffset, itr->z, itr->m);

    itr += 2; /* c */
    set_vf(tri++, itr->x, -yoffset, itr->z, itr->m);

    itr++; /* nxt a */
  }

  /* Volume for caller to set mass. */
  map->attribs.kg = M_PI * r * r * l;

  return map;
}

hmapf_t *
hmapf_tetraplate_c(session_t *session)
{
  hmapf_t *rval = NULL;
  return rval;
}

hmapf_t *
hmapf_cylinder_stack(session_t *session)
{
  hmapf_t *rval = NULL;
  return rval;
}

hmapf_t *
hmapf_null(session_t *session)
{
  hmapf_t *rval = NULL;
  return rval;
}

hmapf_t *
hmapf_points(session_t *session)
{
  hmapf_t *rval = NULL;
  return rval;
}

hmapf_t *
hmapf_lines(session_t *session)
{
  hmapf_t *rval = NULL;
  return rval;
}

/* Add triangle referenced by av to hmap data referenced through *p. */
void
add_tri_to_hmapf(vf_t *av, vf_t **p)
{
  int i;
  vf_t *v;

  v = av;
  for (i = 0; i < 3; i++, v++, (*p)++) {
    (*p)->x = v->x;
    (*p)->y = v->y;
    (*p)->z = v->z;
    (*p)->m = v->m;
  }
}

/* Add vertice referenced by v to hmap data referenced through *p. */
void
add_vf_to_hmap(vf_t *v, vf_t **p)
{
  (*p)->x = v->x;
  (*p)->y = v->y;
  (*p)->z = v->z;
  (*p)->m = v->m;
  (*p)++;
}
