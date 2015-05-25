#ifndef MAP_AI_H
#define MAP_AI_H

#include "mapdefs.h"

#define HEATMAP_SIZE 256

enum aitasks {

	AIT_WORKING, // for non-hostile NPCs doing "their regular jobs"
	AIT_PATROLLING, // for security/police NPCs
	AIT_PLEASE_LEAVE, // asking the target to leave the area
	AIT_CHECKING_OUT,
	AIT_PURSUING,   // when the AI sees the target directly
	AIT_LOOKING_FOR,// and whenn doesn't.
};

struct t_map_ai_data {

	struct t_map_entity* usedby;
	enum movedirections viewdir;
	bool wideview;
	bool show_fov;

	uint8_t viewarr [ MAP_WIDTH * MAP_HEIGHT ];

	// 0 = never seen this square.
	// 1 = seen it on a map, but not directly.
	// 2 = seen it before, but not right now.
	// 3 = seeing it right now.
	// 4 = seeing it right now, and there's an entity there.
	
	bool viewarr_updated;
	
	uint16_t patharr [ MAP_WIDTH * MAP_HEIGHT ];
	enum movedirections pathprev[ MAP_WIDTH * MAP_HEIGHT ];

	uint16_t heatmap_old [ HEATMAP_SIZE ]; //storing coords as (y * MAP_WIDTH + x).
	uint16_t heatmap_new [ HEATMAP_SIZE ]; //65535 stands for "empty".

	enum aitasks task;

	int path_plotted;

	uint8_t dx, dy;
	uint8_t lx, ly; //last x and y

	uint8_t alert_state;
	struct t_map_entity* target;

	uint16_t crimes;

	uint16_t timer;

	// array for pathfinding

};

extern bool needs_ai [ET_COUNT];
	
extern struct t_map_ai_data aient[MAX_AI_ENTITIES];

uint16_t enemy_seeFunc(struct t_map* map, struct t_map_entity* me, uint8_t y, uint8_t x, struct t_map_entity* whom);

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me);
uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me);

int heatmap_clear(uint16_t* hm);

#endif
