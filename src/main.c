#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {

	srand(time(NULL));
	initscr();	
	cbreak();
	noecho();

	mapmode();

	endwin();

	return 0;

}
