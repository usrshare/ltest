#include <curses.h>

#include "cpairs.h"
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
	TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //closed door
	TF_NOSPAWN, //open door
	TF_SOLID | TF_NOSPAWN, //bars
	TF_SOLID | TF_DODGE | TF_NOSPAWN, //table,
	TF_NOSPAWN, //custom
	TF_NOSPAWN, //stairs
	TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_SOUND | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //unknown
};

int movediff[MD_COUNT][2] = {
	{0,-1},
	{1,-1},
	{1,0},
	{1,1},
	{0,1},
	{-1,1},
	{-1,0},
	{-1,-1}};

int vtile(uint8_t x, uint8_t y) {

	return ((x < MAP_WIDTH) && (y < MAP_HEIGHT));
}

chtype mapchar(struct t_square* sq) {

	switch (sq->type) {
		case TT_OUTSIDE: return ',';
		case TT_SPACE: return '.';
		case TT_RESTRICTED_SPACE: return '_';
		case TT_SPECIAL_SPACE: return '.';
		case TT_WALL: return ACS_CKBOARD;
		case TT_WINDOW: return '~';
		case TT_DOOR_CLOSED: return '+';
		case TT_DOOR_OPEN: return '-';
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

struct t_map_ai_data* next_empty_ai_data(void) {
	for (int i=0; i < MAX_AI_ENTITIES; i++)
		if (aient[i].usedby == NULL) return &(aient[i]);

	return NULL;
}

chtype entchar(struct t_map_entity* e) {

	switch (e->type) {
		case ET_NONE: return 0;
		case ET_PLAYER: return ('@' | A_REVERSE);
		case ET_CPU: return '@';
		case ET_STATIC: return '!';
		default: return 'X';
	}
}

int draw_map(struct t_map* map, struct t_map_entity* persp) {

	int x,y;
	getparyx(statwindow,y,x);

	wmove(mapwindow,0,0);

	for (int iy=0; iy< MAP_HEIGHT; iy++) {
		for (int ix=0; ix < MAP_WIDTH; ix++) {

			int tilech = mapchar(&map->sq[iy*(MAP_WIDTH)+ix]); 
			
			int tilevis = 1;

			if ((persp != NULL) && (persp->aidata)) tilevis = persp->aidata->viewarr[iy * MAP_WIDTH + ix];

			chtype fovcolor = 0;

			int tilecolor = 0;

			if (tilevis == 1) tilecolor = CP_BLUE; else if (tilevis >= 3) tilecolor = A_BOLD;

			int enemyfov = 0;

			if ((tflags[map->sq[iy*(MAP_WIDTH)+ix].type] & TF_BLOCKS_VISION) == 0) {
			
			for (int i=0; i < MAX_ENTITIES; i++) {

				if ((map->ent[i].type != ET_PLAYER) && (map->ent[i].aidata) && (map->ent[i].aidata->viewarr[iy * MAP_WIDTH + ix] >= 3) ) {
					
					enemyfov=1;

					switch (map->ent[i].aidata->task) {

						case AIT_WORKING: fovcolor = CP_GREEN; break;
						case AIT_PATROLLING: fovcolor = CP_CYAN; break;
						case AIT_CHECKING_OUT: fovcolor = CP_YELLOW; break;
						case AIT_PURSUING: fovcolor = CP_RED; break;
						 
					}

				}

			} }

			if (tilevis) mvwaddch(mapwindow,iy,ix, tilecolor | fovcolor | tilech );

		}
	}

	for (int i=0; i < MAX_ENTITIES; i++) {

		if (map->ent[i].type == ET_NONE) continue;
		int x = map->ent[i].x; int y = map->ent[i].y;

		if ( (persp == NULL) || ( (persp->aidata) && (persp->aidata->viewarr[y * (MAP_WIDTH) + x] >= 2) ) ) {

			mvwaddch(mapwindow,y,x,entchar(&map->ent[i]));
		}

	}

	wrefresh(mapwindow);

	wmove(statwindow,y,x);
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

struct t_map_entity* find_entity(struct t_map* map, uint8_t x, uint8_t y) {

	for (int i=0; i < MAX_ENTITIES; i++) {
		struct t_map_entity* e = &map->ent[i];
		if ((e->x == x) && (e->y == y)) return e;
	}
	return NULL;
}

int space_taken(struct t_map* map, uint8_t x, uint8_t y) {

	for (int i=0; i < MAX_ENTITIES; i++) {
		struct t_map_entity* e = &map->ent[i];
		if ((e->type != ET_NONE) && (e->x == x) && (e->y == y)) return 1;
	}

	return 0;
}

struct t_map_entity* spawn_entity(struct t_map* map, enum entitytypes type, enum spawnpos position, turnFunc tf, useFunc uf, seeFunc sf, hearFunc hf) {

	struct t_map_entity* newent = next_empty_entity(map);
	if (newent == NULL) return NULL;

	if (needs_ai[type]) {

		struct t_map_ai_data* newai = next_empty_ai_data();
		if (newai == NULL) return NULL;

		newent->aidata = newai;
		newai->usedby = newent;

	}

	newent->type = type;

	int x,y;

	switch(position) {

		case SF_DEFAULT:
			break;

		case SF_RANDOM:

			do {
				x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
			} while ( ( tflags[map->sq[y*MAP_WIDTH+x].type] & TF_NOSPAWN ) || space_taken(map,x,y) );
			break;

		case SF_RANDOM_INSIDE: {

					       int tf = 0;
					       do {
						       x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
						       tf = tflags[map->sq[y*MAP_WIDTH+x].type];
					       } while ( (tf & TF_OUTSIDE) || (tf & TF_NOSPAWN) || space_taken(map,x,y) );
					       break; }
		case SF_RANDOM_RESTRICTED:

				       do {
					       x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
				       } while ( (map->sq[y*MAP_WIDTH+x].type != TT_RESTRICTED_SPACE) || space_taken(map,x,y) );
				       break;
		case SF_RANDOM_SPECIAL:

				       do {
					       x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
				       } while ( (map->sq[y*MAP_WIDTH+x].type != TT_SPECIAL_SPACE) || space_taken(map,x,y) );
				       break;


	}

	newent->x = x;
	newent->y = y;
	newent->turn = tf;
	newent->use = uf;
	newent->sound_cb = hf;
	newent->vision_cb = sf;
	return newent;
}

int mapmode() {

	struct t_map map1;

	memset(&(map1.sq), 0, sizeof(struct t_square) * MAP_WIDTH * MAP_HEIGHT); 
	memset(&(map1.ent), 0, sizeof(struct t_map_entity) * MAX_ENTITIES); 
	memset(aient, 0, sizeof(struct t_map_ai_data) * MAX_AI_ENTITIES); 

	int cs = curs_set(0);	

	mapwindow = newwin(20,COLS,LINES-20,0);

	generate_buildings(&map1,GM_SINGLE);

	struct t_map_entity* player_ent = spawn_entity(&map1,ET_PLAYER,SF_RANDOM,player_turnFunc,NULL,NULL,NULL);

	struct t_map_entity* enemies[8];

	for (int i=0; i < 8; i++) {
		enemies[i] = spawn_entity(&map1,ET_CPU,SF_RANDOM_INSIDE,enemy_turnFunc,NULL,NULL,NULL);

		if (enemies[i]) {enemies[i]->aidata->task = AIT_PATROLLING;}
	}

	player_ent->aidata->wideview = 1;

	statwindow_b = newwin(LINES-20,COLS,0,0);
	wmove(statwindow_b,LINES-21,0);
	whline(statwindow_b,ACS_HLINE,COLS);
	wrefresh(statwindow_b);

	statwindow = subwin(statwindow_b,LINES-21,COLS,0,0);
	if (statwindow == 0) exit(2);
	scrollok(statwindow,1);

	keypad(statwindow,1);

	draw_map(&map1, NULL);

	int loop = 1;

	int turn_n = 0;

	while (loop) {
		make_turn(&map1);
		turn_n++;
		loop = check_conditions(&map1);
		wrefresh(statwindow);
		draw_map(&map1, player_ent);
	}

	delwin(mapwindow);
	delwin(statwindow);
	delwin(statwindow_b);

	curs_set(cs);

	return 0;
}

int mapgetch() {

	int x,y;

	getparyx(statwindow,y,x);
	int m = wmove(statwindow_b,LINES-21,COLS-1);
	if (m == ERR) beep();

	int c = getch();

	wmove(statwindow,y,x);

	return c;
}

int nc_beep(void) {

	return beep();
}

enum movedirections askdir() {
	
	wprintw(statwindow,"Please specify a direction: [yuhjklbn]>");
	wrefresh(statwindow);
	
	enum movedirections r = MD_COUNT;
	
	int go_on = 1;

	while (go_on) {
	int c = wgetch(statwindow);
	wprintw(statwindow,"\n");
	switch(c) {
		case 'h':
		case 'H':
		r = MD_WEST; go_on = 0;break;
		case 'j':
		case 'J':
		r = MD_SOUTH; go_on = 0;break;
		case 'k':
		case 'K':
		r = MD_NORTH; go_on = 0;break;
		case 'l':
		case 'L':
		r = MD_EAST; go_on = 0;break;
		case 'y':
		case 'Y':
		r = MD_NORTHWEST; go_on = 0;break;
		case 'u':
		case 'U':
		r = MD_NORTHEAST; go_on = 0;break;
		case 'b':
		case 'B':
		r = MD_SOUTHWEST; go_on = 0;break;
		case 'n':
		case 'N':
		r = MD_SOUTHEAST; go_on = 0;break;
		default:
		beep();
		wprintw(statwindow,"Invalid direction. Please choose [yuhjklbn]>");
		wrefresh(statwindow);
	}

	}

	return r;
}
