/* ifnodeglx.h 
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_IFNODEGLX_H
#define VRT_IFNODEGLX_H

struct interface_display {
	int indexfov; /* hmap dpy_ is attached to */
	float keyroll;
	float keyvfwd; /* accelerate summing v_axi * vfwd into v_vel' */
	float keypan;
	float keytilt;
};
typedef struct interface_display ifdpy_t;

void node(int argc, char **argv);

#endif /* VRT_IFNODEGLX_H */
