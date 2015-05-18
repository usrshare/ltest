#ifndef MAPMODE_H
#define MAPMODE_H

#include <stdbool.h>
#include <stdint.h>

#include "mapdefs.h"
#include "map_ai.h"

int draw_map(struct t_map* map, struct t_map_entity* persp, bool show_fov, bool show_targets, bool show_heatmaps, bool hl_persp);

int space_taken(struct t_map* map, uint8_t x, uint8_t y);
struct t_map_entity* find_entity(struct t_map* map, uint8_t x, uint8_t y);

int mapgetch();
int nc_beep(void);

int vtile(uint8_t x, uint8_t y);

enum movedirections askdir();
int mapmode();
#endif
