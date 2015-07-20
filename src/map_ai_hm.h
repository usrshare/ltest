// vim: cin:sts=4:sw=4 
#ifndef MAP_AI_HM_H
#define MAP_AI_HM_H
#include "mapdefs.h"

#define HEATMAP_SIZE 2048

int heatmap_clear(uint16_t* hm);

int update_heatmap(struct t_map* map, uint8_t x, uint8_t y, uint16_t* h_old, uint16_t* h_new);
int find_closest_on_heatmap(uint8_t x, uint8_t y, uint16_t* heatmap_old, uint16_t* heatmap_new, uint8_t* o_x, uint8_t* o_y);

#endif
