#include <curses.h>

#include "cpairs.h"
#include "mapmode.h"
#include "mapgen.h"
#include "map_fov.h"

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
	TF_SOLID | TF_DODGE | TF_NOSPAWN | TF_CONTAINS_LOOT, //table,
	TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_SOUND | TF_NOSPAWN | TF_CONTAINS_LOOT, //locker
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
		case TT_LOCKER: return '\\';
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

int draw_map(struct t_map* map, struct t_map_entity* persp, bool show_fov, bool show_targets, bool show_heatmaps) {

	int x,y;
	getparyx(statwindow,y,x);

	wmove(mapwindow,0,0);

	for (int iy=0; iy< MAP_HEIGHT; iy++) {
		for (int ix=0; ix < MAP_WIDTH; ix++) {

			int tilech = mapchar(&map->sq[iy*(MAP_WIDTH)+ix]); 
			
			int tilevis = 1;

			if ((persp != NULL) && (persp->aidata)) tilevis = persp->aidata->viewarr[iy * MAP_WIDTH + ix];

			chtype tileflags = 0;

			if (show_fov) {
			chtype fovcolor = 0;
			if ((tflags[map->sq[iy*(MAP_WIDTH)+ix].type] & TF_BLOCKS_VISION) == 0) {
			
			for (int i=0; i < MAX_ENTITIES; i++) {

				if ((map->ent[i].type != ET_PLAYER) && (map->ent[i].aidata) && (map->ent[i].aidata->viewarr[iy * MAP_WIDTH + ix] >= 3) ) {
					
					switch (map->ent[i].aidata->task) {
						case AIT_WORKING: fovcolor = CP_GREEN; break;
						case AIT_PATROLLING: fovcolor = CP_CYAN; break;
						case AIT_CHECKING_OUT: fovcolor = CP_YELLOW; break;
						case AIT_PURSUING: fovcolor = CP_RED; break;
						case AIT_LOOKING_FOR: fovcolor = CP_PURPLE; break;
					}
				}
			} }
			tileflags |= fovcolor;
			}

			if (tilevis == 1) tileflags |= CP_BLUE; else if (tilevis >= 3) tileflags |= A_BOLD;

			if (tilevis) mvwaddch(mapwindow,iy,ix, tileflags | tilech );

		}
	}

	for (int i=0; i < MAX_ENTITIES; i++) {

		if (map->ent[i].type == ET_NONE) continue;

		if ((map->ent[i].aidata) != NULL) {
		
		if (show_targets) {
			if (vtile(map->ent[i].aidata->dx,map->ent[i].aidata->dy))
				mvwaddch(mapwindow,map->ent[i].aidata->dy,map->ent[i].aidata->dx,'%' | CP_PURPLE);
		}
		
		if (show_heatmaps) {
			for (int m=0; m < HEATMAP_SIZE; m++) {
				uint16_t yx = map->ent[i].aidata->heatmap_old[m];
				if (yx == 65535) continue;
				uint8_t y = yx / MAP_WIDTH; uint8_t x = yx % MAP_WIDTH;
				mvwaddch(mapwindow,y,x,'&' | CP_YELLOW);
			}
			for (int m=0; m < HEATMAP_SIZE; m++) {
				uint16_t yx = map->ent[i].aidata->heatmap_new[m];
				if (yx == 65535) continue;
				uint8_t y = yx / MAP_WIDTH; uint8_t x = yx % MAP_WIDTH;
				mvwaddch(mapwindow,y,x,'&' | CP_GREEN);
			}
		}

		}
	}
	for (int i=0; i < MAX_ENTITIES; i++) {

		if (map->ent[i].type == ET_NONE) continue;
		int ex = map->ent[i].x; int ey = map->ent[i].y;
		
		if ( (persp == NULL) || ( (persp->aidata) && (persp->aidata->viewarr[ey * (MAP_WIDTH) + ex] >= 2) ) ) {
			mvwaddch(mapwindow,ey,ex,entchar(&map->ent[i]));
		}
	}

	wrefresh(mapwindow);

	wmove(statwindow,y,x);
	return 0;
}

int make_turn(struct t_map* map) {
	for (int i=0; i < MAX_ENTITIES; i++) {
		struct t_map_entity* e = &map->ent[i];
		if (e->type != ET_NONE) {

			if (e->twait > 0) e->twait--;
			if ((e->twait == 0) && (e->turn)) e->twait += e->turn(map,e);

		}
	}
	return 0;
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
		newai->dx = 255;
		newai->dy = 255;
		heatmap_clear(newai->heatmap_old);
		heatmap_clear(newai->heatmap_new);
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

int kill_entity(struct t_map_entity* ent) {

	//this removes the entity and all references to it.

	ent->type = ET_NONE;

	struct t_map_ai_data* ai = ent->aidata;
	ent->aidata = NULL;
	ent->turn = ent->use = ent->sound_cb = ent->vision_cb = NULL;
	memset(ai,0,sizeof(struct t_map_ai_data));
	return 0;
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

	#define ENEMIES_COUNT 16

	struct t_map_entity* enemies[ENEMIES_COUNT];

	for (int i=0; i < ENEMIES_COUNT; i++) {
		enemies[i] = spawn_entity(&map1,ET_CPU,SF_RANDOM_INSIDE,enemy_turnFunc,NULL,NULL,NULL);

		if (enemies[i]) {enemies[i]->aidata->task = AIT_PATROLLING;}
	}

	player_ent->aidata->wideview = 1;

	memset(player_ent->aidata->viewarr,1,sizeof(uint8_t) * MAP_WIDTH * MAP_HEIGHT);

	statwindow_b = newwin(LINES-20,COLS,0,0);
	wmove(statwindow_b,LINES-21,0);
	whline(statwindow_b,ACS_HLINE,COLS);
	wrefresh(statwindow_b);

	statwindow = subwin(statwindow_b,LINES-21,COLS,0,0);
	if (statwindow == 0) return 1;
	scrollok(statwindow,1);

	keypad(statwindow,1);
	
	do_fov(&map1,player_ent,25,FA_FULL,player_ent->aidata->viewarr,NULL);

	int loop = 1;
	int turn_n = 0;
	do {
		wrefresh(statwindow);
		draw_map(&map1, player_ent,1,1,1);
		make_turn(&map1);
		loop = check_conditions(&map1);
		turn_n++;
	} while (loop);

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
