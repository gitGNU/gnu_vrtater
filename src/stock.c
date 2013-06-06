/* stock.c: generic vtrmaps. these are rolled into a hmap per vrtater layout.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "progscope.h"
#include "stock.h"
#include "vectors.h"
#include "hmap.h"

/* icosahedron_b */
#define WKY 0.26286555606 /* width of key triangle */
#define LNK 0.809016994375 /* length of key triangle */
#define ATIP 0.850650808352 /* aligned tip */
#define AEDG 0.688190960236 /* aligned edge */
#define ETIP 0.5 /* edge tip */
#define ENDH 0.525731112119 /* height of ends */
#define ISL 0.948537775762 /* inside layer */
#define ISL2 0.474268887881 /* inside layer / 2 */
#define ICOSAHEDRON_B_BOUND0 1

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

float a_icosahedron_b[12][3] = {
				{0, ISL2+ENDH, 0}, /* 0 */

				{0, ISL2, ATIP}, /* 1 */
	{-LNK, ISL2, WKY}, /* 2 */			{LNK, ISL2, WKY}, /* 3 */

		{-ETIP, ISL2, -AEDG}, /* 4 */	{ETIP, ISL2, -AEDG}, /* 5 */

		{-ETIP, -ISL2, AEDG}, /* 6 */	{ETIP, -ISL2, AEDG}, /* 7 */

	{-LNK, -ISL2, -WKY}, /* 8 */			{LNK, -ISL2, -WKY}, /* 9 */
				{0, -ISL2, -ATIP}, /* 10 */

				{0, -ISL2-ENDH, 0} /* 11 */
};

/* enscribe an icosahedron_b VRT_DRAWGEOM_TRIANGLES hmap attached to session,
   with a radius of r.  return null pointer if vohspace full */
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
	vf_t *tri;

	/* vf data
	   calculate hmap size in terms of total vf_t's (3 per triangle)
	   2 type c caps + icosahedron_b capped-face-count */
	int v_total = 3 * (VRT_CAPC_FCOUNT * 2 + VRT_ICOSAHEDRON_B_CFCOUNT);

	/* dialog data
	   calculate size of dialog data(if any) for hmap */
	/* int d_total = 0; */
	/* ... */

	/* diag */
	int v_count = 0;

	/* fill in hmap
	   set total vertice count */
	hmap->vf_total = v_total;

	/* set draw format options */
	hmap->draw.geom = VRT_DRAWGEOM_TRIANGLES;

	/* set bounding */
	hmap->envelope.geom = VRT_BOUND_SPHERE;
	hmap->envelope.v_sz.x = ICOSAHEDRON_B_BOUND0 * r;
	form_mag_vf(&(hmap->envelope.v_sz));

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
		for(j=0, tri=av; j<3; j++, tri++, v_count++) {
			tri->x = a_icosahedron_b[icosahedron_b_idx_top[i][j]][0] * r;
			tri->y = a_icosahedron_b[icosahedron_b_idx_top[i][j]][1] * r;
			tri->z = a_icosahedron_b[icosahedron_b_idx_top[i][j]][2] * r;
			form_mag_vf(tri);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* mid section */
	for(i=0; i<VRT_ICOSAHEDRON_B_CFCOUNT; i++) {
		for(j=0, tri=av; j<3; j++, tri++, v_count++) {
			tri->x = a_icosahedron_b[icosahedron_b_idx_mid[i][j]][0] * r;
			tri->y = a_icosahedron_b[icosahedron_b_idx_mid[i][j]][1] * r;
			tri->z = a_icosahedron_b[icosahedron_b_idx_mid[i][j]][2] * r;
			form_mag_vf(tri);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* bottom cap */
	for(i=0;i<VRT_CAPC_FCOUNT; i++) {
		for(j=0, tri=av; j<3; j++, tri++, v_count++) {
			tri->x = a_icosahedron_b[icosahedron_b_idx_bot[i][j]][0] * r;
			tri->y = a_icosahedron_b[icosahedron_b_idx_bot[i][j]][1] * r;
			tri->z = a_icosahedron_b[icosahedron_b_idx_bot[i][j]][2] * r;
			form_mag_vf(tri);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* volume for caller to set mass */
	hmap->attribs.kg = 4.18879020479 * r * r * r;

	/* diag */
	if(v_total != v_count)
		__builtin_printf("hmap(): err, vertice compile mismatch\n");

	/* add/allocate for dialog data(if any) */
	/* ... */

	return hmap;
}

/* cube_b cubeoid */
#define QDR_SZ 0.57735026919 /* derived from opposite corners at distance 2 */
#define CUBE_B_BOUND0 QDR_SZ

int cube_b_idx[VRT_CUBE_B_FCOUNT][3] = {
	{5, 4, 7}, {7, 4, 6}, {5, 1, 4}, {4, 1, 0}, {5, 7, 1}, {1, 7, 3},
	{2, 3, 6}, {6, 3, 7}, {2, 0, 3}, {3, 0, 1}, {2, 6, 0}, {0, 6, 4}
};

float a_cube_b[8][3] = {
	{-QDR_SZ, QDR_SZ, -QDR_SZ}, /* 0 */ {QDR_SZ, QDR_SZ, -QDR_SZ}, /* 1 */
   {-QDR_SZ, QDR_SZ, QDR_SZ}, /* 2 */		{QDR_SZ, QDR_SZ, QDR_SZ}, /* 3 */

	{-QDR_SZ, -QDR_SZ, -QDR_SZ}, /* 4 */ {QDR_SZ, -QDR_SZ, -QDR_SZ}, /* 5 */

   {-QDR_SZ, -QDR_SZ, QDR_SZ}, /* 6 */		{QDR_SZ, -QDR_SZ, QDR_SZ} /* 7 */
};

/* enscribe a cube_b VRT_DRAWGEOM_TRIANGLES hmap attached to session, with
   length width and height given.  return null pointer if vohspace full */
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
	vf_t *tri;

	/* vf data
	   calculate hmap size in terms of total vf_t's (3 per triangle) */
	int v_total = VRT_CUBE_B_FCOUNT * 3;

	/* dialog data
	   calculate size of dialog data(if any) for hmap */
	/* int d_total = 0; */
	/* ... */

	/* diag */
	int v_count = 0;

	/* fill in hmap
	   set total vertice count */
	hmap->vf_total = v_total;

	/* set draw format options */
	hmap->draw.geom = VRT_DRAWGEOM_TRIANGLES;

	/* set bounding */
	hmap->envelope.geom = VRT_BOUND_RCUBOID;
	hmap->envelope.v_sz.x = CUBE_B_BOUND0 * w / 2;
	hmap->envelope.v_sz.y = CUBE_B_BOUND0 * h / 2;
	hmap->envelope.v_sz.z = CUBE_B_BOUND0 * l / 2;
	form_mag_vf(&(hmap->envelope.v_sz));

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
		for(j=0, tri=av; j<3; j++, tri++, v_count++) {
			tri->x = a_cube_b[cube_b_idx[i][j]][0] * l;
			tri->y = a_cube_b[cube_b_idx[i][j]][1] * w;
			tri->z = a_cube_b[cube_b_idx[i][j]][2] * h;
			form_mag_vf(tri);
		}
		add_tri_to_hmapf(av, ppd);
	}

	/* volume for caller to set mass */
	hmap->attribs.kg = l * w * h;

	/* diag */
	if(v_total != v_count)
		__builtin_printf("hmap(): err, vertice compile mismatch\n");

	/* add/allocate for dialog data(if any) */
	/* ... */

	return hmap;
}

/* enscribe a cylinder_b VRT_DRAWGEOM_TRIANGLES hmap attached to session, with
   r radius, e capedges, l length, and t threads/length.  t may be 0.  the first
   and last threads have a lead-in taper that adds an extra thread length
   between them thus l is divided into t + 1 lengths per thread.  return null
   pointer if vohspace full */
hmapf_t *
hmapf_cylinder_b(session_t *session, float r, int e, float l, int t)
{
	int i, j;

	hmapf_t *hmap;
	if(!(hmap = hmapf(session)))
		return NULL;

	hmap->draw.geom = VRT_DRAWGEOM_TRIANGLES;
	hmap->envelope.geom = VRT_BOUND_CYLINDER;
	hmap->envelope.v_sz.x = r;
	hmap->envelope.v_sz.y = l / 2;
	hmap->envelope.v_sz.z = 0;
	form_mag_vf(&(hmap->envelope.v_sz));

	hmap->vf_total = 3 * ((e * 2 * (3 + t)) - 2);

	/* allocate for hmap's vf data(if any) */
	vf_t *data_vf;
	if((data_vf = (vf_t *) malloc(hmap->vf_total * sizeof(vf_t))) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not malloc for hmap %i\n",
			__FILE__, __LINE__, hmap->index);
		abort();
	}
	hmap->p_data_vf = data_vf; /* maintain p_data_vf in hmap */

/* need to shift the sine vs. cosine for each tip of the polygon formed, as the
   triangle derived will be composed of one the previous triangle components */

	float yoffset = l / 2;							/* scaling vs. y axis of hmap */
	float ledge = r * 2 * sin(M_PI / e);		/* length of polygon edge */
	float halfa = 2 * M_PI / e;					/* half corner angle */
	float lpthread = l / (t + 1);					/* cylinder length per thread */
	float incthread = lpthread / e;				/* increment sum for thread */
	float tprhyp = ledge * e;						/* taper hyptoenuse */
	float pitcha = atan(lpthread / tprhyp);	/* pitch angle of thread */
	float pitchhyp = tprhyp / cos(pitcha);		/* pitch hypotenuse */
	float tpradj = tprhyp * cos(pitcha);		/* taper adjacent */
	float spanbase = e * (1 /(e - .5));			/* span triangle base */
	float threadspan = tprhyp * sin(pitcha);	/* thread span */
	float tprend = sin(spanbase * e) - tprhyp; /* taper endpiece */
	float tproffs = tprend / e;					/* taper offset */

	vf_t org, mid, end, *itr, *tri = (vf_t *) data_vf; /* vrtater coords */
	form_mag_vf(set_vf(&org, 0, yoffset, 0, 0));
	form_mag_vf(set_vf(&mid, 0, yoffset, r, 0));

	/* top */
	for(i = 1; i <= e; i++) {
		set_vf(&end, sin(i * halfa) * r, yoffset, cos(i * halfa) * r, 0);
		form_mag_vf(&end);
		tri->x = (&org)->x;
		tri->y = (&org)->y;
		tri->z = (&org)->z;
		form_mag_vf(tri++);
		tri->x = (&mid)->x;
		tri->y = (&mid)->y;
		tri->z = (&mid)->z;
		form_mag_vf(tri++);
		tri->x = (&end)->x;
		tri->y = (&end)->y;
		tri->z = (&end)->z;
		form_mag_vf(tri++);
		cp_vf(&end, &mid);
	}

	/* top taper */
	itr = (vf_t *) data_vf;
	for(i = 1; i <= e; i++) {
		/* @ orgin, _ m e, o m e, ... */
		++itr;
		tri->x = itr->x; /* mid, o _ e, o m e, ... */
		tri->y = itr->y;
		tri->z = itr->z;
		(tri++)->m = itr->m;

		++itr;
		tri->x = itr->x; /* end, o m _, o m e, ... */
		tri->y = itr->y;
		tri->z = itr->z;
		(tri++)->m = itr->m;

		tri->x = itr->x;
		tri->y = itr->y - (i * incthread); /* end along y */
		tri->z = itr->z;
		form_mag_vf(tri++);

		++itr; /* next orgin, o m e, _ m e, ... */

		if(i > 1) {
			/* @ orgin third segment, o m e, o m e, _.. */
			itr -= 2;
			tri->x = itr->x; /* mid, o m e, o _ e, ... */
			tri->y = itr->y;
			tri->z = itr->z;
			(tri++)->m = itr->m;

			itr++;
			tri->x = itr->x; /* end, o m e, o m _, ... */
			tri->y = itr->y - (i * incthread);
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr--;
			tri->x = itr->x; /* mid, o m e, o _ e, ... */
			tri->y = itr->y - ((i - 1) * incthread); /* end along y - incthread */
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr += 2; /* next orgin, o m e, o m e, _.. */
		}
	}

	/* thread */
	for(i = 0; i < t; i++) {
		itr = (vf_t *) data_vf;
		for(j = 0; j < e; j++) {
			/* @ orgin, _ m e, o m e, ... */
			itr++;
			tri->x = itr->x; /* mid, o _ e, o m e, ... */
			tri->y = itr->y - ((j * incthread) + (i * lpthread));
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr++;
			tri->x = itr->x; /* end, o m _, o m e, ... */
			tri->y = itr->y - (((j + 1) * incthread) + (i * lpthread));
			tri->z = itr->z;
			form_mag_vf(tri++);

			tri->x = itr->x; /* end, o m _, o m e, ... */
			tri->y = itr->y - (((j + 1) * incthread) + ((i + 1) * lpthread));
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr--;
			tri->x = itr->x; /* mid, o _ e, o m e, ... */
			tri->y = itr->y - ((j * incthread) + (i * lpthread));
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr++;
			tri->x = itr->x; /* end, o m _, o m e, ... */
			tri->y = itr->y - (((j + 1) * incthread) + ((i + 1) * lpthread));
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr--;
			tri->x = itr->x; /* mid, o _ e, o m e, ... */
			tri->y = itr->y - ((j * incthread) + ((i + 1) * lpthread));
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr += 2; /* next orgin, o m e, _ m e, ... */
		} 
	}

	/* bot taper */
	itr = (vf_t *) data_vf;
	for(i = e; i > 0; i--) {
		/* last orgin, _ m e, o m e, ... */
		if(i > 1) {
			itr++;
			tri->x = itr->x; /* mid, o _ e, o m e, ... */
			tri->y = -itr->y + (i * incthread);
			tri->z = itr->z;
			form_mag_vf(tri++);

			itr++;
			tri->x = itr->x; /* end, o m _, o m e, ... */
			tri->y = -itr->y + ((i - 1) * incthread);
			tri->z = itr->z;
			form_mag_vf(tri++);

			tri->x = itr->x;
			tri->y = -itr->y;
			tri->z = itr->z;
			(tri++)->m = itr->m;

			itr++; /* next orgin, o m e, _ m e, ... */
		}

		if(i == 1)
			itr += 3;

		itr -= 2;
		tri->x = itr->x; /* last mid, o _ e, o m e, ... */
		tri->y = -itr->y + (i * incthread);
		tri->z = itr->z;
		form_mag_vf(tri++);

		itr++;
		tri->x = itr->x; /* end, o m _, o m e, ... */
		tri->y = -itr->y;
		tri->z = itr->z;
		(tri++)->m = itr->m;

		itr--;
		tri->x = itr->x; /* mid, o _ e, o m e, ... */
		tri->y = -itr->y;
		tri->z = itr->z;
		(tri++)->m = itr->m;

		itr += 2; /* next orgin, o m e, _ m e, ... */
	}

	/* bot
	   inverting y flips normals yet does not change the drawing precedence */
	itr = (vf_t *) data_vf;
	for(i = 1; i <= e; i++) {
		tri->x = itr->x;
		tri->y = -itr->y;
		tri->z = itr->z;
		(tri++)->m = (itr++)->m;
		tri->x = itr->x;
		tri->y = -itr->y;
		tri->z = itr->z;
		(tri++)->m = (itr++)->m;
		tri->x = itr->x;
		tri->y = -itr->y;
		tri->z = itr->z;
		(tri++)->m = (itr++)->m;
	}

	/* volume for caller to set mass */
	hmap->attribs.kg = M_PI * r * r * l;

	return hmap;
}

/* add a triangle to hmap data  */
void
add_tri_to_hmapf(vf_t *av, vf_t **ppd)
{
	int i;
	vf_t *p;

	p = av;
	for(i=0; i<3; i++, p++, (*ppd)++) {
		(*ppd)->x = p->x;
		(*ppd)->y = p->y;
		(*ppd)->z = p->z;
		(*ppd)->m = p->m;
	}
}

/* this may be used for line type hmap if/when implemented */
void
add_vf_to_hmap(vf_t *p, vf_t **ppd)
{
	(*ppd)->x = p->x;
	(*ppd)->y = p->y;
	(*ppd)->z = p->z;
	(*ppd)->m = p->m;
	(*ppd)++;
}
