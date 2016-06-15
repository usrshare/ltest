// vim: cin:sts=4:sw=4 
#include "basemode_ui.h"
#include "weapon.h"
#include <curses.h>
#include <string.h>

WINDOW* statuswindow;
WINDOW* squadwindow;
WINDOW* menuwindow;

int menugetch() {

	int c = wgetch(menuwindow);
	return c;
}

int update_squad_stats(struct t_squad* squad) {

	wmove(squadwindow,0,0);
	for (int i=0; i < COLS; i++) waddch(squadwindow, ACS_HLINE);

	mvwprintw(squadwindow,0,1," %s ", squad->name);

	for (int i=0; i < SQUAD_MAXSIZE; i++) {
		if (!(squad->member[i])) continue;
		wmove(squadwindow,i+1,0);
		wclrtoeol(squadwindow);
		char entname[32];
		describe_entity(squad->member[i], entname, 32);

		wprintw(squadwindow,"%d: %-20.32s | ", i+1, entname);
		
		const char* e_weapon = w_type(squad->member[i]->weapon)->shortname;
		
		wprintw(squadwindow,"%10.10s | ", e_weapon);

		int statattr;
		char* e_stat = gethealthstat(squad->member[i], 1, &statattr);
		
		wattron(squadwindow,statattr);
		wprintw(squadwindow,"%10.10s",e_stat);
		wattroff(squadwindow,statattr);

	}
	return 0;
}

int draw_basemode_menu() {

	wclear(menuwindow);
	mvwprintw(menuwindow, 2, 2, "M - Launch map mode");
	mvwprintw(menuwindow, 3, 2, "Q - Exit");

	wrefresh(menuwindow);

	return 0;
}

int update_basemode(struct t_squad* squad) {

	update_squad_stats(squad);
	wrefresh(squadwindow);

	return 0;
}
int basemode_ui_init() {

	statuswindow = newwin(1, COLS, 0, 0);
	squadwindow = newwin(SQUAD_MAXSIZE + 1, COLS, 1, 0);
	menuwindow = newwin(LINES-SQUAD_MAXSIZE - 2, COLS, SQUAD_MAXSIZE + 2, 0);

	keypad(menuwindow,1);
	return 0;
}

int basemode_ui_free() {

	delwin(menuwindow);
	delwin(squadwindow);
	delwin(statuswindow);
	return 0;
}
