// vim: cin:sts=4:sw=4 
#ifndef MAPMODE_H
#define MAPMODE_H

#include <stdbool.h>
#include <stdint.h>

#include "mapdefs.h"
#include "map_ai.h"

struct t_map* curmap;

int space_taken(struct t_map* map, uint8_t x, uint8_t y);
struct t_map_entity* find_entity(struct t_map* map, uint8_t x, uint8_t y);
int vtile(uint8_t x, uint8_t y);

struct t_item* next_empty_loot(struct t_map* map);

struct spawnflags {
    bool gen_creature; ///< whether or not to generate a creature
    struct t_creature_generate_rules* genrules; ///< rules for generating creatures
    turnFunc tf; ///< turn function
    actFunc af; ///< act function
    enum spawnpos position; ///< position
    uint8_t x; ///< x coord
    uint8_t y; ///< x coord
};

struct t_map_entity* spawn_loot(struct t_map* map, struct t_item* items, uint8_t y, uint8_t x);
struct t_map_entity* spawn_entity(struct t_map* map, enum entitytypes type, struct spawnflags sf);
int kill_entity(struct t_map_entity* ent);

int mapmode();

char hasdisguise(struct t_creature* cr, bool restricted);
#endif
