/* ifnodeglx.c: node interface for vrtater.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <GL/glx.h>
#include <stdio.h>
#include <math.h>
#include "progscope.h"
#include "ifnodeglx.h" /* !! */
#include "hmap.h"
#include "generator.h"
#include "rendergl.h"
#include "tug.h"

void setup_glx(int argc, char **argv);
void shutdown_glx(void);

Display *dpy0 = NULL;
ifdpy_t ifdpy0;
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

/* state machine */
void
node(int argc, char **argv)
{

	/* setup node */
	setup_glx(argc, argv);
	generate_node();
	init_tug_io();

	/* dpy0->indexfov, for now
	   use hmap 0 by nullifying wander apon it
	   set oa and fp to position camera */
	fov0 = (hmapf_t *) p_hmapf(0);
	fov0->ang_spd = 0; /* zero roll */
	set_vf(&(fov0->v_vel), 0, 0, 0, 0); /* zero track */
	set_vf(&(fov0->v_axi), 0, 0, -1.0, 1); /* oa */
	set_vf(&(fov0->v_pos), 0, 0, 1, 1); /* @ fp */

	XEvent xevent;
	int quit = 0;

	/* interface node */
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
						(&ifdpy0)->keyroll += .017453;
						if((&ifdpy0)->keyroll > M_PI)
							(&ifdpy0)->keyroll = -M_PI; /* max perceiveable */
					break;
					case XK_d:
						(&ifdpy0)->keyroll -= .017453;
						if((&ifdpy0)->keyroll < -M_PI)
							(&ifdpy0)->keyroll = M_PI;
					break;
					case XK_w:
						(&ifdpy0)->keyvfore -= VRT_RENDER_CYC; /* (+/-)1m/(keyboard)s^2 */
					break;
					case XK_s:
						(&ifdpy0)->keyvfore += VRT_RENDER_CYC;
					break;
					case XK_k:
						(&ifdpy0)->keypan += .017453;
						if((&ifdpy0)->keypan > M_PI)
							(&ifdpy0)->keypan = -M_PI;
					break;
					case XK_semicolon:
						(&ifdpy0)->keypan -= .017453;
						if((&ifdpy0)->keypan < -M_PI)
							(&ifdpy0)->keypan = M_PI;
					break;
					case XK_o:
						(&ifdpy0)->keytilt += .017453;
						if((&ifdpy0)->keytilt > M_PI)
							(&ifdpy0)->keytilt = -M_PI;
					break;
					case XK_l:
						(&ifdpy0)->keytilt -= .017453;
						if((&ifdpy0)->keytilt < -M_PI)
							(&ifdpy0)->keytilt = M_PI;
					break;
					case XK_space:
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

		__builtin_printf("\troll %f : pan %f : tilt %f : vfore %f\n\n", (&ifdpy0)->keyroll, (&ifdpy0)->keypan, (&ifdpy0)->keytilt, (&ifdpy0)->keyvfore);

		/* render to dpy0 */
		if(dbuff)
			glXSwapBuffers(dpy0, xwin0);
		else
			glFlush();

		/* assertion?: always load identity matrix at start of new frame
		   initial call is in init_renderer() */
		glLoadIdentity();
	}

	/* shutdown node */
	close_vobspace(0); /* now for now */
	close_node(); /* move to callback_close_vobspace() after implemented */
	shutdown_glx();
}
