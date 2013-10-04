/* dialogX11.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_DIALOGX11_H
#define VRT_DIALOGX11_H

#include "hmap.h"

void refresh_dialog_interfaces(void);
void node_partial_dialog(select_t *, session_t *partial);
int *node_orgin_dialog(select_t *);
int maintain_reputation(session_t *);

#endif /* VRT_DIALOGX11_H */
