// vim: cin:sts=4:sw=4 
#include "cpairs.h"
#include <curses.h>

int init_pairs() {

    	int c = start_color();	

	assume_default_colors(7,-1);
	
	if (c == ERR) return 1;
	
	init_pair(1,COLOR_GREEN,-1);
	init_pair(2,COLOR_CYAN,-1);
	init_pair(3,COLOR_BLUE,-1);
	init_pair(4,COLOR_YELLOW,-1);
	init_pair(5,COLOR_MAGENTA,-1);
	init_pair(6,COLOR_RED,-1);
	init_pair(7,(COLORS >= 256) ? 231 : (7 | A_BOLD),-1);
	init_pair(8,(COLORS >= 256) ? 237 : (COLOR_BLUE),-1);
	init_pair(9,(COLORS >= 256) ? 244 : (COLOR_BLACK | A_BOLD),-1);
	init_pair(10,(COLORS >= 256) ? 250 : COLOR_WHITE,-1);

	return 0;
}
