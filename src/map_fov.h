// vim: cin:sts=4:sw=4 
#ifndef MAP_FOV_H
#define MAP_FOV_H
#include <stdint.h>
#include <unistd.h>
#include "mapdefs.h"

enum fov_angles {
	FA_NORMAL, //90 degrees
	FA_WIDE,   //180 degrees
	FA_FULL,   //360 degrees
};

typedef int (*los_cb)(struct t_map* map, uint8_t x, uint8_t y, void* param);
int los_default_cb(struct t_map* map, uint8_t x, uint8_t y, void* param);
int lineofsight(struct t_map* map, uint8_t sx, uint8_t sy, uint8_t tx, uint8_t ty, los_cb cb, void* cbparam);

void find_visible_entities(struct t_map* map, uint8_t* va, struct t_map_entity** o_entities, size_t sz);
void do_fov(struct t_map* map, struct t_map_entity* e, int radius, enum fov_angles angle, uint8_t* mem_array, int* visible_entities);
bool can_see(struct t_map* map, struct t_map_entity* e, uint8_t x, uint8_t y);
#endif
	
