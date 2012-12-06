/* vrtater.c 'virtual' 'reality' engine. earliest edition. features missing.
   Copyright (C) 2012 J. A. Green <green8@sdf-eu.org>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "progscope.h"
#include "ifnodeglx.h"
#include "generator.h"
#include "tug.h"

extern unsigned int vrt_hmaps_max;

int
main(int argc, char **argv)
{
	setup_node(argc, argv);
	init_node(); /* your local node */
	init_tug_io();
	node(); /* is a state machine */
	close_node();
	shutdown_node();
}
