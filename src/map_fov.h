#ifndef MAP_FOV_H
#define MAP_FOV_H
#include <stdint.h>
#include <unistd.h>
#include "mapdefs.h"

void find_visible_entities(struct t_map* map, uint8_t* va, struct t_map_entity** o_entities, size_t sz);
void do_fov(struct t_map* map, struct t_map_entity* e, int radius, uint8_t* mem_array, int* visible_entities);
#endif
	
