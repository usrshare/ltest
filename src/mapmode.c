// vim: cin:sts=4:sw=4 

#include "globals.h"

#include "mapmode.h"
#include "entity_types.h"
#include "mapgen.h"
#include "map_fov.h"

#include "random.h"
#include "map_ui.h"

#include <stdarg.h>
#include <string.h> //memset

enum terrainflags tflags[TT_ELEMENT_COUNT] = {
	TF_OUTSIDE, //outside
	TF_OUTSIDE, //grass
	TF_OKAY_DOOR, //corridor
	TF_OKAY_DOOR, //space
	TF_OKAY_DOOR, //restricted space
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

int vtile(uint8_t x, uint8_t y) { return ((x < MAP_WIDTH) && (y < MAP_HEIGHT)); }

struct t_map_entity* next_empty_entity(struct t_map* map) {
	for (int i=0; i < MAX_ENTITIES; i++)
		if (map->ent[i].type == ET_NONE) return &(map->ent[i]);

	return NULL;
}

struct t_creature* next_empty_temp_entity(struct t_map* map) {
	for (int i=0; i < MAX_ENTITIES; i++)
		if (map->creat[i].type == 0) return &(map->creat[i]);

	return NULL;
}

struct t_map_ai_data* next_empty_ai_data(void) {
	for (int i=0; i < MAX_AI_ENTITIES; i++)
		if (aient[i].usedby == NULL) return &(aient[i]);

	return NULL;
}


int make_turn(struct t_map* map) {
	for (int i=0; i < MAX_ENTITIES; i++) {
		struct t_map_entity* e = (map->ent)+i;
		if (e->type != ET_NONE) {

			if (e->twait > 0) e->twait--;
			if (e->turn) e->turn(map,e);
			if ((e->twait == 0) && (e->act)) e->twait += e->act(map,e);

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

struct t_map_entity* spawn_entity(struct t_map* map, enum entitytypes type, bool gen_creature, struct t_creature_generate_rules* genrules, enum spawnpos position, turnFunc tf, actFunc af) {

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

	if (gen_creature) {

	    struct t_creature* newcr = next_empty_temp_entity(map);
	    if (newcr == NULL) return NULL;

	    creature_init(newcr,genrules);
	    newent->ent = newcr;
	}

	switch(position) {

		case SF_DEFAULT: {

			int i=0;

			do {
				x = map->spawn_points[i].x; y = map->spawn_points[i].y;
				i++;
			} while ( space_taken(map,x,y) );
			break; }

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
	newent->act = af;
	return newent;
}

int kill_entity(struct t_map_entity* ent) {

	//this removes the entity and all references to it.

	ent->type = ET_NONE;

	struct t_map_ai_data* ai = ent->aidata;
	ent->aidata = NULL;
	ent->turn = ent->act = NULL;
	memset(ai,0,sizeof(struct t_map_ai_data));
	return 0;
}

int mapmode() {

	struct t_map map1;

	memset(&(map1.sq), 0, sizeof(struct t_square) * MAP_WIDTH * MAP_HEIGHT); 
	memset(&(map1.ent), 0, sizeof(struct t_map_entity) * MAX_ENTITIES); 
	memset(aient, 0, sizeof(struct t_map_ai_data) * MAX_AI_ENTITIES); 

	map1.alert_state = 0; map1.alert_time = 0;

	generate_buildings(&map1,GM_SINGLE);

	#define PLAYERS_COUNT 1

	struct t_map_entity* players[PLAYERS_COUNT];
	
	for (int i=0; i < PLAYERS_COUNT; i++) {
		players[i] = spawn_entity(&map1,ET_PLAYER,false,NULL,SF_DEFAULT,player_turnFunc,player_actFunc);
		if (players[i]) {
		players[i]->flags |= EF_ALWAYSVISIBLE;
		players[i]->e_id = i;
		players[i]->aidata->wideview = 1;
		memset(players[i]->aidata->viewarr,0,sizeof(uint8_t) * MAP_WIDTH * MAP_HEIGHT);
		}
	}
       
	#define ENEMIES_COUNT 16

	struct t_map_entity* enemies[ENEMIES_COUNT];

	for (int i=0; i < ENEMIES_COUNT; i++) {
		enemies[i] = spawn_entity(&map1,ET_CPU,true,&type_rules[ET_SECURITYGUARD],SF_RANDOM_INSIDE,enemy_turnFunc,enemy_actFunc);
		if (enemies[i]) {enemies[i]->aidata->task = AIT_PATROLLING;}
	}

	for (int i=0; i < PLAYERS_COUNT; i++) {
		memset(players[i]->aidata->viewarr,1,sizeof(uint8_t) * MAP_WIDTH * MAP_HEIGHT);
		do_fov(&map1,players[i],25,FA_FULL,players[i]->aidata->viewarr,NULL);
		draw_map(&map1,players[i],1,dbgmode ? 1 : 0, dbgmode ? 1 : 0,0);
	}
	
	map_ui_init(&map1);
	
	int loop = 1;
	int turn_n = 0;
	do {
		update_ui(&map1);
		make_turn(&map1);
		loop = check_conditions(&map1);
		turn_n++;
		if (map1.alert_time > 0) map1.alert_time--; if ((map1.alert_time == 0) && (map1.alert_state > 0)) map1.alert_state--;
		for (int i = 0; i < MAX_ENTITIES; i++) {
			if ((map1.ent[i].aidata) && (map1.ent[i].aidata->timer > 0)) map1.ent[i].aidata->timer--; }
	} while (loop);

	map_ui_free(&map1);
	return 0;
}
