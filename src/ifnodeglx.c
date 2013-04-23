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

/* diagnostic */
hmapf_t *diag, *diag2, *diag3;

/* state machine */
void
node(int argc, char **argv)
{

	/* setup node */
	setup_glx(argc, argv);
	generate_node();
	init_tug_io();

	/* hmap with fov for dpy0
	   diag: orgin of relative basis vectors */
	fov0 = (hmapf_t *) p_hmapf(0);
	fov0->ang_spd = 0; /* zero roll */
	set_vf(&(fov0->v_vel), 0, 0, 0, 0); /* zero track */
	set_vf(&(fov0->v_axi), 0, 0, -1.0, 1); /* oa, zoom */
	set_vf(&(fov0->v_rel), 0, 1, 0, 1); /* up */
	set_vf(&(fov0->v_pos), 0, 0, 0, 0); /* @ fp */

	/* diag: endpoints of relative basis vectors */
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

	XEvent xevent;
	int quit = 0;

	/* interface node
	   all events since last frame are summed, the new frame is drawn.
	   note: VRT_RENDER_CYC, or current vrt_render_cycle can be used to
	   adjust scaling for rotational constants individually */
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
						ifdpy0->keyroll += .017453;
						if(ifdpy0->keyroll > M_PI)
							ifdpy0->keyroll = -M_PI; /* max perceiveable */
					break;
					case XK_d:
						ifdpy0->keyroll -= .017453;
						if(ifdpy0->keyroll < -M_PI)
							ifdpy0->keyroll = M_PI;
					break;
					case XK_w:
						/* /w ANG_AFS /reg-s u sphere */
						ifdpy0->keyvfwd -= .010281;
					break;
					case XK_s:
						ifdpy0->keyvfwd += .010281;
					break;
					case XK_k:
						ifdpy0->keypan -= .017453;
						if(ifdpy0->keypan < -M_PI)
							ifdpy0->keypan = M_PI;
					break;
					case XK_semicolon:
						ifdpy0->keypan += .017453;
						if(ifdpy0->keypan > M_PI)
							ifdpy0->keypan = -M_PI;
					break;
					case XK_o:
						ifdpy0->keytilt += .017453;
						if(ifdpy0->keytilt > M_PI)
							ifdpy0->keytilt = -M_PI;
					break;
					case XK_l:
						ifdpy0->keytilt -= .017453;
						if(ifdpy0->keytilt < -M_PI)
							ifdpy0->keytilt = M_PI;
					break;
					case XK_space:
						ifdpy0->keyroll *= 0.8;
						ifdpy0->keyvfwd *= 0.8;
						ifdpy0->keypan *= 0.8;
						ifdpy0->keytilt *= 0.8;
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

		/* adjust vobspace per any tug input  */
		/* ... */

		/* generate next frame  */
		regenerate_scene(&quit);
		render_voglspace();

		/* per display per frame, as per interface, set field of view
		   including: focal plane vectors v_rel and side, optical axis
		   vector v_axi, and roll, a combination of v_rel and side. */

		vf_t side;

		/* use rel to generate side vector */
		cprod_vf(&(fov0->v_axi), &(fov0->v_rel), &side);
		normz_vf(&side, &side);

		/* move camera for ifdpy0 */
		rotate_vf(&(fov0->v_rel), &(fov0->v_axi), ifdpy0->keyroll);
		rotate_vf(&side, &(fov0->v_axi), ifdpy0->keyroll);
		rotate_vf(&side, &(fov0->v_rel), ifdpy0->keypan);
		rotate_vf(&(fov0->v_axi), &(fov0->v_rel), ifdpy0->keypan);
		rotate_vf(&(fov0->v_axi), &side, ifdpy0->keytilt);
		rotate_vf(&(fov0->v_rel), &side, ifdpy0->keytilt);

		 /* bulk normalize to save cycles */
		normz_vf(&side, &side);
		normz_vf(&(fov0->v_axi), &(fov0->v_axi));
		normz_vf(&(fov0->v_rel), &(fov0->v_rel));

		/* diag */
		cp_vf(&side, &(diag->v_pos));
		cp_vf(&(fov0->v_rel), &(diag2->v_pos));
		cp_vf(&(fov0->v_axi), &(diag3->v_pos));
		__builtin_printf("   side: x %f y %f z %f m %f\n",
			diag->v_pos.x, diag->v_pos.y,
			diag->v_pos.z, diag->v_pos.m);
		__builtin_printf("  v_rel: x %f y %f z %f m %f\n",
			diag2->v_pos.x, diag2->v_pos.y,
			diag2->v_pos.z, diag2->v_pos.m);
		__builtin_printf("  v_axi: x %f y %f z %f m %f\n",
			diag3->v_pos.x, diag3->v_pos.y,
			diag3->v_pos.z, diag3->v_pos.m);

		__builtin_printf("roll %f : pan %f : tilt %f : vfwd %f\n\n\n",
			ifdpy0->keyroll, ifdpy0->keypan,
			ifdpy0->keytilt, ifdpy0->keyvfwd);

		/* draw to dpy0 */
		if(dbuff)
			glXSwapBuffers(dpy0, xwin0);
		else
			glFlush();

		/* start of new frame */
		glLoadIdentity();
	}

	/* shutdown node */
	close_vobspace(0); /* _now_ for now */
	close_node(); /* !!move to callback_close_vobspace() */
	shutdown_glx();
}
