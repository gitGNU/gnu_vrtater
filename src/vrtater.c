/* vrtater.c: Peer to peers 'virtual' 'reality' software.  In pre-alpha.
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
#include "global.h"

#if defined(VRT_X_SUPPORT) && defined(VRT_RENDERER_GL)
#include "ifnodeglx.h"
#endif /* VRT_X_SUPPORT && VRT_RENDERER_GL */

extern unsigned int vrt_hmaps_max;
static int verbose_flag;

void version(void);
void usage(void);

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
		"Usage: vrtater [OPTIONS]...\n\n"
		"  --brief             Run with brief console messages\n"
		"  -d, --diag[=LEVEL]  Apply tug diagnostic mode with optional LEVEL\n"
		"  -f, --find=URL      Start with a remote node search for ADDR\n"
		"  -?, --help          Print this usage message\n"
		"  --informal          If possible, abstract output for readability\n"
		"  -o, --orgin=DIR     Start node-orgin defined in directory DIR\n"
		"  -p, --partial=DIR   Start node-partial defined in directory DIR\n"
		"  --verbose           Output console messages in verbose format\n"
		"  --version           Output disclaimers and versions\n\n"
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
	int option;
	while(1) {
		static struct option long_options[] = {
			{"brief", no_argument, &verbose_flag, 0},
			{"diag", optional_argument, 0, 'd'},
			{"find", required_argument, 0, 'f'},
			{"help", no_argument, 0, '?'},
			{"informal", no_argument, &verbose_flag, 1},
			{"orgin", required_argument, 0, 'o'},
			{"partial", required_argument, 0, 'p'},
			{"verbose", no_argument, &verbose_flag, 2},
			{"version", no_argument, 0, 0},
			{0, 0, 0, 0}
		};
		int option_index = 0;
		option = getopt_long(argc, argv, "d::f:?o:p:", long_options,
			 &option_index);
		if(option == -1)
			break;
		switch(option) {

			case 0:
			if(long_options[option_index].flag != 0) {
				if (verbose_flag) {
					if (--verbose_flag) {
						__builtin_printf("setting --%s\n",
							long_options[option_index].name);
							break;
					} else {
						__builtin_printf("setting --%s\n",
							long_options[option_index].name);
							break;
					}
				} else {
					__builtin_printf("setting --%s\n",
						long_options[option_index].name);
					break;
				}
			}
			version();
			exit(0);

			case 'd':
			if (optarg)
				__builtin_printf("Setting diagnostic mode %s\n",
					optarg);
			else
				__builtin_printf("Setting diagnostic mode\n");
			break;

			case 'f':
			if (optarg)
				__builtin_printf("Searching for node %s\n",
					optarg);
			else {
				__builtin_fprintf(stderr, "vrtater: Error: "
					"syntax -f, --find, requires "
					" a node search string\n");
				abort();
			}
			break;

			case 'o':
			if (optarg)
				__builtin_printf("Setting node-orgin to %s "
					"configuration\n", optarg);
			else
				__builtin_printf("Setting node-orgin to "
					"default configuration\n");
			break;

			case 'p':
			if (optarg)
				__builtin_printf("Adding called node-partial "
					"%s to node-orgin\n", optarg);
			else {
				__builtin_fprintf(stderr, "vrtater: Error: "
					"Syntax -p, --partial, requires a "
					"called node-partial specifier\n");
				abort();
			}
			break;

			case '?':
			usage();
			exit(0);

			default:
			abort();
		}
		exit(0);
	}

	/* Read defaults. */
	vrt_hmaps_max = 40; /* for now */

	/* vrtater */
	node(argc, argv);
}
