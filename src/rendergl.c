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
#include "hmap.h"

hmapf_t *fov0;
vf_t oa_fp, *vpt = &oa_fp; 

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
	glFrustum(-.03, .03, -.03, .03, .03, 10000.0); /* for now */

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

/* called after hmap holding fov0 is sent */
void
renderer_next_genopts(genopts_t *genopts)
{
	;
}

/* called per hmap per frame vs. DRAWGEOM, draw hmaps where format supported */
void
render_hmapf(hmapf_t *hmap, int lod)
{

	int i, j;
	vf_t v, nv, edge, plane, *data_vf = hmap->p_data_vf;
	GLfloat glv[3][3], gln[3];

	/* if kbase set, magnify rendered vs. node hugeorgin */
	/* ... */

	/* todo: for render_vobspace()
	   buffer vrt_hmaps_max <= *hmaps, vs. their lod value */
	switch(lod) {

		case VRT_MASK_LOD_INF:
		fov0 = hmap; /* vs. filter in proc_hmapf() sent once, first */
		vpt = &(hmap->v_pos);
		//__builtin_printf("vpt rend %f %f %f\n", vpt->x, vpt->y, vpt->z);
		break;

		case VRT_MASK_LOD_NEAR:
		break;

		case VRT_MASK_LOD_PERIF:
		break;

		case VRT_MASK_LOD_FAR:
		break;
	}

	/* translate to where the optical axis eminates from the focal plane */
	glTranslatef(-vpt->x, -vpt->y, -vpt->z);

	/* draw on basis of draw format options */
	switch(hmap->draw.geom) {

		case VRT_DRAWGEOM_NONE:
		break;

		case VRT_DRAWGEOM_TRIANGLES:
		
		for(i = 0; i < hmap->vf_total / 3; i++) {
			for(j = 0; j < 3; j++, data_vf++) {

				/* rotate verticies for rendering */
				cp_vf(data_vf, &v);
				rotate_vf(&v, &(hmap->v_axi), hmap->ang_dpl);

				/* format vertices for rendering */
				glv[j][0] = (GLfloat)(&v)->x + hmap->v_pos.x;
				glv[j][1] = (GLfloat)(&v)->y + hmap->v_pos.y;
				glv[j][2] = (GLfloat)(&v)->z + hmap->v_pos.z;

				/* diag */
				if((hmap->index >= 0) && (hmap->index <= 5)) {
					if(hmap->index == 0) {
						YEL();
						if(i == 0)
							RED();
					}
					if(hmap->index == 1) {
						GRN();
						if(i == 0)
							RED();
						if(i == 1)
							YEL();
					}
					if(hmap->index == 2) {
						YEL();
						if(i == 0)
							RED();
						if(i == 1)
							GRN();
					}
					if(hmap->index == 3)
						ORN();
					if(hmap->index == 4)
						BLU();
					if(hmap->index == 5)
						VLT();
				} else { GRN(); }
			}

			/* set surface normal */
			(&edge)->x = (float)glv[1][0] - glv[0][0];
			(&edge)->y = (float)glv[1][1] - glv[0][1];
			(&edge)->z = (float)glv[1][2] - glv[0][2];
			(&plane)->x = (float)glv[2][0] - glv[1][0];
			(&plane)->y = (float)glv[2][1] - glv[1][1];
			(&plane)->z = (float)glv[2][2] - glv[1][2];
			cprod_vf(&edge, &plane, &nv); /* adds (&nv)->m */
			gln[0] = (GLfloat)(&nv)->x / (&nv)->m;
			gln[1] = (GLfloat)(&nv)->y / (&nv)->m;
			gln[2] = (GLfloat)(&nv)->z / (&nv)->m;
			glNormal3fv(gln);

			/* draw */
			glBegin(GL_TRIANGLES);
				glVertex3fv(&glv[0][0]);
				glVertex3fv(&glv[1][0]);
				glVertex3fv(&glv[2][0]);
			glEnd();
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
