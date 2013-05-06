/* rendergl.c: render vobs through gl calls.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <math.h>
#include <GL/gl.h>
#include <stdlib.h>
#include "progscope.h"
#include "rendergl.h"
#include "vectors.h"
#include "rotation.h"

void draw_gl_tri(vf_t *, vf_t *, vf_t *);

/* note: keep this re-entrant for regenerating node */
void
init_renderer(void)
{
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_CULL_FACE);

	/* frustum */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/* left, right, bottom, top, near, far */
	glFrustum(-.03, .03, -.03, .03, .03, 100.0);

	/* assume modeling transforms */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* draw a triangle with 3 supplied vf_t's */
void
draw_gl_tri(vf_t *a, vf_t *b, vf_t *c)
{
	vf_t nv;
	float *q, *r, *s, *n;
	GLfloat glv[3][3], gln[3];
	int i;

	/* surface normal */
	cprod_vf(c, b, &nv);
	normz_vf(&nv, &nv);

	n = &((&nv)->x);
	q = &(a->x);
	r = &(b->x);
	s = &(c->x);

	for(i=0; i<3; i++) {
		glv[0][i] = (GLfloat)*q++;
		glv[1][i] = (GLfloat)*r++;
		glv[2][i] = (GLfloat)*s++;
		gln[i] = (GLfloat)*n++;
	}

	glNormal3fv(gln);
	glBegin(GL_TRIANGLES);
		glVertex3fv(&glv[0][0]);
		glVertex3fv(&glv[1][0]);
		glVertex3fv(&glv[2][0]);
	glEnd();
}

/* always called first per frame */
void
set_main_vohfov(vf_t *view_pos, vf_t *view_dir, vf_t *view_roll)
{
	set_vf(view_pos, 0.0f, 1.0f, 2.0f, 2.236068f);
	set_vf(view_dir, 0.0f, -1.0f, -1.0f, 1.414213f);
	set_vf(view_roll, 0.0f, 0.0f, 0.0f, 0.0f);

	/* set next position for main vofov */
	/* ... */

	/* clear last frame */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | \
		GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/* for now, set a color */
	glColor3f(.1,.5,0);
}

/* called per hmap per frame in frequency:
   vs. DRAWGEOM, draw hmaps where format supported */
void
draw_hmapf(hmapf_t *hmap, int lod)
{
	int i, j, vt;
	vf_t *data_vf = hmap->p_data_vf;
	vf_t av[3]; /* array for largest polygonal representation */
	vf_t cpy;
	vf_t *v = &cpy;

	/* if kbase set, magnify rendered vs. node hugeorgin */
	/* ... */

	/* diag markers */
	static vf_t m1, m2;
	set_vf(&m1, 0.57735026919, 0.57735026919, 0.57735026919, 1);
	set_vf(&m2, -0.57735026919, -0.57735026919, -0.57735026919, 1);

	/* them's who run this node percieves at display the field of view of
	   vobspace from glroo.  this is an offset from the vobspace orgin
	   representing where them's are seeing vobs from.  with the following
	   matrix translation, the gl renderer automatically places vobs in
	   the correct place vs. percieved vobspace */
	glTranslatef(-(&glroo)->x, -(&glroo)->y, -(&glroo)->z);

	/* draw on basis of draw format options */
	switch(hmap->draw.geom) {

		case VRT_DRAWGEOM_NONE:
		break;

		case VRT_DRAWGEOM_TRIANGLES:
		/* 3 vf_t's(vertices) per triangle */
		vt = hmap->vf_total / 3;
		for(i=0;i<vt;i++) {
			for(j=0;j<3;j++, data_vf++) {

				/* work with a unit vector representation */
				cp_vf(data_vf, v);
				normz_vf(v, v);

				/* diag markers */
				if(hmap->index == 1)
					cp_vf(&m1, &(hmap->v_pos));
				if(hmap->index == 2)
					cp_vf(&m2, &(hmap->v_pos));

				/* rotate */
				rotate_vf(v, &(hmap->v_axi), hmap->ang_dpl);

				/* restore magnitude vs. unit vector rep. */
				tele_mag_vf(v, v, data_vf->m);

				/* transfer vertex value */
				(&av[j])->x = v->x + hmap->v_pos.x;
				(&av[j])->y = v->y + hmap->v_pos.y;
				(&av[j])->z = v->z + hmap->v_pos.z;

				/* diag for now, appearance */
				if((hmap->index == 0) \
					|| (hmap->index == 1) \
					|| (hmap->index == 2)) {
					/* vob 0 */
					if(hmap->index == 0) {
						YEL();
						/* tri 0 */
						if(i == 0) {
							RED();
						}
					}
					/* vob 1 */
					if(hmap->index == 1) {
						GRN();
						/* tri 0 */
						if(i == 0) {
							RED();
						}
						/* tri 1 */
						if(i == 1) {
							YEL();
						}
					}
					/* vob 2 */
					if(hmap->index == 2) {
						YEL();
						/* tri 0 */
						if(i == 0) {
							RED();
						}
						/* tri 1 */
						if(i == 1) {
							GRN();
						}
					}
				} else {
					/* all else */
					GRN();
				}

				/* diag */
				if((hmap->index > 2) && (hmap->index < 15)) {
					if(hmap->index == 3)
						ORN();
					if(hmap->index == 4)
						BLU();
					if(hmap->index == 5)
						VLT();
				}
			}
			/* gl */
			draw_gl_tri(&av[0], &av[1], &av[2]);
		}
		break;

		case VRT_DRAWGEOM_LINES:
		for(i=0;i<hmap->vf_total;i++) {
			;
		}
		break;

		default:
		__builtin_printf("renderer: err, unsupported hmap geom.\n");
		break;
	}
	glTranslatef((&glroo)->x, (&glroo)->y, (&glroo)->z);
}

/* called immediately before buffer is drawn to all dpy's,  here is nested one
   stacklevel in apon the modelview matrix of where there might be at least
   32, a function to render apon, within, and about hmaps */
void
render_voglspace(hmapf_t *freefov0)
{
	/* set gl renderer offset orgin */
	glTranslatef(-(&glroo)->x, -(&glroo)->y, -(&glroo)->z);
	/* ... */
	;
	glTranslatef((&glroo)->x, (&glroo)->y, (&glroo)->z);
}
