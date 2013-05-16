/* ifnodeglx.c: node interface for vrtater.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <GL/glx.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "progscope.h"
#include "ifnodeglx.h"
#include "session.h"
#include "hmap.h"
#include "attribs.h"
#include "generator.h"
#include "dialogX11.h"
#include "rendergl.h"
#include "rotation.h"
#include "tug.h"

Display *dpy0, *dpy_dialog;
XVisualInfo *xvinf0, *xvinf_dialog;
Window xwin0, xwin_dialog;
GLXContext glxcontext0;
int dbuff = 1; /* single or double buffer video, will mabye be compile opt */
ifdpy_t ifd0, *ifdpy0 = &ifd0;
hmapf_t *fov0;
int fov0_available;
int exiting = 0;

/* interface factors, for now */
float accel_adv;
float accel_crv;
float aaccel_adv;

/* fps */
time_t start, stop;
int run, frames, sfreq, reads, infcount;
float fps;
float vrt_render_cyc; /* external */

/* diagnostic */
hmapf_t *ryg, *ryg1, *diag1, *diag2, *diag3, *diag4, *diag5, *diag6;
vf_t diag = { 0, 1, 0, 1 };
vf_t isb = { .5,  0,  0, .5};
vf_t jsb = {  0, .5,  0, .5};
vf_t ksb = {  0,  0, .5, .5};

void setup_glx(int argc, char **argv);
void shutdown_glx(void);

/* dialog */
void setup_dialog_interface(void);
void shutdown_dialog_interface(void);

/* session */
void tendto_curr_sessions(void);
int connect_partialspace(session_t *);
void cfg_session_filter(void);

/* setup for given display and rendering libs */
void
setup_glx(int argc, char **argv)
{
	dpy0 = NULL;
	xvinf0 = NULL;

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

void
setup_dialog_interface(void)
{
	;
}

void
shutdown_dialog_interface(void)
{
	;
}

/* state machine */
void
node(int argc, char **argv)
{

	/* setup node */

	setup_glx(argc, argv);
	generate_node();

	/* fov */
	fov0 = (hmapf_t *) p_hmapf(0); /* for now */
	fov0_available = 1;

	/* tug */
	init_tug_io(); /* if any tug tend to it /w start_tug(init_tug_io()) */

	/* fps indication, disabled below for now */
	fps = 28.6; /* guess */
	vrt_render_cyc = .034965;
	frames = 0;
	run = 0;
	sfreq = 1000;
	infcount = 0;
	reads = 0;

	/* interface factors, for now */
	accel_adv = .0575;
	accel_crv = .1; /* reciprocal */
	aaccel_adv = .035;

	/* hmap with fov for dpy0 */
	fov0->ang_spd = 0; /* for now not used for fov hmap */
	fov0->ang_dpl = 0; /* for now not used for fov hmap */
	set_vf(&(fov0->v_vel), 0, 0, 0, 0);
	set_vf(&(fov0->v_axi), 0, 0, -1, 1); /* oa */
	set_vf(&(fov0->v_rel), 0, 1, 0, 1); /* up locally */
	set_vf(&(fov0->v_pos), 0, 0, 0, 0); /* oa@fp */


	/* diag */
	ryg = (hmapf_t *) p_hmapf(1);
	ryg->ang_spd = 1;
	ryg->ang_dpl = 0;
	set_vf(&(ryg->v_vel), 0, 0, 0, 0);
	set_vf(&(ryg->v_axi), .707106, .707106, 0, 1);
	set_vf(&(ryg->v_pos), 0, 49.5, -1, 0);
	form_mag_vf(&(ryg->v_pos));

	ryg1 = (hmapf_t *) p_hmapf(2);
	ryg1->ang_spd = 1;
	ryg1->ang_dpl = 3.141592;
	set_vf(&(ryg1->v_vel), 0, 0, 0, 0);
	set_vf(&(ryg1->v_axi), 0, .707106, .707106, 1);
	set_vf(&(ryg1->v_pos), 0, 49.5, 1, 0);
	form_mag_vf(&(ryg1->v_pos));

	/* diag */
	diag1 = (hmapf_t *) p_hmapf(3);
	diag1->ang_spd = .000012; 
	set_vf(&(diag1->v_vel), 0, .1, 0, .1);
	set_vf(&(diag1->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag1->v_pos), 7.071, 7.071, 7.071, 1);

	diag2 = (hmapf_t *) p_hmapf(4);
	diag2->ang_spd = .000010;
	set_vf(&(diag2->v_vel), 0, .1, 0, .1);
	set_vf(&(diag2->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag2->v_pos), 7.071, 7.071, -7.071, 1);

	diag3 = (hmapf_t *) p_hmapf(5);
	diag3->ang_spd = .000015;
	set_vf(&(diag3->v_vel), 0, .1, 0, .1);
	set_vf(&(diag3->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag3->v_pos), 7.071, -7.071, 7.071, 1);

	/* diag: std basis */
	diag4 = (hmapf_t *) p_hmapf(6);
	diag4->ang_spd = 0;
	set_vf(&(diag4->v_vel), 0, 0, 0, 0);
	set_vf(&(diag4->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag4->v_pos), 0, 0, 0, 0);
	cp_vf(&isb, &(diag4->v_pos));

	diag5 = (hmapf_t *) p_hmapf(7);
	diag5->ang_spd = 0;
	set_vf(&(diag5->v_vel), 0, 0, 0, 0);
	set_vf(&(diag5->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag5->v_pos), 0, 0, 0, 0);
	cp_vf(&jsb, &(diag5->v_pos));

	diag6 = (hmapf_t *) p_hmapf(8);
	diag6->ang_spd = 0;
	set_vf(&(diag6->v_vel), 0, 0, 0, 0);
	set_vf(&(diag6->v_axi), 0, 0, -1.0, 1);
	set_vf(&(diag6->v_pos), 0, 0, 0, 0);
	cp_vf(&ksb, &(diag6->v_pos));

	XEvent xevent;

	/* interface node
	   all events since last frame are summed, the new frame is drawn */
	while(!exiting) {
		while(XPending(dpy0)) {
			XNextEvent(dpy0, &xevent);
			switch(xevent.type) {

				case KeyPress:
				switch(XKeycodeToKeysym(dpy0, xevent.xkey.keycode, 0)) {

					case XK_Escape:
						exiting = 1;
					break;
					case XK_a:
						ifdpy0->keypan += .017453 * aaccel_adv;
						if(ifdpy0->keypan > M_PI)
							ifdpy0->keypan = -M_PI;
					break;
					case XK_d:
						ifdpy0->keypan -= .017453 * aaccel_adv;
						if(ifdpy0->keypan < -M_PI)
							ifdpy0->keypan = M_PI;
					break;
					case XK_l:
						ifdpy0->keytilt += .017453 * aaccel_adv;
						if(ifdpy0->keytilt > M_PI)
							ifdpy0->keytilt = -M_PI;
					break;
					case XK_o:
						ifdpy0->keytilt -= .017453 * aaccel_adv;
						if(ifdpy0->keytilt < -M_PI)
							ifdpy0->keytilt = M_PI;
					break;
					case XK_k:
						ifdpy0->keyroll += .017453 * aaccel_adv;
						if(ifdpy0->keyroll > M_PI)
							ifdpy0->keyroll = -M_PI;
					break;
					case XK_semicolon:
						ifdpy0->keyroll -= .017453 * aaccel_adv;
						if(ifdpy0->keyroll < -M_PI)
							ifdpy0->keyroll = M_PI;
					break;
					case XK_w:
						/* /w ANG_AFS /reg-s u sphere */
						ifdpy0->keyvfwd += .011281 * accel_adv;
					break;
					case XK_s:
						ifdpy0->keyvfwd -= .011281 * accel_adv;
					break;
					case XK_c:
						ifdpy0->keyvside += .008281 * accel_adv;
					break;
					case XK_z:
						ifdpy0->keyvside -= .008281 * accel_adv;
					break;
					case XK_p:
						ifdpy0->keyvvrt += .008281 * accel_adv;
					break;
					case XK_comma:
						ifdpy0->keyvvrt -= .008281 * accel_adv;
					break;
					case XK_space:
						ifdpy0->keypan *= 0.65;
						ifdpy0->keytilt *= 0.55;
						ifdpy0->keyroll *= 0.9;
						ifdpy0->keyvfwd *= 0.85;
						ifdpy0->keyvside *= 0.85;
						ifdpy0->keyvvrt *= 0.85;
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
		   sum in curvilinear fore/backwards acceleration feedback */
		tele_magz_vf(&(fov0->v_vel), &(fov0->v_vel),
			(fov0->v_vel.m * fov0->v_vel.m) /
			(fov0->v_vel.m + (fov0->v_vel.m * accel_crv)));

		/* accelerate, summing d/t/t with d/t for (+/-)fwd, side */
                vf_t acc, acc2, acc3;
                tele_magz_vf(&(fov0->v_axi), &acc, ifdpy0->keyvfwd);
                sum_vf(&acc, &(fov0->v_vel), &(fov0->v_vel));
		tele_magz_vf(&(fov0->v_vel), &(fov0->v_vel), fov0->v_vel.m);

		cprod_vf(&(fov0->v_axi), &(fov0->v_rel), &acc2);
                tele_magz_vf(&acc2, &acc2, ifdpy0->keyvside);
                sum_vf(&(fov0->v_vel), &acc2, &(fov0->v_vel));
                tele_magz_vf(&(fov0->v_rel), &acc3, ifdpy0->keyvvrt);
                sum_vf(&(fov0->v_vel), &acc3, &(fov0->v_vel));

		/* further adjust interfaced hmaps while representing node
		   output.  per display(dpy) per frame, set the field of view
		   (fov) per the hmap's holding any.  a focal plane is
		   represented by vectors v_rel and side, and an optical axis
		   by vector v_axi.  roll is represented by a combination of
		   v_rel and side.  v_pos is represented when applied by the
		   renderer to a matrix translation of the inverse of
		   fov0->v_pos before rendering, and back again before
		   returning. */

		/* calculate a basis vector for tilt */
		vf_t side;
		cprod_vf(&(fov0->v_axi), &(fov0->v_rel), &side);
		normz_vf(&side, &side);

		/* roll rel and side around axial */
		rotate_vf(&(fov0->v_rel), &(fov0->v_axi), ifdpy0->keyroll);
		rotate_vf(&side, &(fov0->v_axi), ifdpy0->keyroll);

		/* pan side and axial around rel */
		rotate_vf(&side, &(fov0->v_rel), -ifdpy0->keypan);
		rotate_vf(&(fov0->v_axi), &(fov0->v_rel), -ifdpy0->keypan);

		/* tilt axial and rel around side */
		rotate_vf(&(fov0->v_axi), &side, -ifdpy0->keytilt);
		rotate_vf(&(fov0->v_rel), &side, -ifdpy0->keytilt);

		/* base's are maintained in their normalized form */
		normz_vf(&(fov0->v_axi), &(fov0->v_axi));
		normz_vf(&(fov0->v_rel), &(fov0->v_rel));

		/* fov0 is passed first to set fp_oa */
		proc_hmapf(fov0, LOD_INF);

		/* generate next frame's worth of hmaps
		   regenerate scene modifies hmap position vs. v_vel, and
		   position and soon rotation through intersection of hmaps */
		regenerate_scene();

		/* rotate the rendered scene around fov0 */
		glRotatef(ifdpy0->keyroll * 180 / M_PI,
			fov0->v_axi.x, fov0->v_axi.y, fov0->v_axi.z);
		glRotatef(-ifdpy0->keypan * 180 / M_PI,
			fov0->v_rel.x, fov0->v_rel.y, fov0->v_rel.z);
		glRotatef(-ifdpy0->keytilt * 180 / M_PI,
			side.x, side.y, side.z);

#define DIAG_OFF
#ifdef DIAG
		/* diag term output */
		__builtin_printf("fov0\n");
		__builtin_printf("  v_pos: x %f y %f z %f m %f\n",
			fov0->v_pos.x, fov0->v_pos.y,
			fov0->v_pos.z, fov0->v_pos.m);
		__builtin_printf("  v_vel: x %f y %f z %f m %f\n",
			fov0->v_vel.x, fov0->v_vel.y,
			fov0->v_vel.z, fov0->v_vel.m);
		__builtin_printf("   vfwd: x %f y %f z %f m %f\n",
			acc.x, acc.y, acc.z, acc.m);
		__builtin_printf("  vside: x %f y %f z %f m %f\n",
			acc2.x, acc2.y, acc2.z, acc2.m);
		__builtin_printf("   vvrt: x %f y %f z %f m %f\n",
			acc3.x, acc3.y, acc3.z, acc3.m);

		__builtin_printf("kbd\n");
		__builtin_printf("   roll(k/;) %f  pan(a/d) %f tilt(o/l) %f\n"
			"   vfwd(w/s) %f vvrt(p/,) %f decel(space)\n",
			ifdpy0->keyroll, ifdpy0->keypan, ifdpy0->keytilt,
			ifdpy0->keyvfwd, ifdpy0->keyvvrt);
#endif /* DIAG */
#define DIAG_TIME_OFF
#ifdef DIAG_TIME
		/* fps: sample interval sfreq, set above, should be proportional
		   considering the sample granularity used.  at the moment this
		   is disabled pending further reading.  for now using
		   vrt_render_cyc default value as scale for state increment.
		   vrtater time dependant calculations will likely be determined
		   primarily vs. the video hardware */
		if(!(frames++ % sfreq)) {
			if(!run++) {
				start = time(NULL);
			} else {
				run -= 2;
				stop = time(NULL);
				fps = (float)frames / (float)(stop - start);
				if(!isinf(fps)) {
					vrt_render_cyc = 1 / fps;
					reads++;
				} else {
					infcount++;
				}
				frames = 0; /* reset */
			}
		}
		__builtin_printf("time\n    fps: %f cyc: %f reads: %i/%i\n\n",
			1 / vrt_render_cyc, vrt_render_cyc,
			reads, reads + infcount);
#endif /* DIAG_TIME */

		/* renderer renders apon, inside of, and outside of hmaps */
		glPushMatrix();
		render_vobspace(fov0_available);
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

/* tending to curr_session_t and prev_caller_session_t info.
   referencing session info generated thru session.c, selection of available
   nodes for calling(cuing) and running, as well as the previous caller archive
   that allows sessions to be continued.  connections are achieved based on
   configuration files, or calls herein */
void
tendto_curr_sessions(void)
{
	/* conditionally connect_vobspace(), etc... */
	;
}

/* run session with remote vobspace node
   other nodes will mirror optionally given 'partial vobspace', or selected
   vobs herein.  success is assumed while implied session_t remains in
   all_sessions data.  reads from remote node will succeed with no data until
   session sync or closed */
int
connect_partialspace(session_t *p_session)
{
	return on_node_session(p_session);
}

void
cfg_session_filter(void)
{
	/* name a list of lists of sessions */
	;
}
