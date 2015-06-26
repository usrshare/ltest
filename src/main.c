// vim: cin:sts=4:sw=4 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>

#include "cpairs.h"
#include "mapmode.h"
#include "globals.h"

int init_memory() {

	memset(pool,0,sizeof(struct t_creature*) * POOLSIZE);
}

int main(int argc, char** argv) {

	srand(time(NULL));

	int opt = -1;

	dbgmode = 0;

	while ((opt = getopt(argc, argv, "dv")) != -1) {
		switch (opt) {
			case 'd':
				dbgmode = 1;
				printf("Press ENTER to start (feel free to attach a debugger to this process at this moment).\n");
				getc(stdin);
				break;
			case 'v':
				printf("insert version info here\n");
				exit(0);
				break;
			case '?':
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-d] [-v]\n"
						"\n"
						" -d : Debug mode.\n"
						" -v : Version info.\n"
						"\n"
						,argv[0]);
				exit(0);
		}
	}


	initscr();	
	cbreak();
	noecho();

	refresh();

	init_pairs();

	mapmode();

	endwin();

	return 0;

}
