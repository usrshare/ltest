#include "cpairs.h"
#include <curses.h>

int init_pairs() {

    	int c = start_color();	

	use_default_colors();
	
	if (c == ERR) exit(1);
	
	init_pair(1,COLOR_GREEN,-1);
	init_pair(2,COLOR_CYAN,-1);
	init_pair(3,COLOR_BLUE,-1);
	init_pair(4,COLOR_YELLOW,-1);
	init_pair(5,COLOR_MAGENTA,-1);
	init_pair(6,COLOR_RED,-1);

	return 0;
}
