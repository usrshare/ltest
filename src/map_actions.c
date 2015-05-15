#include "map_actions.h"
#include "mapdefs.h"
#include "mapmode.h"

#include <stdlib.h>

int open_door (struct t_map* map, struct t_map_entity* who, uint8_t x, uint8_t y) {

	if (!vtile(x,y)) return -1; // not a valid space

	if ((abs(who->x - x) > 1) || (abs(who->y - y) > 1)) return -3; //out of reach

	if (map->sq[y * MAP_WIDTH+x].type != TT_DOOR_CLOSED) return -2; //not a closed door
	map->sq[y * MAP_WIDTH+x].type = TT_DOOR_OPEN; return 8;	
}
