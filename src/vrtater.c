/* vrtater.c: peer to peers 'virtual' 'reality' software.  in pre-alpha.
   Copyright (C) 2012, 2013 J. A. Green <green8@sdf-eu.org>

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

#ifdef VRT_X_SUPPORT
#include "ifnodeglx.h"
#endif /* VRT_X_SUPPORT */

void version(void);
void usage(void);

extern unsigned int vrt_hmaps_max;
static int verbose_flag;
static int readable_flag;

void
version(void)
{
	__builtin_printf("\n"
		"vrtater 0.3\n"
		"Copyright (C) 2012, 2013 J. A. Green\n"
		"License GPLv3+: GNU GPL version 3 or later "
		"<http://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change "
		"and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law.\n\n");
}

void
usage(void)
{
	__builtin_printf("\n"
		"usage: vrtater [-?h] [-d[level]] [-f on-node-name] "
		"[-i[in-node-name]]\n\n"
		"--brief                Run with beief console messages\n"
		"--diag         -d      Apply tug diagnostic mode /w opt level\n"
		"--find-node    -f      Start with a remote node search\n"
		"--help         -?      Print this usage message\n"
		"--in-node      -i      Start with default or given in-node\n"
		"--informal     -h      Print output in human readable form\n"
		"--verbose              Run with verbose console messages\n"
		"--version              Output disclaimers and versions\n\n"
		"Report bugs to: green8@sdf-eu.org\n"
		"pre-alpha development: "
		"<http://savannah.nongnu.org/projects/vrtater/>\n"
		"General help using GNU software: "
		"<http://www.gnu.org/gethelp/>\n\n");
}

int
main(int argc, char **argv)
{
	/* optional args */
	int c;
	while(1) {
		static struct option long_options[] = {
			{"verbose", no_argument, &verbose_flag, 1},
			{"brief", no_argument, &verbose_flag, 0},
			{"version", no_argument, 0, 0},
			{"find-node", required_argument, 0, 'f'},
			{"in-node", optional_argument, 0, 'i'},
			{"diag", optional_argument, 0, 'd'},
			{"informal", no_argument, 0, 'h'},
			{"help", no_argument, 0, '?'},
			{0, 0, 0, 0}
		};
		int option_index = 0;
		c = getopt_long(argc, argv, "f:i::d::h?", long_options,
			 &option_index);
		if(c == -1)
			break;
		switch(c) {

			case 0:
			if(long_options[option_index].flag != 0) {
				if(verbose_flag) {
					__builtin_printf("setting --%s\n",
						long_options[option_index].name);
					break;
				} else {
					__builtin_printf("setting --%s\n",
						long_options[option_index].name);
					break;
				}
			}
			version();
			exit(0);
			break;

			case 'f':
			if(optarg)
				__builtin_printf("Searching for node %s\n",
					optarg);
			else {
				__builtin_printf("foo\n");
				__builtin_fprintf(stderr, "vrtater: Error: "
					"syntax -f, --find-node, requires "
					" a node search string.\n");
				abort();
			}
			break;

			case 'i':
			if(optarg)
				__builtin_printf("Setting in-node to %s "
					"configuration\n", optarg);
			else
				__builtin_printf("Setting in-node to default "
					"configuration\n");
			break;

			case 'd':
			if(optarg)
				__builtin_printf("Setting diagnostic mode %s\n"
					, optarg);
			else
				__builtin_printf("Setting diagnostic mode\n");
			break;

			case 'h':
			readable_flag = 1;
			__builtin_printf("setting --%s mode(readable_flag)\n",
				long_options[option_index].name);
			break;

			case '?':
			usage();
			exit(0);

			default:
			abort();
		}
	}

	/* read defaults */
	vrt_hmaps_max = 20;

	/* vrtater */
	node(argc, argv); /* is a state machine */
}
