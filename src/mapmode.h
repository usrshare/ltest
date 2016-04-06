// vim: cin:sts=4:sw=4 
#ifndef MAPMODE_H
#define MAPMODE_H

#include <stdbool.h>
#include <stdint.h>

#include "mapdefs.h"
#include "map_ai.h"

struct t_map {

	char map_title[40];

	struct t_square sq[MAP_WIDTH*MAP_HEIGHT];
	
	uint8_t width;
	uint8_t height;

	short type;

	uint32_t time;
	bool sitealarm;
	int16_t sitealarmtimer;
	int16_t postalarmtimer;
	int16_t sitealienate;

	struct t_map_ai_data aidata;

	struct coords spawn_points[SQUAD_MAX];

	struct t_map_entity ent[MAX_ENTITIES];
	struct t_creature creat[MAX_ENTITIES]; //this stores a list of temporary entities for this map.
						//these entities are either forgotten forever, or stored
						//into a more permanent list (which doesn't exist yet)
	struct t_item inventory[MAX_LOOTS * INVENTORY_SIZE];
};

extern struct t_map* curmap;

int space_taken(struct t_map* map, uint8_t x, uint8_t y);
struct t_map_entity* find_entity_ind(struct t_map* map, uint8_t x, uint8_t y, int ind);
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
bool can_attack(struct t_map* map, struct t_map_entity* a, struct t_map_entity* t, bool no_ranged, bool no_melee);

int mapmode();

char hasdisguise(struct t_creature* cr, bool restricted);
#endif
