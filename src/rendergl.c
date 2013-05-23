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

hmapf_t *fov0;
vf_t oa_fp, *vpt = &oa_fp; 

void draw_gl_tri(vf_t *, vf_t *, vf_t *);

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

/* always called first per frame */
void
init_next_buffer(void)
{
	/* clear last frame */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/* for now, set a color */
	glColor3f(.1,.5,0);
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

/* called per hmap per frame vs. DRAWGEOM, draw hmaps where format supported */
void
draw_hmapf(hmapf_t *hmap, int lod)
{
	int i, j, vt;
	vf_t *data_vf = hmap->p_data_vf;
	vf_t av[3]; /* array for largest polygonal representation */
	vf_t v;

	/* if kbase set, magnify rendered vs. node hugeorgin */
	/* ... */

	/* set vpt */
	if(lod == LOD_INF) {
		vpt = &(hmap->v_pos);
		fov0 = hmap;
	}

	/* translate to where the optical axis eminates from the focal plane */
	glTranslatef(-vpt->x, -vpt->y, -vpt->z);

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
				cp_vf(data_vf, &v);
				normz_vf(&v, &v);

				/* rotate verticies for rendering */
				rotate_vf(&v, &(hmap->v_axi), hmap->ang_dpl);

				/* restore magnitude vs. unit vector rep. */
				tele_mag_vf(&v, &v, data_vf->m);

				/* transfer vertex value */
				(&av[j])->x = (&v)->x + hmap->v_pos.x;
				(&av[j])->y = (&v)->y + hmap->v_pos.y;
				(&av[j])->z = (&v)->z + hmap->v_pos.z;

				/* diag */
				if((hmap->index == 0) || (hmap->index == 1) || (hmap->index == 2)) {
					/* hmap 0 */
					if(hmap->index == 0) {
						YEL();
						/* tri 0 */
						if(i == 0)
							RED();
					}
					/* hmap 1 */
					if(hmap->index == 1) {
						GRN();
						/* tri 0 */
						if(i == 0)
							RED();
						/* tri 1 */
						if(i == 1)
							YEL();
					}
					/* hmap 2 */
					if(hmap->index == 2) {
						YEL();
						/* tri 0 */
						if(i == 0)
							RED();
						/* tri 1 */
						if(i == 1)
							GRN();
					}
				} else {
					/* all else */
					GRN();
				}

				/* diag */
				if((hmap->index >= 3) && (hmap->index <= 5)) {
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
	glTranslatef(vpt->x, vpt->y, vpt->z);
}

/* called after draw_hmapf() and immediately before buffer is drawn to dpy0
   here is nested one stacklevel in apon the modelview matrix of where there
   might be at least 32, a function to render apon hmaps and also therein and
   thereout of those.  viewpoint vpt is the position of hmap fov0, and where
   freefov is true, caller specifies that the renderer may modify fov0 attribs
   in the way specified through the renderers interface */
void
render_vobspace(int fov0_available)
{
	/* set gl renderer offset orgin */
	glTranslatef(-vpt->x, -vpt->y, -vpt->z);
	;
	glTranslatef(vpt->x, vpt->y, vpt->z);
}
