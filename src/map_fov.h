#ifndef MAP_FOV_H
#define MAP_FOV_H
#include <stdint.h>
#include "mapdefs.h"

void do_fov(struct t_map* map, struct t_map_entity* e, uint8_t* mem_array);
#endif
