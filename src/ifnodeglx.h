/* ifnodeglx.h 
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_IFNODEGLX_H
#define VRT_IFNODEGLX_H

struct interface_display {
	int indexfov; /* hmap dpy_ is attached to */
	float keyroll;
	float keyvfore; /* accelerate summing v_axi * vfore into v_vel' */
	float keypan;
	float keytilt;
};
typedef struct interface_display ifdpy_t;

void setup_node(int argc, char **argv);
void node(void);
void shutdown_node(void);

#endif /* VRT_IFNODEGLX_H */
