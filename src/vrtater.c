/* vrtater.c: peer to peers 'virtual' 'reality' software.  in pre-alpha.
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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "progscope.h"
#include "ifnodeglx.h"
#include "generator.h"
#include "tug.h"

extern unsigned int vrt_hmaps_max;
static int verbose_flag;

int
main(int argc, char **argv)
{
	/* optional args */
	int c;
	while(1) {
		static struct option long_options[] = {
			/* flag setting options */
			{"verbose", no_argument, &verbose_flag, 1},
			{"brief", no_argument, &verbose_flag, 0},
			/* flagless options */
			{"non-flag-opt1", no_argument, 0, 0},
			{"non-flag-opt2", required_argument, 0, 0},
			/* indexed options */
			{"in-node", required_argument, 0, 'i'},
			{"find-node", required_argument, 0, 'f'},
			/* /w optional argument(requires -o=val or --opt=val) */
			{"diag", optional_argument, 0, 'd'},
			{0, 0, 0, 0}
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "i:f:d::", long_options, &option_index);
		if(c == -1)
			break;
		switch(c) {

			case 0:
			if(long_options[option_index].flag != 0) {
				__builtin_printf("setting --%s\n",
					long_options[option_index].name);
				break;
			}
			__builtin_printf("tend to flagless long-only option %s",
				long_options[option_index].name);
			if(optarg)
				__builtin_printf(" with %s as arg", optarg);
			__builtin_printf("\n");
			break;

			case 'i':
			if(optarg)
				__builtin_printf("Setting in-node to %s\n", optarg);
			break;

			case 'f':
			if(optarg)
				__builtin_printf("Searching for node%s\n", optarg);
			break;

			case 'd':
			if(optarg)
				__builtin_printf("Setting diagnostic mode %s\n", optarg);
			else
				__builtin_printf("Setting diagnostic mode\n");
			break;

			default:
			abort();
		}
	}
	/* non-option args */
	if(optind < argc) {
		__builtin_printf("non-option ARGV-elements: ");
		while(optind < argc)
			__builtin_printf("%s ", argv[optind++]);
		putchar('\n');
	}

	setup_node(argc, argv);
	init_node(); /* your local node */
	init_tug_io();
	node(); /* is a state machine */
	close_node();
	shutdown_node();
}
