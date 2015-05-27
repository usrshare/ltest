// vim: cin:sts=4:sw=4 
#ifndef MAPDEFS_H
#define MAPDEFS_H

#include <stdbool.h>
#include <stdint.h>
#include "entity.h"
#include "globals.h"

#define MAX_ENTITIES 128
#define MAX_AI_ENTITIES 64
#define MAP_WIDTH 80
#define MAP_HEIGHT 20

struct coords {
	uint8_t x;
	uint8_t y;
};

enum maptypes {

	MT_EXTERIOR,
	MT_INTERIOR,
	MT_ELEMENT_COUNT

};

enum mapflags {

	MF_SYMMETRIC = 1,
	MF_SECURE = 2,
	MF_PARANOID = 4,	
};

enum terraintypes {
	TT_OUTSIDE,
	TT_GRASS,
	TT_CORRIDOR,
	TT_SPACE,
	TT_RESTRICTED_SPACE,
	TT_SPECIAL_SPACE,
	TT_WALL,
	TT_WINDOW,
	TT_DOOR_CLOSED,
	TT_DOOR_OPEN,
	TT_BARS,
	TT_TABLE,
	TT_LOCKER,
	TT_CUSTOM,
	TT_STAIRS,
	TT_UNKNOWN,
	TT_ELEMENT_COUNT
};

enum terrainflags {

	TF_SOLID = 1,
	TF_BLOCKS_VISION = 2,
	TF_BLOCKS_SOUND = 4,
	TF_BLOCKS_ATTACKS = 8,
	TF_DODGE = 16,
	TF_NOSPAWN = 32,
	TF_OUTSIDE = 64,
	TF_CONTAINS_LOOT = 128,
	TF_OKAY_DOOR = 256,
};

extern enum terrainflags tflags[TT_ELEMENT_COUNT];

enum extraflags {
	XF_MAIN_ENTRANCE_POINT = 1,
	XF_BACK_ENTRANCE_POINT = 2,
	XF_ENEMY_SPAWN_POINT = 4,
	XF_SAFE_AREA = 8,
	XF_PATROL_WAYPOINT = 16,
};

enum entitytypes {
	ET_NONE = 0,
	ET_PLAYER,
	ET_CPU,
	ET_STATIC,
	ET_COUNT
};

enum entityflags {
	EF_ALLY = 1,
	EF_ALWAYSVISIBLE = 2,
};

enum spawnpos {

	SF_DEFAULT = 0,
	SF_RANDOM,
	SF_RANDOM_INSIDE,
	SF_RANDOM_RESTRICTED,
	SF_RANDOM_SPECIAL,
};

struct t_square {
	enum terraintypes type;
	enum extraflags flags;
	uint16_t hp;
};

enum movedirections {
	MD_NORTH = 0,
	MD_NORTHEAST = 1,
	MD_EAST = 2,
	MD_SOUTHEAST = 3,
	MD_SOUTH = 4,
	MD_SOUTHWEST = 5,
	MD_WEST = 6,
	MD_NORTHWEST = 7,
	MD_COUNT = 8,
};

extern int movediff[MD_COUNT][2];

#define MOVE(x) (movediff[MD_##x][0]),(movediff[MD_##x][1])

struct t_map;
struct t_map_entity;

//typedef uint16_t (*hearFunc)(struct t_map* map, struct t_map_entity* me, uint8_t y, uint8_t x);
//typedef uint16_t (*seeFunc)(struct t_map* map, struct t_map_entity* me, uint8_t y, uint8_t x, struct t_map_entity* whom);
typedef uint16_t (*turnFunc)(struct t_map* map, struct t_map_entity* me);
typedef uint16_t (*actFunc)(struct t_map* map, struct t_map_entity* me);
//typedef uint16_t (*useFunc)(struct t_map* map, struct t_map_entity* me, struct t_map_entity* user);

struct t_map_entity {
	enum entitytypes type;
	enum entityflags flags;
	struct t_creature* ent;
	uint8_t e_id;
	uint16_t x;
	uint16_t y;
	uint16_t twait; ///< turns left to wait
	struct t_map_ai_data* aidata;
	turnFunc turn; //this function is called every turn
	actFunc act; //this function is only called when twait is 0.
};

enum t_alertlevel {

	AL_NORMAL, //nobody is alerted
	AL_SUSPICIOUS, //enemies suspicious
	AL_ALERTED, //alarm raised
	AL_BACKUP, //backup called
	AL_COUNT, 
};

struct t_map {

	char map_title[40];

	struct t_square sq[MAP_WIDTH*MAP_HEIGHT];
	
	uint8_t width;
	uint8_t height;

	enum t_alertlevel alert_state;
	uint16_t alert_time;

	struct coords spawn_points[SQUAD_MAX];

	struct t_map_entity ent[MAX_ENTITIES];
	struct t_creature creat[MAX_ENTITIES]; //this stores a list of temporary entities for this map.
						//these entities are either forgotten forever, or stored
						//into a more permanent list (which doesn't exist yet)
};

#endif
