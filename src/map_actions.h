// vim: cin:sts=4:sw=4 
#ifndef MAP_ACTIONS_H
#define MAP_ACTIONS_H
#include <stdint.h>
#include "mapdefs.h"

int open_door (struct t_map* map, struct t_map_entity* who, uint8_t x, uint8_t y);

#endif
