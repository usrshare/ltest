#ifndef MAPMODE_H
#define MAPMODE_H

#include <stdbool.h>
#include <stdint.h>

#include "mapdefs.h"
#include "map_ai.h"

int draw_map(struct t_map* map, struct t_map_entity* persp);

int mapgetch();

int mapmode();
#endif
