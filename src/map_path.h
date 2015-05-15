#ifndef MAP_PATH_H
#define MAP_PATH_H

#include "map_ai.h"
enum movedirections plot_follow(uint8_t x, uint8_t y, enum movedirections* pathprev);
int plot_path(struct t_map* map, struct t_map_entity* who, uint8_t dx, uint8_t dy, uint16_t* patharr, enum movedirections* pathprev);

#endif
