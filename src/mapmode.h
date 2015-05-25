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

struct t_map_entity* spawn_entity(struct t_map* map, enum entitytypes type, bool gen_creature, struct t_entity_generate_rules* genrules, enum spawnpos position, turnFunc tf, actFunc af);
int kill_entity(struct t_map_entity* ent);

int mapmode();
#endif
