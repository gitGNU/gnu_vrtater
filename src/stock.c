/* stock.c: unrolled vobs. these are rolled into a hmap per vrtater layout.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "progscope.h"
#include "vectors.h"
#include "hmap.h"

/* hmap icosahedron_b: (vrtater layout) */
#define WKY 0.26286555606 /* width of key triangle */
#define LNK 0.809016994375 /* length of key triangle */
#define ATIP 0.850650808352 /* aligned tip */
#define AEDG 0.688190960236 /* aligned edge */
#define ETIP 0.5 /* edge tip */
#define ENDH 0.525731112119 /* height of ends */
#define ISL 0.948537775762 /* inside layer */
#define ISL2 0.474268887881 /* inside layer / 2 */
#define ICOSAHEDRON_B_BOUND0 1
float af_icosahedron_b[12][3] = {
				{0, ISL2+ENDH, 0}, /* 0 */

				{0, ISL2, ATIP}, /* 1 */
	{-LNK, ISL2, WKY}, /* 2 */			{LNK, ISL2, WKY}, /* 3 */

		{-ETIP, ISL2, -AEDG}, /* 4 */	{ETIP, ISL2, -AEDG}, /* 5 */

		{-ETIP, -ISL2, AEDG}, /* 6 */	{ETIP, -ISL2, AEDG}, /* 7 */

	{-LNK, -ISL2, -WKY}, /* 8 */			{LNK, -ISL2, -WKY}, /* 9 */
				{0, -ISL2, -ATIP}, /* 10 */

				{0, -ISL2-ENDH, 0} /* 11 */
};

/* indices for type icosahedron_b, capped */
int icosahedron_b_idx_top[VRT_CAPC_FCOUNT][3] = {
	{0, 1, 3}, {0, 3, 5}, {0, 5, 4}, {0, 4, 2}, {0, 2, 1}
};
int icosahedron_b_idx_mid[VRT_ICOSAHEDRON_B_CFCOUNT][3] = {
	{1, 7, 3}, {3, 7, 9}, {3, 9, 5}, {5, 9, 10}, {4, 5, 10},
	{4, 10, 8}, {2, 4, 8}, {2, 8, 6}, {1, 2, 6}, {1, 6, 7}
};
int icosahedron_b_idx_bot[VRT_CAPC_FCOUNT][3] = {
	{6, 11, 7}, {7, 11, 9}, {9, 11, 10}, {8, 10, 11}, {6, 8, 11}
};

/* enscribe an icosahedron_b VRT_DRAWTYPE_TRIANGLES hmap attached to session,
   with a radius of r.  return null pointer if vobspace full */
hmapf_t *
hmapf_icosahedron_b(session_t *session, float r)
{
	int i, j;

	/* attach an hmap if any left */
	hmapf_t *hmap;
	if(!(hmap = hmapf(session)))
		return NULL;

	/* for VRT_DRAWGEOM_TRIANGLES, 3 vectors to hold a triangle */
	vf_t av[3];
	vf_t *t;

	/* vf data
	   calculate hmap size in terms of total vf_t's (3 per triangle)
	   2 type c caps + icosahedron_b capped-face-count */
	int v_total = VRT_CAPC_FCOUNT * 2 * 3 + VRT_ICOSAHEDRON_B_CFCOUNT * 3;

	/* dialog data
	   calculate size of dialog data(if any) for hmap */
	/* int d_total = 0; */
	/* ... */

	/* keep diagnostic count of vertices as hmap data is written */
	int v_count = 0;

	/* fill in hmap
	   set total vertice count */
	hmap->vf_total = v_total; /* set total vertice count */

	/* set draw format options */
	hmap->draw.geom = VRT_DRAWGEOM_TRIANGLES;

	/* set bounding */
	hmap->bounding.geom = VRT_BOUND_SPHERE;
	hmap->bounding.v_sz.x = ICOSAHEDRON_B_BOUND0 * r;
	form_mag_vf(&(hmap->bounding.v_sz));

	/* allocate for hmap's vf data(if any) */
	vf_t *data_vf;
	if((data_vf = (vf_t *) malloc(v_total * sizeof(vf_t))) == NULL) {
		__builtin_fprintf(stderr,  "vrtater:%s:%d: "
			"Error: Could not malloc for hmap %i\n",
			__FILE__, __LINE__, hmap->index);
		abort();
	}
	hmap->p_data_vf = data_vf; /* maintain p_data_vf in hmap */

	/* fill in hmap data */
	vf_t *pd;
	vf_t **ppd = &pd;
	*ppd = hmap->p_data_vf;

	/* top cap */
	for(i=0; i<VRT_CAPC_FCOUNT; i++) {
		for(j=0, t=av; j<3; j++, t++, v_count++) {
			t->x = af_icosahedron_b[ icosahedron_b_idx_top[i][j] ] [0] * r;
			t->y = af_icosahedron_b[ icosahedron_b_idx_top[i][j] ] [1] * r;
			t->z = af_icosahedron_b[ icosahedron_b_idx_top[i][j] ] [2] * r;
			form_mag_vf(t);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* mid section */
	for(i=0; i<VRT_ICOSAHEDRON_B_CFCOUNT; i++) {
		for(j=0, t=av; j<3; j++, t++, v_count++) {
			t->x = af_icosahedron_b[ icosahedron_b_idx_mid[i][j] ] [0] * r;
			t->y = af_icosahedron_b[ icosahedron_b_idx_mid[i][j] ] [1] * r;
			t->z = af_icosahedron_b[ icosahedron_b_idx_mid[i][j] ] [2] * r;
			form_mag_vf(t);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* bottom cap */
	for(i=0;i<VRT_CAPC_FCOUNT; i++) {
		for(j=0, t=av; j<3; j++, t++, v_count++) {
			t->x = af_icosahedron_b[ icosahedron_b_idx_bot[i][j] ] [0] * r;
			t->y = af_icosahedron_b[ icosahedron_b_idx_bot[i][j] ] [1] * r;
			t->z = af_icosahedron_b[ icosahedron_b_idx_bot[i][j] ] [2] * r;
			form_mag_vf(t);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* diag */
	if(v_total != v_count)
		__builtin_printf("hmap(): err, vertice compile mismatch\n");

	/* add/allocate for dialog data(if any) */
	/* ... */

	return hmap;
}

/* hmap cubeoid: (vrtater layout)
   QDR_SZ derived from opposite corners at distance 2 */
#define CUBE_B_BOUND0 0.57735026919
#define QDR_SZ 0.57735026919
float af_cube_b[8][3] = {
	{-QDR_SZ, QDR_SZ, -QDR_SZ}, /* 0 */ {QDR_SZ, QDR_SZ, -QDR_SZ}, /* 1 */
   {-QDR_SZ, QDR_SZ, QDR_SZ}, /* 2 */		{QDR_SZ, QDR_SZ, QDR_SZ}, /* 3 */

	{-QDR_SZ, -QDR_SZ, -QDR_SZ}, /* 4 */ {QDR_SZ, -QDR_SZ, -QDR_SZ}, /* 5 */

   {-QDR_SZ, -QDR_SZ, QDR_SZ}, /* 6 */		{QDR_SZ, -QDR_SZ, QDR_SZ} /* 7 */
};

/* indices for type cube_b */
int cube_b_idx[VRT_CUBE_B_FCOUNT][3] = {
	{5, 4, 7}, {7, 4, 6}, {5, 1, 4}, {4, 1, 0}, {5, 7, 1}, {1, 7, 3},
	{2, 3, 6}, {6, 3, 7}, {2, 0, 3}, {3, 0, 1}, {2, 6, 0}, {0, 6, 4}
};

/* enscribe a cube_b VRT_DRAWTYPE_TRIANGLES hmap attached to session, with
   length, width, and height given.  return null pointer if vobspace full */
hmapf_t *
hmapf_cube_b(session_t *session, float l, float w, float h)
{
	int i, j;

	/* attach an hmap if any left */
	hmapf_t *hmap;
	if(!(hmap = hmapf(session)))
		return NULL;

	/* for VRT_DRAWGEOM_TRIANGLES, 3 vectors to hold a triangle */
	vf_t av[3];
	vf_t *t;

	/* vf data
	   calculate hmap size in terms of total vf_t's (3 per triangle)
	   2 type c caps + icosahedron_b capped-face-count */
	int v_total = VRT_CUBE_B_FCOUNT * 3;

	/* dialog data
	   calculate size of dialog data(if any) for hmap */
	/* int d_total = 0; */
	/* ... */

	/* keep diagnostic count of vertices as hmap data is written */
	int v_count = 0;

	/* fill in hmap
	   set total vertice count */
	hmap->vf_total = v_total; /* set total vertice count */

	/* set draw format options */
	hmap->draw.geom = VRT_DRAWGEOM_TRIANGLES;

	/* set bounding */
	hmap->bounding.geom = VRT_BOUND_RCUBOID;
	hmap->bounding.v_sz.x = CUBE_B_BOUND0 * w / 2;
	hmap->bounding.v_sz.y = CUBE_B_BOUND0 * h / 2;
	hmap->bounding.v_sz.z = CUBE_B_BOUND0 * l / 2;
	form_mag_vf(&(hmap->bounding.v_sz));

	/* allocate for hmap's vf data(if any) */
	vf_t *data_vf;
	if((data_vf = (vf_t *) malloc(v_total * sizeof(vf_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for hmap %i\n",
			__FILE__, __LINE__, hmap->index);
		abort();
	}
	hmap->p_data_vf = data_vf; /* maintain p_data_vf in hmap */

	/* fill in hmap data */
	vf_t *pd;
	vf_t **ppd = &pd;
	*ppd = hmap->p_data_vf;

	/* cube_b */
	for(i=0; i<VRT_CUBE_B_FCOUNT; i++) {
		for(j=0, t=av; j<3; j++, t++, v_count++) {
			t->x = af_cube_b[ cube_b_idx[i][j] ] [0] * l;
			t->y = af_cube_b[ cube_b_idx[i][j] ] [1] * w;
			t->z = af_cube_b[ cube_b_idx[i][j] ] [2] * h;
			form_mag_vf(t);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* diag */
	if(v_total != v_count)
		__builtin_printf("hmap(): err, vertice compile mismatch\n");

	/* add/allocate for dialog data(if any) */
	/* ... */

	return hmap;
}
