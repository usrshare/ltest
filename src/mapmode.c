#include <curses.h>

#include "mapmode.h"
#include "mapgen.h"

#include "random.h"

#include <string.h> //memset

WINDOW* statwindow_b;
WINDOW* statwindow;
WINDOW* mapwindow;

enum terrainflags tflags[TT_ELEMENT_COUNT] = {
	TF_OUTSIDE, //outside
	TF_OUTSIDE, //grass
	0, //corridor
	0, //space
	0, //restricted space
	TF_NOSPAWN, //special space
	TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_SOUND | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //wall
	TF_SOLID | TF_BLOCKS_SOUND | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //window
	TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //door
	TF_SOLID | TF_NOSPAWN, //bars
	TF_SOLID | TF_DODGE | TF_NOSPAWN, //table,
	TF_NOSPAWN, //custom
	TF_NOSPAWN, //stairs
};

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

	for (int iy=0; iy< MAP_HEIGHT; iy++) {
		for (int ix=0; ix < MAP_WIDTH; ix++) {
			mvwaddch(mapwindow,iy,ix,mapchar(&map->sq[iy*(MAP_WIDTH)+ix]));
		}
	}

	for (int i=0; i < MAX_ENTITIES; i++) {

		if (map->ent[i].type == ET_NONE) continue;
		int x = map->ent[i].x; int y = map->ent[i].y;

		mvwaddch(mapwindow,y,x,entchar(&map->ent[i]));


	}

	wrefresh(mapwindow);
}

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me) {

	int pch = getch();

	return 1;
}

int make_turn(struct t_map* map) {
	for (int i=0; i < MAX_ENTITIES; i++) {
		struct t_map_entity* e = &map->ent[i];
		if (e->type != ET_NONE) {

			if (e->twait > 0) e->twait--;
			if ((e->twait == 0) && (e->turn)) e->twait += e->turn(map,e);

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

int spawn_entity(struct t_map* map, enum entitytypes type, enum spawnpos position, turnFunc tf, useFunc uf ) {

	struct t_map_entity* newent = next_empty_entity(map);
	if (newent == NULL) return 1;

	newent->type = type;

	int x,y;

	switch(position) {

		case SF_DEFAULT:
			break;

		case SF_RANDOM:

			do {
				x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
			} while ((tflags[map->sq[y*MAP_WIDTH+x].type] & TF_NOSPAWN) != 0);
			break;

		case SF_RANDOM_INSIDE: {

					       int tf = 0;
					       do {
						       x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
						       tf = tflags[map->sq[y*MAP_WIDTH+x].type];
					       } while ( ((tf & TF_OUTSIDE) == 0) | ((tf & TF_NOSPAWN) != 0) );
					       break; }
		case SF_RANDOM_RESTRICTED:

				       do {
					       x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
				       } while (map->sq[y*MAP_WIDTH+x].type != TT_RESTRICTED_SPACE);
				       break;
		case SF_RANDOM_SPECIAL:

				       do {
					       x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
				       } while (map->sq[y*MAP_WIDTH+x].type != TT_SPECIAL_SPACE);
				       break;


	}

	newent->x = x;
	newent->y = y;
	newent->turn = tf;
	newent->use = uf;

}

int mapmode() {

	struct t_map map1;

	memset(&(map1.sq), 0, sizeof(struct t_square) * MAP_WIDTH * MAP_HEIGHT); 
	memset(&(map1.ent), 0, sizeof(struct t_map_entity) * MAX_ENTITIES); 

	generate_buildings(&map1,GM_LIBERAL);
	
	spawn_entity(&map1,ET_PLAYER,SF_RANDOM,player_turnFunc,NULL);

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
