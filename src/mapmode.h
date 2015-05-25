// vim: cin:sts=4:sw=4 
#ifndef MAPMODE_H
#define MAPMODE_H

#include <stdbool.h>
#include <stdint.h>

#include "mapdefs.h"
#include "map_ai.h"

int space_taken(struct t_map* map, uint8_t x, uint8_t y);
struct t_map_entity* find_entity(struct t_map* map, uint8_t x, uint8_t y);
int vtile(uint8_t x, uint8_t y);

int kill_entity(struct t_map_entity* ent);

int mapmode();
#endif
