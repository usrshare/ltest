#ifndef MAP_AI_H
#define MAP_AI_H

#include "mapdefs.h"

struct t_map_ai_data {

	struct t_map_entity* usedby;
	enum movedirections viewdir;
	uint8_t viewwidth;

	uint8_t viewarr [ MAP_WIDTH * MAP_HEIGHT ];

};

extern bool needs_ai [ET_COUNT];
	
extern struct t_map_ai_data aient[MAX_AI_ENTITIES];

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me);

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me);

int look(struct t_map* map, uint8_t y, uint8_t x, uint8_t angle, uint8_t* vis_array);


#endif
