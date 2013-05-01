/* ifnodeglx.c: node interface for vrtater.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "progscope.h"
#include "ifnodeglx.h" /* !! */
#include "hmap.h"
#include "generator.h"
#include "rendergl.h"
#include "rotation.h"
#include "tug.h"

void setup_glx(int argc, char **argv);
void shutdown_glx(void);

Display *dpy0 = NULL;
ifdpy_t ifd0, *ifdpy0 = &ifd0;
hmapf_t *fov0; /* external */
XVisualInfo *xvinf0 = NULL;
Window xwin0;
GLXContext glxcontext0;
int dbuff = LVAL_TRUE; /* force flush for single buffer'd visual */

vf_t glroo; /* gl renderer orgin offset.  external */

/* interface factors, for now */
float accel_adv; /* advantage */
float accel_ofs; /* offset */

/* diagnostic */
hmapf_t *diag, *diag2, *diag3, *diag4, *diag5, *diag6;

/* setup for given display and rendering libs */
void
setup_glx(int argc, char **argv)
{
	/* gl X rendering */

	/* open connection to X server */
	if((dpy0 = XOpenDisplay(NULL)) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not open a connection to X server\n",
			__FILE__, __LINE__);
		__builtin_exit(1);
	}

	/* determine glX support */
	int error;
	int event;
	if(!glXQueryExtension(dpy0, &error, &event)) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error %i: X server has no support for GLX extension\n",
			__FILE__, __LINE__, error);
		__builtin_exit(1);
	}

	/* choose visual by matching available params */
	int dbv[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
	int sbv[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
	if((xvinf0 = glXChooseVisual(dpy0, DefaultScreen(dpy0), dbv)) == NULL) {
		if((xvinf0 = glXChooseVisual(dpy0, DefaultScreen(dpy0), sbv)) == NULL) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: No visual(double nor single) choosable",
				__FILE__, __LINE__);
			__builtin_exit(1);
		}
		dbuff = LVAL_FALSE;
	}

	/* create X colormap */
	Colormap cmap;
	cmap=XCreateColormap(dpy0, RootWindow(dpy0, xvinf0->screen), xvinf0->visual, AllocNone);

	/* setup Xwindow attributes */
	XSetWindowAttributes xwinattr;
	xwinattr.colormap = cmap;
	xwinattr.event_mask = KeyPressMask;

	/* create an Xwindow with visual and attributes */
	xwin0=XCreateWindow(dpy0, RootWindow(dpy0, xvinf0->screen), 0, 0, 800, 800, 0, xvinf0->depth, InputOutput, xvinf0->visual, CWColormap|CWEventMask, &xwinattr);

	/* set Xwindow properties */
	XSetStandardProperties(dpy0, xwin0, "vrtater", "vrtater", None, argv, argc, NULL);

	/* set gl X rendering context */
	if((glxcontext0 = glXCreateContext(dpy0, xvinf0, NULL, GL_TRUE)) == NULL)
 		__builtin_exit(1);

	/* make Xwindow current and map it */
	glXMakeCurrent(dpy0, xwin0, glxcontext0);
	XMapWindow(dpy0, xwin0);
}

void
shutdown_glx(void)
{
	if(glxcontext0 != NULL) {
		glXMakeCurrent(dpy0, None, NULL);
		glXDestroyContext(dpy0, glxcontext0);
		glxcontext0 = NULL;
	}
}

/* state machine */
void
node(int argc, char **argv)
{

	/* setup node */
	setup_glx(argc, argv);
	generate_node();
	init_tug_io();

	/* interface factors, for now */
	accel_adv = .1;
	accel_ofs = .5;

	/* hmap with fov for dpy0 */
	fov0 = (hmapf_t *) p_hmapf(0);
	fov0->ang_spd = 0; /* for now not used for fov hmap */
	fov0->ang_dpl = 0; /* for now not used for fov hmap */
	set_vf(&(fov0->v_vel), 0, 0, 0, 0);
	set_vf(&(fov0->v_axi), 0, 0, -1, 1); /* oa */
	set_vf(&(fov0->v_rel), 0, 1, 0, 1); /* up locally */
	set_vf(&(fov0->v_pos), 0, 0, 0, 0); /* @fp */

	/* diag: endpoints of relative basis vectors vs. node orgin */
	diag = (hmapf_t *) p_hmapf(3);
	diag->ang_spd = 0;
	set_vf(&(diag->v_vel), 0, 0, 0, 0);
	set_vf(&(diag->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag->v_pos), 0, 0, 0, 0);

	diag2 = (hmapf_t *) p_hmapf(4);
	diag2->ang_spd = 0;
	set_vf(&(diag2->v_vel), 0, 0, 0, 0);
	set_vf(&(diag2->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag2->v_pos), 0, 0, 0, 0);

	diag3 = (hmapf_t *) p_hmapf(5);
	diag3->ang_spd = 0;
	set_vf(&(diag3->v_vel), 0, 0, 0, 0);
	set_vf(&(diag3->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag3->v_pos), 0, 0, 0, 0);

	/* diag: std basis */
	diag4 = (hmapf_t *) p_hmapf(6);
	diag4->ang_spd = 0;
	set_vf(&(diag4->v_vel), 0, 0, 0, 0);
	set_vf(&(diag4->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag4->v_pos), 0, 0, 0, 0);

	diag5 = (hmapf_t *) p_hmapf(7);
	diag5->ang_spd = 0;
	set_vf(&(diag5->v_vel), 0, 0, 0, 0);
	set_vf(&(diag5->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag5->v_pos), 0, 0, 0, 0);

	diag6 = (hmapf_t *) p_hmapf(8);
	diag6->ang_spd = 0;
	set_vf(&(diag6->v_vel), 0, 0, 0, 0);
	set_vf(&(diag6->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag6->v_pos), 0, 0, 0, 0);

	XEvent xevent;
	int quit = 0;

	/* interface node
	   all events since last frame are summed, the new frame is drawn */
	while(!quit) {
		while(XPending(dpy0)) {
			XNextEvent(dpy0, &xevent);
			switch(xevent.type) {

				case KeyPress:
				switch(XKeycodeToKeysym(dpy0, xevent.xkey.keycode, 0)) {

					case XK_Escape:
						quit = LVAL_TRUE;
					break;
					case XK_a:
						ifdpy0->keypan += .017453 * accel_adv;
						if(ifdpy0->keypan > M_PI)
							ifdpy0->keypan = -M_PI;
					break;
					case XK_d:
						ifdpy0->keypan -= .017453 * accel_adv;
						if(ifdpy0->keypan < -M_PI)
							ifdpy0->keypan = M_PI;
					break;
					case XK_w:
						/* /w ANG_AFS /reg-s u sphere */
						ifdpy0->keyvfwd += .011281 * accel_adv;
					break;
					case XK_s:
						ifdpy0->keyvfwd -= .011281 * accel_adv;
					break;
					case XK_k:
						ifdpy0->keyroll += .017453 * accel_adv;
						if(ifdpy0->keyroll > M_PI)
							ifdpy0->keyroll = -M_PI;
					break;
					case XK_semicolon:
						ifdpy0->keyroll -= .017453 * accel_adv;
						if(ifdpy0->keyroll < -M_PI)
							ifdpy0->keyroll = M_PI;
					break;
					case XK_o:
						ifdpy0->keytilt -= .017453 * accel_adv;
						if(ifdpy0->keytilt < -M_PI)
							ifdpy0->keytilt = M_PI;
					break;
					case XK_l:
						ifdpy0->keytilt += .017453 * accel_adv;
						if(ifdpy0->keytilt > M_PI)
							ifdpy0->keytilt = -M_PI;
					break;
					case XK_space:
						ifdpy0->keyroll *= 0.9;
						ifdpy0->keyvfwd *= 0.85;
						ifdpy0->keypan *= 0.75;
						ifdpy0->keytilt *= 0.75;
					break;
					case XK_Tab:
					break;
					case XK_backslash:
					break;
				}
				break;
				case ButtonPress:
				break;
				case ButtonRelease:
				break;
				case MotionNotify:
				break;
			}
		}

		/* adjust interfaced hmaps per any node input
		   for now sum friction into velocity for interfaced hmaps.
		   this will bring hmap accel to 0 over time */
		tele_magz_vf(&(fov0->v_vel), &(fov0->v_vel),
			fov0->v_vel.m * accel_ofs);

		/* accelerate, summing d/t/t with d/t for forward/back */
                vf_t acc;
                tele_magz_vf(&(fov0->v_axi), &acc, ifdpy0->keyvfwd);
                sum_vf(&acc, &(fov0->v_vel), &(fov0->v_vel));

		/* set orgin offset for gl renderer */
		cp_vf(&(fov0->v_pos), &glroo);

		/* generate next frame's worth of hmaps
		   regenerate scene modifies hmap position vs. v_vel, and
		   soon position and orientation through intersection. */
		regenerate_scene(&quit);

		/* further adjust interfaced hmaps while representing node
		   output.  per display(dpy) per frame, set the field of view
		   (fov) per the hmap's holding any.  a focal plane is
		   represented by vectors v_rel and side, and an optical axis
		   by vector v_axi.  roll is represented by a combination of
		   v_rel and side. v_pos is represented when applied to a matrix
		   translation of the fov along the optical axis.
		   note: the scene is generated already.  hmap positions remain
		   valid.  new hmap orientations here set are applicable
		   immediately, however not drawn till next frame. */

		/* calculate a basis vector for tilt */
		vf_t side;
		cprod_vf(&(fov0->v_axi), &(fov0->v_rel), &side);
		normz_vf(&side, &side);

		/* roll rel and side around axial */
		rotate_vf(&(fov0->v_rel), &(fov0->v_axi), ifdpy0->keyroll);
		rotate_vf(&side, &(fov0->v_axi), ifdpy0->keyroll);
		glRotatef(ifdpy0->keyroll * 180 / M_PI,
			fov0->v_axi.x, fov0->v_axi.y, fov0->v_axi.z);

		/* pan side and axial around rel */
		rotate_vf(&side, &(fov0->v_rel), -ifdpy0->keypan);
		rotate_vf(&(fov0->v_axi), &(fov0->v_rel), -ifdpy0->keypan);
		glRotatef(-ifdpy0->keypan * 180 / M_PI,
			fov0->v_rel.x, fov0->v_rel.y, fov0->v_rel.z);

		/* tilt axial and rel around side */
		rotate_vf(&(fov0->v_axi), &side, -ifdpy0->keytilt);
		rotate_vf(&(fov0->v_rel), &side, -ifdpy0->keytilt);
		glRotatef(-ifdpy0->keytilt * 180 / M_PI,
			side.x, side.y, side.z);

		/* base's are maintained in their normalized form */
		normz_vf(&(fov0->v_axi), &(fov0->v_axi));
		normz_vf(&(fov0->v_rel), &(fov0->v_rel));


		/* diag
		   express abs rotations with hmaps */
		//cp_vf(&side, &(diag->v_pos));
		//cp_vf(&(fov0->v_rel), &(diag2->v_pos));
		//cp_vf(&(fov0->v_axi), &(diag3->v_pos));
		/* indicators */
		vf_t i = {.05,   0,   0, .05};
		vf_t j = {  0, .05,   0, .05};
		vf_t k = {  0,   0, .05, .05};
		cp_vf(&i, &(diag4->v_pos));
		cp_vf(&j, &(diag5->v_pos));
		cp_vf(&k, &(diag6->v_pos));
		/* diag term output */
		__builtin_printf("fov0\n");
		__builtin_printf("  v_pos: x %f y %f z %f m %f\n",
			fov0->v_pos.x, fov0->v_pos.y,
			fov0->v_pos.z, fov0->v_pos.m);
		__builtin_printf("  v_vel: x %f y %f z %f m %f\n",
			fov0->v_vel.x, fov0->v_vel.y,
			fov0->v_vel.z, fov0->v_vel.m);
		__builtin_printf("    acc: x %f y %f z %f m %f\n",
			acc.x, acc.y, acc.z, acc.m);
		__builtin_printf("kbd\n");
		__builtin_printf("   roll(k/;) %f  pan(a/d) %f tilt(o/l) %f\n"
			"   vfwd(w/s) %f decel(space)\n\n",
			ifdpy0->keyroll, ifdpy0->keypan,
			ifdpy0->keytilt, ifdpy0->keyvfwd);


		/* renderer renders apon, inside of, and outside of hmaps */
		glPushMatrix();
		render_voglspace();
		glPopMatrix();

		/* draw to dpy0 */
		if(dbuff)
			glXSwapBuffers(dpy0, xwin0);
		else
			glFlush();

		/* start of new frame.  not clearing identity matrix.  will
		   let it stack.  a function to periodically align hmap's
		   will follow at some point, clearing the identity matrix */
		glMatrixMode(GL_MODELVIEW);
	}

	/* shutdown node */
	close_vobspace(0); /* now, for now */
	close_node(); /* note: move to callback_close_vobspace() */
	shutdown_glx();
}
