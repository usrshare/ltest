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
	TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_SOUND | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //unknown
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

int trymove(struct t_map* map, struct t_map_entity* whom, int8_t dx, int8_t dy) {

	if ((dx < -1) || (dx > 1)) return 1;
	if ((dy < -1) || (dy > 1)) return 1; //this is only for simple movements.

	int time = (abs(dx) + abs(dy)) == 2 ? 6 : 4;
	
	int rx = whom->x + dx;
	int ry = whom->y + dy;
	
	if (tflags[map->sq[ry*MAP_WIDTH+rx].type] & TF_SOLID) return -1; //solid

	whom->x = rx;
	whom->y = ry;

	return time;
}

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me) {

	int m = wmove(statwindow_b,LINES-21,COLS-1);
	if (m == ERR) beep();

	int pch = getch();
	int r = 0;

	switch (pch) {

		case 'h':
		r = trymove(map,me,-1,0); //left
		break;
		
		case 'j':
		r = trymove(map,me,0,1); //up
		break;
		
		case 'k':
		r = trymove(map,me,0,-1); //down
		break;
		
		case 'l':
		r = trymove(map,me,1,0); //right
		break;
		
		case 'y':
		r = trymove(map,me,-1,-1); //northwest
		break;
		
		case 'u':
		r = trymove(map,me,1,-1); //northeast
		break;
		
		case 'b':
		r = trymove(map,me,-1,1); //southwest
		break;
		
		case 'n':
		r = trymove(map,me,1,1); //southeast
		break;

		case 'w': //wait
		return 1;
	}

	if (r < 0) { beep(); return 0;} else return r;
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

	int cs = curs_set(0);	
	
	mapwindow = newwin(20,COLS,LINES-20,0);

	generate_buildings(&map1,GM_SINGLE);
	
	spawn_entity(&map1,ET_PLAYER,SF_RANDOM,player_turnFunc,NULL);

	statwindow_b = newwin(LINES-20,COLS,0,0);
	wmove(statwindow_b,LINES-21,0);
	whline(statwindow_b,ACS_HLINE,COLS);
	wrefresh(statwindow_b);

	statwindow = subwin(statwindow_b,LINES-21,COLS,0,0);
	if (statwindow == 0) exit(2);
	scrollok(statwindow,1);

	keypad(statwindow,1);

	draw_map(&map1);

	int loop = 1;

	int turn_n = 0;

	while (loop) {
		make_turn(&map1);
		turn_n++;
		loop = check_conditions(&map1);
		wrefresh(statwindow);
		draw_map(&map1);
	}

	delwin(mapwindow);
	delwin(statwindow);
	delwin(statwindow_b);

	curs_set(cs);

	return 0;
}
