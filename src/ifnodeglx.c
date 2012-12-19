/* ifnodeglx.c: node interface for vrtater.
   Copyright (C) 2012 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <GL/glx.h>
#include <stdio.h>
#include "progscope.h"
#include "generator.h"

void setup_glx(int argc, char **argv);
void shutdown_glx(void);

Display *dpy = NULL;
XVisualInfo *xvinf = NULL;
Window xwin;
GLXContext glxcontext;
int dbuff = LVAL_TRUE; /* force flush for single buffer'd visual */

/* setup for given display and rendering libs */
void
setup_glx(int argc, char **argv)
{
	/* gl X rendering */

	/* open connection to X server */
	if((dpy = XOpenDisplay(NULL)) == NULL) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error: Could not open a connection to X server\n",
			__FILE__, __LINE__);
		__builtin_exit(1);
	}

	/* determine glX support */
	int error;
	int event;
	if(!glXQueryExtension(dpy, &error, &event)) {
		__builtin_fprintf(stderr, "vrtater:%s:%d: "
			"Error %i: X server has no support for GLX extension\n",
			__FILE__, __LINE__, error);
		__builtin_exit(1);
	}

	/* choose visual by matching available params */
	int dbv[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
	int sbv[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
	if((xvinf = glXChooseVisual(dpy, DefaultScreen(dpy), dbv)) == NULL) {
		if((xvinf = glXChooseVisual(dpy, DefaultScreen(dpy), sbv)) == NULL) {
			__builtin_fprintf(stderr, "vrtater:%s:%d: "
				"Error: No visual(double nor single) choosable",
				__FILE__, __LINE__);
			__builtin_exit(1);
		}
		dbuff = LVAL_FALSE;
	}

	/* create X colormap */
	Colormap cmap;
	cmap=XCreateColormap(dpy, RootWindow(dpy, xvinf->screen), xvinf->visual, AllocNone);

	/* setup Xwindow attributes */
	XSetWindowAttributes xwinattr;
	xwinattr.colormap = cmap;
	xwinattr.event_mask = KeyPressMask;

	/* create an Xwindow with visual and attributes */
	xwin=XCreateWindow(dpy, RootWindow(dpy, xvinf->screen), 0, 0, 800, 800, 0, xvinf->depth, InputOutput, xvinf->visual, CWColormap|CWEventMask, &xwinattr);

	/* set Xwindow properties */
	XSetStandardProperties(dpy, xwin, "vrtater", "vrtater", None, argv, argc, NULL);

	/* set gl X rendering context */
	if((glxcontext = glXCreateContext(dpy, xvinf, NULL, GL_TRUE)) == NULL)
 		__builtin_exit(1);

	/* make Xwindow current and map it */
	glXMakeCurrent(dpy, xwin, glxcontext);
	XMapWindow(dpy, xwin);
}

void
setup_node(int argc, char **argv)
{
	setup_glx(argc, argv);
}

void
shutdown_glx(void)
{
	if(glxcontext != NULL) {
		glXMakeCurrent(dpy, None, NULL);
		glXDestroyContext(dpy, glxcontext);
		glxcontext = NULL;
	}
}

void
shutdown_node(void)
{
	shutdown_glx();
}

/* state machine */
void
node(void)
{

	XEvent xevent;
	int quit = 0;

	/* interface node */
	while(!quit) {
		while(XPending(dpy)) {
			XNextEvent(dpy, &xevent);
			switch(xevent.type) {

				case KeyPress:
				switch(XKeycodeToKeysym(dpy, xevent.xkey.keycode, 0)) {

					case XK_Escape:
					return;
				}
				break;
			}
		}

		/* adjust vobspace per any tug input  */
		/* ... */

		/* generate next frame  */
		regenerate_scene(&quit);

		/* render */
		if(dbuff)
			glXSwapBuffers(dpy, xwin);
		else
			glFlush();
	}
}
