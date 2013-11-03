/* global.h
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>
   license: GNU GPL v3, see COPYING, otherwise see vrtater.c
*/

#ifndef VRT_GLOBAL_H
#define VRT_GLOBAL_H

#define VRT_X_SUPPORT
#define VRT_RENDERER_GL
#define VRT_US101KBD_X11_DIAG
#define PI_180 0.017453292519943
#define ANG_AFS 1.697652 /* angular acc factor for regular solid sphere */

/* Patch diagnostic.  Used with keypress f, g, or h when in positional mode. */
#define DIAG_RECEIVE_MAP_OFF
#define DIAG_SEARCH_VOHSPACE_OFF
#define DIAG_CONTINUING_SESSION /* exclusive vs. DIAG_FLEXIBLE_SESSION */
#define DIAG_CONTINUING_REMOTE_SENDS_RETRY_OFF
#define DIAG_CONTINUING_REMOTE_SENDS_KEYERR_LAST_OFF
#define DIAG_CONTINUING_REMOTE_SENDS_KEYERR_BOTH_OFF
#define DIAG_FLEXIBLE_PARTIAL_OFF
#define DIAG_FLEXIBLE_SESSION_OFF /* exclusive vs. DIAG_CONTINUING_SESSION */
#define DIAG_FLEXIBLE_MAPKEY_REDUNDANT_OFF
#define DIAG_FLEXIBLE_MAPBKPKEY_REDUNDANT_OFF
#define DIAG_FLEXIBLE_KEYERR_OFF
#define DIAG_FLEXIBLE_BKPKEYERR_OFF
#define DIAG_FLEXIBLE_NEWREPUTE_OFF
#define DIAG_HMAP_MESSAGES_OFF
#define DIAG_FEED_SESSION_OFF
#define DIAG_GROUPS_OFF
#define DIAG_INTERFACE_OFF
#define DIAG_TIME_OFF
#define DIAG_INTERSECTION_OFF
#define DIAG_EFFECT
#define DIAG_HMAPWRAP_OFF
#define DIAG_STF_OFF
#define DIAG_CTF
#define DIAG_STB_OFF
#define DIAG_CTB_OFF
#define DIAG_FF_OFF
#define DIAG_FFS
#define DIAG_FB_OFF

#endif /* VRT_GLOBAL_H */
