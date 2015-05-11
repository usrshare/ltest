#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#include "cpairs.h"
#include "mapmode.h"

int main(int argc, char** argv) {

	srand(time(NULL));

	int opt = -1;

	while ((opt = getopt(argc, argv, "dv")) != -1) {
		switch (opt) {
			case 'd':
				getc(stdin);
				break;
			case 'v':
				printf("insert version info here\n");
				exit(0);
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-d] [-v]\n",
						argv[0]);
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
