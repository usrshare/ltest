#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#include "mapmode.h"

int main(int argc, char** argv) {

	srand(time(NULL));
	initscr();	
	cbreak();
	noecho();

	refresh();

	mapmode();

	endwin();

	return 0;

}
