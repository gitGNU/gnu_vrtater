/* session.h
   Copyright (C) 2012 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_SESSION_H
#define VRT_SESSION_H

#include "progscope.h"

void init_sessions(void);
void session_sync(void);
session_desc_t *p_session_desc(void);
prev_caller_sessions_t *p_prev_caller_sessions(void);
int in_node_session(session_t *);
void list_nodes(char *);
int call_session(char *);
int on_node_session(session_t *);
void close_sessions(session_t *);

#endif /* VRT_SESSION_H */
