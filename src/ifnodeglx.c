/* ifnodeglx.c: A node interface.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#include <X11/X.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "ifnodeglx.h"
#include "tug.h"
#include "generator.h"
#include "vectors.h"
#include "rotation.h"
#include "hmap.h"
#include "session.h"
#include "dialogX11.h"
#include "rendergl.h"
#include "transform.h"
#include "attribs.h"
#include "stock.h"
#include "loginkeys.h"
#include "partial.h"

/* vrtater. */
struct session_desc *orgin_desc;
int runnode = 1;

/* Displays. */
Display *dpy0, *dpy_dialog;
XVisualInfo *xvinf0, *xvinf_dialog;
Window xwin0;
int xwin0x = 800;
int xwin0y = 800;
GLXContext glxcontext0;
/* single or double buffer video, would be nice... Needs to be tested. */
int dbuff = 1;
ifdpy_t ifdpy0 = {0,
  1.150, .1, .035,
  0, .010453, .65,
  0, .010453, .55,
  0, .010453, .9,
  0, .011281, .85,
  0, .008281, .85,
  0, .008281, .85};
hmapf_t *fov0;
int fov0_available = 0;

/* Time related. */
time_t start, stop;
int run, frames, sfreq, reads, infcount;
float fps;
float vrt_render_cyc;

/* Pre-alpha dialog. */
int diagtext = 0;
hmapf_t *diagtext0; /* reference hmap to receive text entry */
int dialogrecurrant = 0;
char diagtextmsg[] = "diagnostic hmap text entry mode\n[tab][space] and "
  ",0123456789=abcdefghijklmnopqrstuvwxyz are appended to dialog\n"
  "[return] resumes directional inputs\n[del] erases any current dialog, "
  "including this\n";

/* Temporary diagnostics. */
hmapf_t *ryg, *ryg1, *diag1, *diag2, *diag3, *diag4, *diag5, *diag6, *diag8;
vf_t diag = { 0, 10, 0, 10 };
vf_t isb = { 5, 0, 0, 5 };
vf_t jsb = { 0, 5, 0, 5 };
vf_t ksb = { 0, 0, 5, 5 };
vf_t vrloc8 = { 0, 0, -80, 80 };

void setup_glx(int argc, char **argv);
void shutdown_glx(void);
void setup_dialog_interface(void);
void shutdown_dialog_interface(void);
void tendto_curr_sessions(void);
int connect_to_peers(session_t *);
int maintain_reputation(session_t *);

/* Temporary diagnostics. */
void diag_char(char c);
void diag_peer_key_f(void);
void diag_peer_key_g(void);
void diag_peer_key_h(void);
void diag_message(void);

/* Testing. */
struct partial *test_continuity(char *url, char *oneliner, session_t *thru, hmapf_t **maps, complextimate_t *cmplxt);
int test_continue_partial(struct partial *node, char *url, struct ptlrepute_list *new, struct ptlrepute *repute, session_t *session_here, hmapf_t *keymap, hmapf_t *holdmap, unsigned int tl_cmplxt);
struct partial *test_mk_flexible_node(char *seedfiles, hmapf_t **, complextimate_t *);
void test_detach_all_partials(void);
void test_continue_repute(hmapf_t *);

/* Setup for given display and rendering libs. */
void
setup_glx(int argc, char **argv)
{
  dpy0 = NULL;
  xvinf0 = NULL;

  /* Open a connection to X server. */
  if ((dpy0 = XOpenDisplay(NULL)) == NULL) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error: Could not open a connection to X server\n",
      __FILE__, __LINE__);
    __builtin_exit(1);
  }

  /* Determine glx support. */
  int error;
  int event;
  if (!glXQueryExtension(dpy0, &error, &event)) {
    __builtin_fprintf(stderr, "vrtater:%s:%d: "
      "Error %i: X server has no support for GLX extension\n",
      __FILE__, __LINE__, error);
    __builtin_exit(1);
  }

  /* Choose visual by matching available parameters. */
  int dbv[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};
  int sbv[] = {GLX_RGBA, GLX_DEPTH_SIZE, 16, None};
  if ((xvinf0 = glXChooseVisual(dpy0, DefaultScreen(dpy0), dbv)) == NULL) {
    if ((xvinf0 = glXChooseVisual(dpy0, DefaultScreen(dpy0), sbv)) == NULL) {
      __builtin_fprintf(stderr, "vrtater:%s:%d: "
        "Error: No visual(double nor single) choosable",
        __FILE__, __LINE__);
      __builtin_exit(1);
    }
    dbuff = 0;
  }

  /* Create X colormap. */
  Colormap cmap;
  cmap = XCreateColormap(dpy0, RootWindow(dpy0, xvinf0->screen), xvinf0->visual, AllocNone);

  /* Setup Xwindow attributes. */
  XSetWindowAttributes xwinattr;
  xwinattr.colormap = cmap;
  xwinattr.event_mask = KeyPressMask;

  /* Create an Xwindow with visual and attributes. */
  xwin0 = XCreateWindow(dpy0, RootWindow(dpy0, xvinf0->screen), 0, 0, xwin0x, xwin0y, 0, xvinf0->depth, InputOutput, xvinf0->visual, CWColormap|CWEventMask, &xwinattr);

  /* Set Xwindow properties. */
  XSetStandardProperties(dpy0, xwin0, "vrtater", "vrtater", None, argv, argc, NULL);

  /* Set gl X rendering context. */
  if ((glxcontext0 = glXCreateContext(dpy0, xvinf0, NULL, GL_TRUE)) == NULL)
    __builtin_exit(1);

  /* Make Xwindow current and map it. */
  glXMakeCurrent(dpy0, xwin0, glxcontext0)   ;
  XMapWindow(dpy0, xwin0);
}

/* Shutdown for given display and rendering libs. */
void
shutdown_glx(void)
{
  if (glxcontext0 != NULL) {
    glXMakeCurrent(dpy0, None, NULL);
    glXDestroyContext(dpy0, glxcontext0);
    glxcontext0 = NULL;
  }
}

/* Pre-alpha dialog interface.  note: The plan is to make a parent X window and
   then one child for interfacing nodes and one for dialog.  The second child's
   pointer is then passed to a setup function in dialog*.c for use and further
   wrapping of any new dialog window. */
void
setup_dialog_interface(void)
{
  /* pre-alpha dialog version */
  add_dialog(diagtext0, diagtextmsg, diagtext0->dialog_len, 0);
}

/* Shutdown for given dialog interface. */
void
shutdown_dialog_interface(void)
{
  ;
}

/* Given argc/argv, run vrtater statefully while runnode equals 0. */
void
peer(int argc, char **argv)
{
  XEvent xevent;

  setup_glx(argc, argv);
  orgin_desc = generate_node_orgin();

  fov0 = p_hmapf(0); /* for now */
  fov0_available = 1; /* will be default in vanilla config file */

  /* For pre-alpha dialog. */
  diagtext0 = fov0; /* default */

  init_tug_io(); /* if any tug tend to it /w start_tug(init_tug_io()) */

  fps = 28.6; /* guess */
  vrt_render_cyc = .034965;
  frames = 0;
  run = 0;
  sfreq = 1000;
  infcount = 0;
  reads = 0;

  /* hmap with fov for dpy0. */
  fov0->ang_spd = 0; /* for now not used for fov hmap */
  fov0->ang_dpl = 0; /* for now not used for fov hmap */
  set_vf(&(fov0->vvel), 0, 0, 0, 0);
  set_vf(&(fov0->vaxi), 0, 0, -1, 1); /* oa */
  set_vf(&(fov0->vrel), 0, 1, 0, 1); /* up locally */
  set_vf(&(fov0->vpos), 0, 0, 0, 0); /* oa@fp */

  /* Some hmaps for diagnostic purpose follow. */
  ryg = p_hmapf(1);
  ryg->ang_spd = .25;
  ryg->ang_dpl = 0;
  set_vf(&(ryg->vvel), 0, 0, 0, 0);
  set_vf(&(ryg->vaxi), .707106, .707106, 0, 1);
  set_vf(&(ryg->vpos), 0, 495, -10, 0);
  form_mag_vf(&(ryg->vpos));

  ryg1 = p_hmapf(2);
  ryg1->ang_spd = .25;
  ryg1->ang_dpl = 3.141592;
  set_vf(&(ryg1->vvel), 0, 0, 0, 0);
  set_vf(&(ryg1->vaxi), 0, .707106, .707106, 1);
  set_vf(&(ryg1->vpos), 0, 495, 10, 0);
  form_mag_vf(&(ryg1->vpos));

  diag1 = p_hmapf(3);
  diag1->ang_spd = .000012;
  set_vf(&(diag1->vvel), 0, .1, 0, .1);
  set_vf(&(diag1->vaxi), 0, 0, -1.0, 1);
  set_vf(&(diag1->vpos), 70.71, 70.71, 70.71, 10);

  diag2 = p_hmapf(4);
  diag2->ang_spd = .000010;
  set_vf(&(diag2->vvel), 0, .1, 0, .1);
  set_vf(&(diag2->vaxi), 0, 0, -1.0, 1);
  set_vf(&(diag2->vpos), 70.71, 70.71, -70.71, 10);

  diag3 = p_hmapf(5);
  diag3->ang_spd = .000015;
  set_vf(&(diag3->vvel), 0, .1, 0, .1);
  set_vf(&(diag3->vaxi), 0, 0, -1.0, 1);
  set_vf(&(diag3->vpos), 70.71, -70.71, 70.71, 10);

  /* diag: std basis */
  diag4 = p_hmapf(6);
  diag4->ang_spd = 0;
  set_vf(&(diag4->vvel), 0, 0, 0, 0);
  set_vf(&(diag4->vaxi), 0, 0, -1.0, 1);
  set_vf(&(diag4->vpos), 0, 0, 0, 0);
  cp_vf(&isb, &(diag4->vpos));

  diag5 = p_hmapf(7);
  diag5->ang_spd = 0;
  set_vf(&(diag5->vvel), 0, 0, 0, 0);
  set_vf(&(diag5->vaxi), 0, 0, -1.0, 1);
  set_vf(&(diag5->vpos), 0, 0, 0, 0);
  cp_vf(&jsb, &(diag5->vpos));

  diag6 = p_hmapf(8);
  diag6->ang_spd = 0;
  set_vf(&(diag6->vvel), 0, 0, 0, 0);
  set_vf(&(diag6->vaxi), 0, 0, -1.0, 1);
  set_vf(&(diag6->vpos), 0, 0, 0, 0);
  cp_vf(&ksb, &(diag6->vpos));

  /* set of cubes 9 - 18 */
  int i;
  for (i = 9; i < 19; i++)
  {
    set_vf(&(p_hmapf(i)->vpos),
      p_hmapf(i)->vpos.x + (i * 10000) / (i * i),
      p_hmapf(i)->vpos.y + (i * 10000) / (i * i),
      p_hmapf(i)->vpos.z + (i * 10000) / (i * i),
      0);
    form_mag_vf(&(p_hmapf(i)->vpos));
  }

  /* cylinder stock */
  diag8 = p_hmapf(19);
  diag8->ang_spd = 0;
  diag8->ang_dpl = M_PI_2;
  set_vf(&(diag8->vvel), 0, 0, 0, 0);
  set_vf(&(diag8->vaxi), 0, .707106, .707106, 1);
  set_vf(&(diag8->vpos), 70.71, 70.71, 70.71, 122.473312);
  cp_vf(&vrloc8, &(diag8->vpos));

  /* Interface nodes. */
  while (runnode) {
    while (XPending(dpy0)) {
      XNextEvent(dpy0, &xevent);
      switch (xevent.type) {

        case KeyPress:
        switch (XKeycodeToKeysym(dpy0, xevent.xkey.keycode, 0)) {

          case VRT_KEY_tab:
          if (diagtext)
            diag_char(VRT_KEYCODE_tab);
          else
            ;
          break;

          case VRT_KEY_linefeed:
          if (diagtext) {
            diagtext = 0;
            diag_char('\n');
          }
          break;

          case VRT_KEY_esc:
          diag_message();
          break;

          case VRT_KEY_space:
          if (diagtext)
            diag_char(VRT_KEYCODE_space);
          else {
            (&ifdpy0)->keypan *= (&ifdpy0)->adclpan;
            (&ifdpy0)->keytilt *= (&ifdpy0)->adcltilt;
            (&ifdpy0)->keyroll *= (&ifdpy0)->adclroll;
            (&ifdpy0)->keyvfwd *= (&ifdpy0)->dclvfwd;
            (&ifdpy0)->keyvside *= (&ifdpy0)->dclvside;
            (&ifdpy0)->keyvvrt *= (&ifdpy0)->dclvvrt;
          }
          break;

          case VRT_KEY_comma:
          if (diagtext)
            diag_char(VRT_KEYCODE_comma);
          else
            (&ifdpy0)->keyvvrt -= (&ifdpy0)->accvvrt * (&ifdpy0)->accel_adv;
          break;

          case VRT_KEY_0:
          if (diagtext)
            diag_char(VRT_KEYCODE_0);
          else
            diagtext0 = p_hmapf(0);
          break;

          case VRT_KEY_1:
          if (diagtext)
            diag_char(VRT_KEYCODE_1);
          else
            diagtext0 = p_hmapf(1);
          break;

          case VRT_KEY_2:
          if (diagtext)
            diag_char(VRT_KEYCODE_2);
          break;

          case VRT_KEY_3:
          if (diagtext)
            diag_char(VRT_KEYCODE_3);
          break;

          case VRT_KEY_4:
          if (diagtext)
            diag_char(VRT_KEYCODE_4);
          break;

          case VRT_KEY_5:
          if (diagtext)
            diag_char(VRT_KEYCODE_5);
          break;

          case VRT_KEY_6:
          if (diagtext)
            diag_char(VRT_KEYCODE_6);
          break;

          case VRT_KEY_7:
          if (diagtext)
            diag_char(VRT_KEYCODE_7);
          break;

          case VRT_KEY_8:
          if (diagtext)
            diag_char(VRT_KEYCODE_8);
          break;

          case VRT_KEY_9:
          if (diagtext)
            diag_char(VRT_KEYCODE_9);
          break;

          case VRT_KEY_semicolon:
          if (!diagtext) {
            (&ifdpy0)->keyroll -= (&ifdpy0)->aaccroll * (&ifdpy0)->aaccel_adv;
            if ((&ifdpy0)->keyroll < -M_PI)
              (&ifdpy0)->keyroll = M_PI;
          }
          break;

          case VRT_KEY_equal:
          if (diagtext)
            diag_char(VRT_KEYCODE_equal);
          break;

          case VRT_KEY_backslash:
          if (!diagtext) {
            diagtext = 1;
            if (!dialogrecurrant++) {
              setup_dialog_interface();
            }
          }
          break;

          case VRT_KEY_a:
          if (diagtext)
            diag_char(VRT_KEYCODE_a);
          else {
            (&ifdpy0)->keypan += (&ifdpy0)->aaccpan * (&ifdpy0)->aaccel_adv;
            if ((&ifdpy0)->keypan > M_PI)
              (&ifdpy0)->keypan = -M_PI;
          }
          break;

          case VRT_KEY_b:
          if (diagtext)
            diag_char(VRT_KEYCODE_b);
          break;

          case VRT_KEY_c:
          if (diagtext)
            diag_char(VRT_KEYCODE_c);
          else
            (&ifdpy0)->keyvside += (&ifdpy0)->accvside * (&ifdpy0)->accel_adv;
          break;

          case VRT_KEY_d:
          if (diagtext)
            diag_char(VRT_KEYCODE_d);
          else {
            (&ifdpy0)->keypan -= (&ifdpy0)->aaccpan * (&ifdpy0)->aaccel_adv;
            if ((&ifdpy0)->keypan < -M_PI)
              (&ifdpy0)->keypan = M_PI;
          }
          break;

          case VRT_KEY_e:
          if (diagtext)
            diag_char(VRT_KEYCODE_e);
          break;

          case VRT_KEY_f:
          if (diagtext)
            diag_char(VRT_KEYCODE_f);
          else {
            diag_peer_key_f();
          }
          break;

          case VRT_KEY_g:
          if (diagtext)
            diag_char(VRT_KEYCODE_g);
          else {
            diag_peer_key_g();
          }
          break;

          case VRT_KEY_h:
          if (diagtext)
            diag_char(VRT_KEYCODE_h);
          else {
            diag_peer_key_h();
          }
          break;

          case VRT_KEY_i:
          if (diagtext)
            diag_char(VRT_KEYCODE_i);
          break;

          case VRT_KEY_j:
          if (diagtext)
            diag_char(VRT_KEYCODE_j);
          else {
            runnode = 0; /* for now */
          }
          break;

          case VRT_KEY_k:
          if (diagtext)
            diag_char(VRT_KEYCODE_k);
          else {
            (&ifdpy0)->keyroll += (&ifdpy0)->aaccroll * (&ifdpy0)->aaccel_adv;
            if ((&ifdpy0)->keyroll > M_PI)
              (&ifdpy0)->keyroll = -M_PI;
          }
          break;

          case VRT_KEY_l:
          if (diagtext)
            diag_char(VRT_KEYCODE_l);
          else {
            (&ifdpy0)->keytilt += (&ifdpy0)->aacctilt * (&ifdpy0)->aaccel_adv;
            if ((&ifdpy0)->keytilt > M_PI)
              (&ifdpy0)->keytilt = -M_PI;
          }
          break;

          case VRT_KEY_m:
          if (diagtext)
            diag_char(VRT_KEYCODE_m);
          break;

          case VRT_KEY_n:
          if (diagtext)
            diag_char(VRT_KEYCODE_n);
          break;

          case VRT_KEY_o:
          if (diagtext)
            diag_char(VRT_KEYCODE_o);
          else {
            (&ifdpy0)->keytilt -= (&ifdpy0)->aacctilt * (&ifdpy0)->aaccel_adv;
            if ((&ifdpy0)->keytilt < -M_PI)
              (&ifdpy0)->keytilt = M_PI;
          }
          break;

          case VRT_KEY_p:
          if (diagtext)
            diag_char(VRT_KEYCODE_p);
          else
            (&ifdpy0)->keyvvrt += (&ifdpy0)->accvvrt * (&ifdpy0)->accel_adv;
          break;

          case VRT_KEY_q:
          if (diagtext)
            diag_char(VRT_KEYCODE_q);
          break;

          case VRT_KEY_r:
          if (diagtext)
            diag_char(VRT_KEYCODE_r);
          break;

          case VRT_KEY_s:
          if (diagtext)
            diag_char(VRT_KEYCODE_s);
          else
            (&ifdpy0)->keyvfwd -= (&ifdpy0)->accvfwd * (&ifdpy0)->accel_adv;
          break;

          case VRT_KEY_t:
          if (diagtext)
            diag_char(VRT_KEYCODE_t);
          break;

          case VRT_KEY_u:
          if (diagtext)
            diag_char(VRT_KEYCODE_u);
          break;

          case VRT_KEY_v:
          if (diagtext)
            diag_char(VRT_KEYCODE_v);
          break;

          case VRT_KEY_w:
          if (diagtext)
            diag_char(VRT_KEYCODE_w);
          else
            (&ifdpy0)->keyvfwd += (&ifdpy0)->accvfwd * (&ifdpy0)->accel_adv;
          break;

          case VRT_KEY_x:
          if (diagtext)
            diag_char(VRT_KEYCODE_x);
          break;

          case VRT_KEY_y:
          if (diagtext)
            diag_char(VRT_KEYCODE_y);
          break;

          case VRT_KEY_z:
          if (diagtext)
            diag_char(VRT_KEYCODE_z);
          else
            (&ifdpy0)->keyvside -= (&ifdpy0)->accvside * (&ifdpy0)->accel_adv;
          break;

          case VRT_KEY_del:
          if (diagtext) {
            write_dialog(diagtext0, "");
          }
          break;
        }
        break;

        case ButtonPress:
          ;
        break;

        case ButtonRelease:
        break;

        case MotionNotify:
        break;
      }
    }

    /* Sum in curvilinear fore/backwards acceleration feedback. */
    tele_magz_vf(&(fov0->vvel), (fov0->vvel.m * fov0->vvel.m) / (fov0->vvel.m + (fov0->vvel.m * (&ifdpy0)->accel_crv)), &(fov0->vvel));

    /* Accelerate, summing d/t/t with d/t for (+/-)fwd, side. */
    vf_t dpl, dpl2, dpl3;
    tele_magz_vf(&(fov0->vaxi), (&ifdpy0)->keyvfwd, &dpl);
    sum_vf(&dpl, &(fov0->vvel), &(fov0->vvel));

    cprod_vf(&(fov0->vaxi), &(fov0->vrel), &dpl2);
    tele_magz_vf(&dpl2, (&ifdpy0)->keyvside, &dpl2);
    sum_vf(&(fov0->vvel), &dpl2, &(fov0->vvel));
    tele_magz_vf(&(fov0->vrel), (&ifdpy0)->keyvvrt, &dpl3);
    sum_vf(&(fov0->vvel), &dpl3, &(fov0->vvel));

    /* Further adjust interfaced hmaps while representing node output.  Per
       display(dpy) per frame, set the field of view (fov) per the hmap's
       holding any.  A focal plane is represented by vectors vrel and side, and
       an optical axis by vector vaxi.  Roll is represented by a combination of
       vrel and side.  vpos is represented when applied by the renderer to a
       matrix translation of the inverse of fov0->vpos before rendering, and
       back again before returning. */

    /* Calculate a basis vector for tilt. */
    vf_t side;
    cprod_vf(&(fov0->vaxi), &(fov0->vrel), &side);
    normz_vf(&side, &side);

    /* Roll rel and side around axial. */
    rotate_vf(&(fov0->vrel), &(fov0->vaxi), (&ifdpy0)->keyroll, &(fov0->vrel));
    rotate_vf(&side, &(fov0->vaxi), (&ifdpy0)->keyroll, &side);

    /* Pan side and axial around rel. */
    rotate_vf(&side, &(fov0->vrel), -(&ifdpy0)->keypan, &side);
    rotate_vf(&(fov0->vaxi), &(fov0->vrel), -(&ifdpy0)->keypan, &(fov0->vaxi));

    /* Tilt axial and rel around side. */
    rotate_vf(&(fov0->vaxi), &side, -(&ifdpy0)->keytilt, &(fov0->vaxi));
    rotate_vf(&(fov0->vrel), &side, -(&ifdpy0)->keytilt, &(fov0->vrel));

    /* Base's are maintained in their normalized form. */
    normz_vf(&(fov0->vaxi), &(fov0->vaxi));
    normz_vf(&(fov0->vrel), &(fov0->vrel));

    /* fov0 is passed and rendered first to set fp_oa. */
    init_next_buffer();
    proc_hmapf(fov0, VRT_MASK_LOD_INF, 1);

    /* Regenerate next frame's worth of hmaps modifying hmap position vs. vvel,
       and (soon rotation/)vpos through intersection of hmaps.  Lod envelopes
       are centered around fov0->vpos'.  Just above, hmap fov0 was processed vs.
       the near lod envelope so it is skipped in regenerate_scene. */
    regenerate_scene(&(fov0->vpos));

    /* Rotate scene rendered vs. fov0->vpos'(set in last call). */
    glRotatef((&ifdpy0)->keyroll * 180 / M_PI,
      fov0->vaxi.x, fov0->vaxi.y, fov0->vaxi.z);
    glRotatef(-(&ifdpy0)->keypan * 180 / M_PI,
      fov0->vrel.x, fov0->vrel.y, fov0->vrel.z);
    glRotatef(-(&ifdpy0)->keytilt * 180 / M_PI,
      side.x, side.y, side.z);

#ifdef DIAG_INTERFACE
    __builtin_printf("\nfov0\n");
    __builtin_printf("  vpos: x %f y %f z %f m %f\n",
      fov0->vpos.x, fov0->vpos.y,
      fov0->vpos.z, fov0->vpos.m);
    __builtin_printf("  vaxi: x %f y %f z %f m %f\n\n",
      fov0->vaxi.x, fov0->vaxi.y,
      fov0->vaxi.z, fov0->vaxi.m);
    __builtin_printf("  vvel: x %f y %f z %f m %f\n",
      fov0->vvel.x, fov0->vvel.y,
      fov0->vvel.z, fov0->vvel.m);
    __builtin_printf("   vfwd: x %f y %f z %f m %f\n",
      dpl.x, dpl.y, dpl.z, dpl.m);
    __builtin_printf("  vside: x %f y %f z %f m %f\n",
      dpl2.x, dpl2.y, dpl2.z, dpl2.m);
    __builtin_printf("   vvrt: x %f y %f z %f m %f\n\n",
      dpl3.x, dpl3.y, dpl3.z, dpl3.m);

    __builtin_printf("kbd\n");
    __builtin_printf("   roll(k/;) %f  pan(a/d) %f tilt(o/l) %f\n"
      "   vfwd(w/s) %f vvrt(p/,) %f vside(z/c) %f\n"
      "   decel(space) dialog(\\) pre-alpha exit(j)\n\n"
      "dialog\n", (&ifdpy0)->keyroll, (&ifdpy0)->keypan,
      (&ifdpy0)->keytilt, (&ifdpy0)->keyvfwd,
      (&ifdpy0)->keyvvrt, (&ifdpy0)->keyvside);

    select_t textbuf = { 0, 1, (hmapf_t **) selectf_a, 0, NULL };
    hmapf_t **map = (&textbuf)->seta;
    *map = diagtext0;
    node_orgin_dialog(&textbuf);
#endif

#ifdef DIAG_TIME
    /* fps: Sample interval sfreq, set above, should be proportional considering
       the sample granularity used.  At the moment this is disabled pending
       further reading.  For now using vrt_render_cyc default value as scale for
       state increment.  vrtater time dependant calculations will likely be
       determined primarily vs. the video hardware. */
    if (!(frames++ % sfreq)) {
      if (!run++) {
        start = time(NULL);
      } else {
        run -= 2;
        stop = time(NULL);
        fps = (float) frames / (float) (stop - start);
        if (!isinf(fps)) {
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
#endif

    /* Render. */
    glPushMatrix();
    render_vobspace(fov0_available);
    glPopMatrix();

    /* Draw to dpy0. */
    if (dbuff)
      glXSwapBuffers(dpy0, xwin0);
    else
      glFlush();

    /* Start of new frame.  Not clearing identity matrix.  Will
       let it stack.  A function to periodically align hmap's
       will follow at some point, clearing the identity matrix. */
    glMatrixMode(GL_MODELVIEW);
  }

  /* Shutdown node-orgin. */
  __builtin_printf("Program exit called\n");
  close_vobspace(0); /* now, for now */
  session_nodemask = 0;
  reset_sessions();
  close_node_orgin(); /* note: move to callback_close_vobspace() */
  shutdown_glx();
}

/* Tend to all_sessions data referencing session info generated thru session.c
   selection of available nodes for vrtlogin continuity and partial. */
void
tendto_curr_sessions(void)
{
  /* Conditionally connect_vobspace(), etc... */
}

/* After receiving nodemap, connect session session.  notes: session represents
   a node-partial to be transfered from a caller node-orgin.  This will be
   obvious from the directional sense visible looking at all_sessions data from
   within the interface to be provided herein.  Remote nodes added to the
   running set for session node-partial will maintain their own representation
   of given node-partial.  Success is assumed while implied session_t remains
   in all_sessions data. */
int
connect_to_peers(session_t *session)
{
  /* This now completed in test_accept_continuing_session. */

  return 0;
}

/* Maintain reputation for session. */
int
maintain_reputation(session_t *session)
{
  return 0;
}

/* Temporary diagnostic to append c to the hmap dialog of the first element of
   selectf_a. */
void
diag_char(char c)
{
  int sc[2] = { '\0', '\0' };

  *sc = (int) c;
  add_dialog(diagtext0, (char *) sc, diagtext0->dialog_len, 0);
}

/* Temporary diagnostic to run test on keypress f. */
void
diag_peer_key_f(void)
{
  diag_generator_key_f();

#ifdef DIAG_CONTINUING_ENABLE
  /* vrtlogin to flexible node from continuing node. */
  struct ptlrepute *repute;
  char url_c[] = "protocol://192.168.0.2/nothernode/"; /* refers to */
  char oneliner_c[] = "nothernode";
  hmapf_t *avatar555, *avatar555_hold, *flow_map = NULL;
  session_t lastkey = { { 0, 0, 0xb0de }, 8 };
  session_t contingentkey = { { 0, 0, 0xface }, 8 };
  session_t holdkey = { { 0, 0, 0 }, 0 };
  complextimate_t cmplxt_here = { 0, 0, 0 };
  hmapf_t **sela = (hmapf_t **) selectf_a; /* returned nodemap ref. */
  select_t sel = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };
  int rval;

  __builtin_printf("Simulated vrtlogin to flexible node\n");
  struct partial *node_c = NULL;
  if ((node_c = test_continuity(url_c, oneliner_c, NULL, sela, &cmplxt_here)) == NULL) {
    __builtin_fprintf(stderr, "Error: Failed to provide "
      "continuing node with continuity\n");
    return;
  }

  /* Now that continuity exists, enter continuing with selectf_a maps. */
  test_add_maps(5, 0, &(node_c->session), &(node_c->nodemap->vpos), &sel, &cmplxt_here);

  /* For testing avatar555 volunteers. */
  avatar555 = p_hmapf(23);
  cp_session(&(node_c->session), &(avatar555->name));
  avatar555->attribs.sign |= VRT_MASK_KEYMAP;
  avatar555_hold = p_hmapf(24);
  cp_session(&(node_c->session), &(avatar555_hold->name));
  avatar555_hold->attribs.sign |= VRT_MASK_HOLD;

#ifdef DIAG_CONTINUING_OTHRHOLDKEY
  /* Set an initial holdkey on continuing for a continued session.
     Value indicates it was set >1 flexible sessions ago. */
  (&holdkey)->hash.l = 0xface;
  (&holdkey)->seq = 555;
#endif

#ifdef DIAG_CONTINUING_SETHOLDKEY
  /* Ask flexible to try to publicize held maps having VRT_MASK_PUB */
  flow_map = avatar555_hold;
#endif

#ifdef DIAG_CONTINUING_FLEXIBLE_SENDS_NEWREPUTED
  /* Sign that a new reputation is requested.  Passing a flag forward for
     this is not possible as the defines for what the flexible feeds back
     simulate an assumed state of the flexible's data.  This just to make
     the diagnostic look correct.  When the ip network code is added,
     the implied defines would be removed and then this could work. */
  zero_fullname(&lastkey);
  zero_fullname(&contingentkey);
#endif

  /* Add repute for avatar555 in node-orgin reputation list providing
     loginkeys at url.  Since this is setup to be a continued session,
     find_loginkeys would normally be used for same.  Sync for this
     is provided in continue_node. */
  repute = add_ptlrepute(node_c->reputed, &lastkey, &holdkey, url_c);
  cp_mapname(&contingentkey, &(repute->contingentkey)); /* established */
  cp_mapname(&holdkey, &(repute->holdkey)); /* with holdkey */
  /* Continue. */
  if ((rval = test_continue_partial(node_c, url_c, node_c->reputed, repute, &(node_c->session), avatar555, flow_map, calc_cmplxt(&cmplxt_here))) != 0)
    ; /* could not vrtlogin */
#endif /* DIAG_CONTINUING_ENABLE */

#ifdef DIAG_FLEXIBLE_ENABLE
  /* Make a flexible node. */
  struct partial *node_f = NULL;
  char url_f[] = "protocol://192.168.0.2/nothernode/"; /* write index */
  char oneliner_f[] = "nothernode"; /* 79 chars max */
  session_t null = { { 0, 0, 0 }, 0 };
  complextimate_t cmplxt = { 0, 0, 0 };
  char seedfiles[] = "";
  hmapf_t *map, **maps = (hmapf_t **) selectf_a;
  select_t t = { 0, 1, maps, 0, NULL};

  __builtin_printf("Simulated creation of flexible node "
    "\"nothernode\"\n makeing an flexible node in node-orgin\n");

  /* Outside map. */
  if ((map = hmapf_cylinder_c(&(orgin_desc)->session, 80.5, 25, 112, 0)) != NULL) {
    map->ang_spd = 0;
    map->ang_dpl = -.761799;
    set_vf(&(map->vvel), 0, 0, 0, 0);
    form_mag_vf(set_vf(&(map->vaxi), .5, 1, 0, 0));
    form_mag_vf(set_vf(&(map->vpos), 200, 500, 0, 0));
    map->attribs.mode |= VRT_MASK_FIXED_FORM;
  }
  /* nodemap. */
  if ((map = hmapf_cylinder_c(&(orgin_desc)->session, 80, 25, 111.5, 0)) != NULL) {
    map->ang_spd = 0;
    map->ang_dpl = -.761799;
    set_vf(&(map->vvel), 0, 0, 0, 0);
    form_mag_vf(set_vf(&(map->vaxi), .5, 1, 0, 0));
    form_mag_vf(set_vf(&(map->vpos), 200, 500, 0, 0));
    map->attribs.mode |= VRT_MASK_FIXED_FORM;
  }
  *maps = map;
#ifdef DIAG_UNDEFINED
  surface_inv_hmapf(&t);
#endif

  node_f = test_mk_flexible_node(seedfiles, maps, &cmplxt);

  /* note: ptlrepute pointer is added to description as code in session.c
     calls sync_loginkeys for a flexible node. */
  add_session_desc(&(node_f->session), &null, &null, 0, url_f, oneliner_f, &cmplxt, map, node_f->reputed);
#endif /* DIAG_FLEXIBLE_ENABLE */

#ifdef DIAG_RECEIVE_MAP
  hmapf_t *nodemap;
  session_t session = {{ 0, 0, 0x1aff5000 }, 0 };
  hmapf_t **a = (hmapf_t **) selectf_a;
  hmapf_t **b = (hmapf_t **) selectf_b;
  select_t receiver = { 0, 1, a, 0, b };

  nodemap = diag_receive_nodemap(&session, &receiver);
#endif
}

/* Temporary diagnostic to run test on keypress g. */
void
diag_peer_key_g(void)
{
  diag_generator_key_g();

#ifdef DIAG_FLEXIBLE_ENABLE
  __builtin_printf("\nSimulate received vrtlogin to \"nothernode\" "
    "after generating flexible.\n");
  struct partial *flexible;
  char url[] = "protocol://192.168.0.2/nothernode/"; /* index */
  session_t session_peer = { { 0, 0, 0xc0de }, 0 };
  session_t null = { { 0, 0, 0 }, 0 };
  struct session_desc *desc_here;
  complextimate_t cmplxt_here = { 0, 0, 0 };
  hmapf_t *nother_hold;
  vf_t portal, d = { 1., 0., 0., 1. };
  select_t sel = {  0, 0, (hmapf_t **) selectf_a, 0, NULL };

  if ((desc_here = find_url(url)) == NULL) {
    printf("desc_here not found\n");
    return;
  }
printf("session %x\n", desc_here->session.hash.l);
  if ((flexible = find_partial(&(desc_here->session))) == NULL) {
    printf("pointer %x flexible not found\n", (int) flexible);
    return;
  }
  cp_vf(&(flexible->nodemap->vpos), &portal);

  /* Simulate provision for flexible. */
  __builtin_printf(" adding some hmaps to flexible\n");
  test_add_maps(1, 0, &(flexible->session), &portal, &sel, &cmplxt_here);
  nother_hold = p_hmapf(26);
  nother_hold->attribs.sign |= VRT_MASK_HOLD;
  nportf(nother_hold, sum_vf(&portal, &d, &portal), 0);

  /* Complete session description enabling logins. */
  desc_here->cmplxt.hmap_count = 8;
  desc_here->cmplxt.tl_vdata = 288;
  desc_here->cmplxt.tl_dialog = 314;
  desc_here->nodemap = flexible->nodemap;
  desc_here->reputed = flexible->reputed;
  desc_here->level = VRT_MASK_SESSION_DETACHED | VRT_MASK_SESSION_ENABLE;

  session_nodemask |= VRT_MASK_ACCEPT_VRTLOGIN; /* temporarily here */
  diag_ls_all_sessions(1);
  __builtin_printf(" accepting vrtlogins on flexible (%x %x %x)...\n",
    flexible->session.hash.h, flexible->session.hash.m,
    flexible->session.hash.l);

  /* Now that a flexible node is in shape, a vrtlogin arrives.  The data sent
     contains the url for the node requested, the session of the peer node and
     perhaps the session of a thru node.  continuing node vrtlogging in will
     show up in all_sessions as a peer.  VRT_MASK_ACCEPT_VRTLOGIN bit for all
     local flexible nodes has already been set to true through ifnode**.c
     interface part.  With received data, continuity may be retreived.  note:
     answer_vrtlogin would normally be called by code in session.c. */
  answer_vrtlogin(&session_peer, &null, url); /* url here indexes desc */
#endif /* DIAG_FLEXIBLE_ENABLE */
}

/* Temporary diagnostic to run test on keypress h. */
void
diag_peer_key_h(void)
{
  diag_generator_key_h();

#ifdef DIAG_FLEXIBLE_ENABLE
  /* Simulate phase-b vrtlogin data received by code in session.c.  Imediately
     after data is sent a password may be requested.  note: flexible side here
     represented assumed to have reputation for avatar555 whom is vrtlogging. */
  complextimate_t cmplxt = { 1, 555, 3 }; /* avatar555's hmaps */
  session_t null = { { 0, 0, 0 }, 0 };
  int rval;

  /* Received before sync_vrtlogin is called. */
  char url[] = "protocol://192.168.0.2/nothernode/";
  session_t session_peer = { { 0, 0, 0xc0de }, 0 };
  session_t loginkey = { { 0, 0, 0xc0de }, 8 };
  session_t lastkey = { { 0, 0, 0xb0de }, 8 };
  session_t contingentkey = { { 0, 0, 0xface }, 8 };
  session_t holdkey = { { 0, 0, 0 }, 0 };
  unsigned int tl_cmplxt = calc_cmplxt(&cmplxt);

  /* Error values for diagnostic. */
  session_t anomalesslast = { { 0xf, 0xade, 0xd }, 8 };
  session_t anomalesscontingent = { { 0xf, 0xade, 0xd }, 8 };

#ifdef DIAG_FLEXIBLE_LASTKEYERR
  /* Simulate keysync error from a disk or memory related event.  This
     non-contingent key was successfully used with vrtlogin previously, however
     it has an error on at least one side of partial somehow, likely after a
     write to disk or memory, or having never been written to disk before a
     reset or power-off.  Since this is a match test, inverse is also valid. */
  cp_mapname(&anomalesslast, &lastkey);
#endif

#ifdef DIAG_FLEXIBLE_CONTINGENTKEYERR
  /* Simulate keysync error from a disk or memory related event.  This key was
     successfully used possibly becoming shifted (system may have gone down
     during shift).  When possibly written to disk then, or in memory on either
     either side of partial, it somehow was in error.  As test implies an error
     in lastkey it is also in err. */
  cp_mapname(&anomalesslast, &lastkey);
  cp_mapname(&anomalesscontingent, &contingentkey);
#endif

#ifdef DIAG_FLEXIBLE_NEWREPUTED
  zero_fullname(&lastkey);
  zero_fullname(&contingentkey);
#endif

#ifdef DIAG_FLEXIBLE_MAPKEY_REDUNDANT
  /* Use a loginkey that is already present in reputation data */
  if (rand() % 2)
    cp_mapname(&lastkey, &loginkey);
  else
    cp_mapname(&contingentkey, &loginkey);
#endif

#ifdef DIAG_FLEXIBLE_SETHOLDKEY
  holdkey.hash.l = 0xc0de;
  holdkey.seq = 555;
#endif

#ifdef DIAG_FLEXIBLE_CMPLXT
  (&cmplxt)->hmap_count = 9385;
  (&cmplxt)->tl_vdata = 903329;
  (&cmplxt)->tl_dialog = 90374043;
#endif

  /* phase-b data arrives.  On syncronization, nodes may become partial
     or continuing may join partial. */
  __builtin_printf("Reputation data received for session (%x %x %x), "
    "meanwhile,\n login authentication has succeeded.\n",
    (&session_peer)->hash.h, (&session_peer)->hash.m,
    (&session_peer)->hash.l);
  if ((rval = sync_vrtlogin(&session_peer, &null, &loginkey, &lastkey, &contingentkey, &holdkey, calc_cmplxt(&cmplxt), url)) < 0) {
    /* Contingency for errors. */
    if (rval == -1)
      __builtin_printf(" ...size of selected hapmaps "
        "exceeds flexible setting.\nSync failed.  "
        "Awaiting retry.\n");
  } else
    /* Contingency for keyuse. */
    ;
#endif /* DIAG_FLEXIBLE_ENABLE */
}

/* Diagnostic test: Make a detached flexible node with a session name based on
   given seedfiles and placing hmaps referenced by maps therein.  note: For a
   flexible session, mk_partial takes the session name of the first map
   referenced by maps.  For now mk_partial can only receive a non compounded
   hmap. */
struct partial *
test_mk_flexible_node(char *seedfiles, hmapf_t **maps, complextimate_t *cmplxt)
{
  struct partial *node = NULL;
  session_t session;

  hash_session_name(&session, seedfiles);
  cp_session(&session, &((*maps)->name));
  __builtin_printf(" adding detached flexible node in node-orgin\n");
  node = mk_partial(NULL, maps, 1, cmplxt);

  return node;
}

/* Diagnostic test: Try to retreive continuity for flexible session described
   in reputation orgin_repute.  Working in tandem with the interface part,
   vrtlogin using keymap.  */
void
test_continue_repute(hmapf_t *keymap)
{
  ;
}

/* Diagnostic test: Call flexible node for nodemap and session details,
   allowing for cmplxt at url.  If successfull, continuing session receives
   continuity apon receiving nodemap. */
struct partial *
test_continuity(char *url, char *oneliner, session_t *thru, hmapf_t **maps, complextimate_t *cmplxt)
{
  struct partial *node = NULL;
  session_t session;
  char seedfiles[] = ""; /* directory containing maps for session */

  hash_session_name(&session, seedfiles);
  if ((node_continuity(url, oneliner, &session, NULL, maps)) != 0)
    return NULL;
  if (*maps) {
    /* Continuity received for session. */
    __builtin_printf("Adding continuing node in node-orgin...\n");
    node = mk_partial(&session, maps, 1, cmplxt);
  } else
    __builtin_printf("Error: node did not receive continuity at "
      "\"%s\".  Suspect DIAG_CONTINUING_SESSION_OFF.\n",
      url);

  return node;
}

/* Diagnostic test: After making continuing node, try to continue partial with
   flexible at url using new list with repute for keymap, using session_here
   for node, and provided flexible will accept cmplxt.  note: hmap keymap has
   entered node desiring vrtlogin, cmplxt reflecting maps selected. */
int
test_continue_partial(struct partial *node, char *url, struct ptlrepute_list *list, struct ptlrepute *repute, session_t *session_here, hmapf_t *keymap, hmapf_t *holdmap, unsigned int tl_cmplxt)
{
  session_t *flow_name, zero_session = { { 0, 0, 0 }, 0 };
  int rval;
  /* Using repute for keymap, continue a previous or new partial. */
  if ((rval = continue_node(url, list, repute, session_here, &(keymap->name), flow_name = holdmap ? &(holdmap->name) : &zero_session, tl_cmplxt)) == 0) {
    add_ptlmap(node->ptlmaps, keymap);
    __builtin_printf("Now representing partial.\n");
    diag_ls_all_sessions(1);
  } else {
    if (rval == -1)
      __builtin_printf("Continuity timed out.\n");
    else
      __builtin_printf("Could not resume partial.\n");
  }

  return rval;
}

/* Diagnostic test: Remove all partials currently defined. */
void
test_detach_all_partials(void)
{
  struct partial *current, *passed;

  current = partials->last;
  passed = partials->last;
  while (current != NULL) {
    rm_partial(current);
    passed = current;
    current = current->precursor;
  }
}

/* Temporary message about exit key sequence. */
void
diag_message(void)
{
  __builtin_printf("For now, please use j to exit "
    "vrtater GNU free software.\n"
    "Please send patches, or become a maintainer.\n "
    "Outreach drive for session.c now underway.\n"
    "Similar to follow for render**.c, dialog*.c ...\n");
}
