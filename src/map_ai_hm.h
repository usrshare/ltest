// vim: cin:sts=4:sw=4 
#ifndef MAP_AI_HM_H
#define MAP_AI_HM_H
#include "mapdefs.h"

#define HEATMAP_SIZE MAP_WIDTH * MAP_HEIGHT

int heatmap_reset(uint8_t* hm);
int heatmap_clear(uint8_t* hm);

int update_heatmap(struct t_map* map, uint8_t* hm, uint8_t x, uint8_t y);
int find_closest_on_heatmap(uint8_t x, uint8_t y, uint8_t* heatmap, uint8_t* o_x, uint8_t* o_y);

#endif
