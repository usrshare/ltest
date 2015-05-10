#ifndef MAPMODE_H
#define MAPMODE_H

#include <stdint.h>

#define MAX_ENTITIES 128
#define MAP_WIDTH 80
#define MAP_HEIGHT 20

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
	TT_DOOR,
	TT_BARS,
	TT_TABLE,
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
};

enum entitytypes {
	ET_NONE = 0,
	ET_PLAYER,
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
	uint16_t hp;
};

struct t_map;
struct t_map_entity;

typedef uint16_t (*turnFunc)(struct t_map* map, struct t_map_entity* me);
typedef uint16_t (*useFunc)(struct t_map* map, struct t_map_entity* me, struct t_map_entity* user);


struct t_map_entity {
	enum entitytypes type;
	uint16_t x;
	uint16_t y;
	uint16_t twait; ///< turns left to wait
	turnFunc turn;
	useFunc use;
};

struct t_map {
	struct t_square sq[MAP_WIDTH*MAP_HEIGHT];
	
	uint8_t width;
	uint8_t height;

	struct t_map_entity ent[MAX_ENTITIES];
};

int draw_map(struct t_map* map);

#endif
