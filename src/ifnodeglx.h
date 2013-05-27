/* ifnodeglx.h 
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_IFNODEGLX_H
#define VRT_IFNODEGLX_H

#include <X11/X.h>
#include "progscope.h"

struct interface_display {
	int indexfov; /* hmap dpy_ is attached to */
	float keypan;
	float keytilt;
	float keyroll;
	float keyvfwd; /* accelerate summing v_axi * vfwd into v_vel' */
	float keyvside; /* "" "" cprod(v_axi, v_rel) * vside into v_vel' */
	float keyvvrt; /* "" "" v_rel * vside into v_vel' */
};
typedef struct interface_display ifdpy_t;

/* for now */
#ifdef VRT_US101KBD_X11_DIAG
enum {
#define VRT_KEY_tab XK_Tab
	VRT_KEYCODE_tab = 9,  /* ^[tab]===comma,seperated,list.* delimiter */
#define VRT_KEY_linefeed XK_Return
	VRT_KEYCODE_linefeed = 10,
#define VRT_KEY_esc XK_Escape
	VRT_KEYCODE_esc = 27,
#define VRT_KEY_space XK_space /* decellerate */
	VRT_KEYCODE_space = 32,
#define VRT_KEY_comma XK_comma /* -keyvvrt */
	VRT_KEYCODE_comma = 44,
#define VRT_KEY_0 XK_0
	VRT_KEYCODE_0 = 48,
#define VRT_KEY_1 XK_1
	VRT_KEYCODE_1 = 49,
#define VRT_KEY_2 XK_2
	VRT_KEYCODE_2 = 50,
#define VRT_KEY_3 XK_3
	VRT_KEYCODE_3 = 51,
#define VRT_KEY_4 XK_4
	VRT_KEYCODE_4 = 52,
#define VRT_KEY_5 XK_5
	VRT_KEYCODE_5 = 53,
#define VRT_KEY_6 XK_6
	VRT_KEYCODE_6 = 54,
#define VRT_KEY_7 XK_7
	VRT_KEYCODE_7 = 55,
#define VRT_KEY_8 XK_8
	VRT_KEYCODE_8 = 56,
#define VRT_KEY_9 XK_9
	VRT_KEYCODE_9 = 57,
#define VRT_KEY_semicolon XK_semicolon /* -keyroll */
	VRT_KEYCODE_semicolon = 59,
#define VRT_KEY_equal XK_equal /* ^[tab]===comma,seperated,list.* delimiter */
	VRT_KEYCODE_equal = 61,
#define VRT_KEY_backslash XK_backslash /* +keypan */
	VRT_KEYCODE_backslash = 92,
#define VRT_KEY_a XK_a /* +keypan */
	VRT_KEYCODE_a = 97,
#define VRT_KEY_b XK_b
	VRT_KEYCODE_b = 98,
#define VRT_KEY_c XK_c /* +keyvside */
	VRT_KEYCODE_c = 99,
#define VRT_KEY_d XK_d /* -keypan */
	VRT_KEYCODE_d = 100,
#define VRT_KEY_e XK_e
	VRT_KEYCODE_e = 101,
#define VRT_KEY_f XK_f
	VRT_KEYCODE_f = 102,
#define VRT_KEY_g XK_g
	VRT_KEYCODE_g = 103,
#define VRT_KEY_h XK_h
	VRT_KEYCODE_h = 104,
#define VRT_KEY_i XK_i
	VRT_KEYCODE_i = 105,
#define VRT_KEY_j XK_j
	VRT_KEYCODE_j = 106,
#define VRT_KEY_k XK_k /* +keyroll */
	VRT_KEYCODE_k = 107,
#define VRT_KEY_l XK_l /* +keytilt */
	VRT_KEYCODE_l = 108,
#define VRT_KEY_m XK_m
	VRT_KEYCODE_m = 109,
#define VRT_KEY_n XK_n
	VRT_KEYCODE_n = 110,
#define VRT_KEY_o XK_o /* -keytilt */
	VRT_KEYCODE_o = 111,
#define VRT_KEY_p XK_p /* +keyvvrt */
	VRT_KEYCODE_p = 112,
#define VRT_KEY_q XK_q
	VRT_KEYCODE_q = 113,
#define VRT_KEY_r XK_r
	VRT_KEYCODE_r = 114,
#define VRT_KEY_s XK_s /* -keyvfwd */
	VRT_KEYCODE_s = 115,
#define VRT_KEY_t XK_t
	VRT_KEYCODE_t = 116,
#define VRT_KEY_u XK_u
	VRT_KEYCODE_u = 117,
#define VRT_KEY_v XK_v
	VRT_KEYCODE_v = 118,
#define VRT_KEY_w XK_w /* +keyvfwd */
	VRT_KEYCODE_w = 119,
#define VRT_KEY_x XK_x
	VRT_KEYCODE_x = 120,
#define VRT_KEY_y XK_y
	VRT_KEYCODE_y = 121,
#define VRT_KEY_z XK_z /* -keyvside */
	VRT_KEYCODE_z = 122,
#define VRT_KEY_del XK_Delete
	VRT_KEYCODE_del = 127
};
#endif /* VRT_US101KBD_X11_DIAG */
	
extern float vrt_render_cyc;

void node(int argc, char **argv);

#endif /* VRT_IFNODEGLX_H */
