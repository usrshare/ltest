#include <curses.h>

#include "mapmode.h"

WINDOW* statwindow_b;
WINDOW* statwindow;
WINDOW* mapwindow;

int generate_map(struct t_map* map, enum maptypes mt, int flags) {

	
	
}

chtype mapchar(struct t_square* sq) {

	switch (sq->type) {
		case TT_OUTSIDE: return ',';
		case TT_SPACE: return '.';
		case TT_RESTRICTED_SPACE: return '_';
		case TT_SPECIAL_SPACE: return '.';
		case TT_WALL: return ACS_CKBOARD;
		case TT_WINDOW: return '~';
		case TT_DOOR: return '+';
		case TT_BARS: return '#';
		case TT_TABLE: return '-';
		case TT_CUSTOM: return '!';
		case TT_STAIRS: return '>';
		default: return 'X';
	}
}

struct t_map_entity* next_empty_entity(struct t_map* map) {
	for (int i=0; i < MAX_ENTITIES; i++)
		if (map->ent[i].type == ET_NONE) return &(map->ent[i]);
	
	return NULL;
}

chtype entchar(struct t_map_entity* e) {

	switch (e->type) {
		case ET_NONE: return 0;
		case ET_PLAYER: return ('@' | A_REVERSE);
		default: return 'X';
	}
}


int draw_map(struct t_map* map) {

	wmove(mapwindow,0,0);

	for (int iy=0; iy< map->height; iy++) {
		for (int ix=0; ix < map->width; ix++) {
			mvwaddch(mapwindow,iy,ix,mapchar(&map->sq[iy*(map->width)+ix]));
		}
	}

	wrefresh(mapwindow);
}

int make_turn(struct t_map* map) 
{
	for (int i=0; i < MAX_ENTITIES; i++) {
		struct t_map_entity* e = &map->ent[i];
		if (e->type != ET_NONE) {

		if (e->twait > 0) e->twait--;
		if ((e->twait == 0) && (e->turnFunc)) e->twait += e->turnFunc(map,e);

		}
	}
}

int check_conditions(struct t_map* map) {

	//checks if conditions are OK for the next move:
	// at least one player entity should be alive.

	int living_players = 0;

	for (int i=0; i < MAX_ENTITIES; i++) {
		struct t_map_entity* e = &map->ent[i];
		if (e->type == ET_PLAYER) living_players++;
		}

	if (living_players == 0) return 0;
	return 1;
}

int spawn_entity(struct t_map* map, enum entitytypes type, enum spawnpos flags, int todo) {

	struct t_map_entity* newent = next_empty_entity(map);
	if (newent == NULL) return 1;

	newent->type = type;




}

int mapmode() {

	struct t_map map1;

	statwindow_b = newwin(LINES-20,COLS,0,0);
	statwindow = derwin(statwindow_b,LINES-19,COLS,0,0);

	wmove(statwindow_b,LINES-19,0);
	for (int i=0; i < COLS; i++) waddch(statwindow_b,ACS_HLINE);

	mapwindow = newwin(20,COLS,LINES-20,0);

	int loop = 1;

	while (loop) {
	make_turn(&map1);
	loop = check_conditions(&map1);
	draw_map(&map1);
	}

	delwin(mapwindow);
	delwin(statwindow);
	delwin(statwindow_b);

	return 0;
}
