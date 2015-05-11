#ifndef MAP_AI_H
#define MAP_AI_H

#include "mapdefs.h"

struct t_map_ai_data {

	struct t_map_entity* usedby;
	enum movedirections viewdir;
	bool wideview;

	uint8_t viewarr [ MAP_WIDTH * MAP_HEIGHT ];

	// 0 = never seen this square.
	// 1 = seen it before, but not right now.
	// 2 = seeing it right now.
	// 3 = seeing it right now, and there's an entity there.

};

extern bool needs_ai [ET_COUNT];
	
extern struct t_map_ai_data aient[MAX_AI_ENTITIES];

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me);

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me);

#endif
