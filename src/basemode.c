// vim: cin:sts=4:sw=4 
#include "basemode.h"
#include "basemode_ui.h"
#include "mapmode.h"
#include <stdbool.h>
#include <time.h>

time_t ingametime;

int basemode(struct t_squad* squad) {

    bool loop = true;


    basemode_ui_init();
    draw_basemode_menu();
    do {

	update_basemode(squad);

	int ch = menugetch();
	switch(ch) {
	
	    case 'q':
		loop = false;
		break;
	    case 'm':
		mapmode(squad);
		break;
	}
	if (ch == 'q') loop = false;

    } while (loop);
    
    basemode_ui_free();
    return 0;
}

