// vim: cin:sts=4:sw=4 
#include "map_ai_hm.h"

#include "mapmode.h"

#include <string.h>

// -- these functions use heatmaps to determine where the player may hide.
// the idea has been shamelessly and badly stolen from 
// http://www.roguebasin.com/index.php?title=Smart_searching_and_Modeling_the_player_with_a_%22heatmap%22

uint16_t* find_empty_heatmap(uint16_t* heatmap) {

	for (int i = 0; i < HEATMAP_SIZE; i++)
		if (heatmap[i] == 65535) return &(heatmap[i]);

	return NULL;
}
int heatmap_exists(uint16_t* hm1, uint16_t* hm2, uint8_t x, uint8_t y) {

	uint16_t yx = y * MAP_WIDTH + x;

	for (int i = 0; i < HEATMAP_SIZE; i++) {
		if (hm1[i] == yx) return 1;
	}

	if (hm2 != NULL) {
		for (int i = 0; i < HEATMAP_SIZE; i++) {
			if (hm2[i] == yx) return 2;
		}
	}
	return 0;
}
int heatmap_add(uint16_t* hm, uint8_t x, uint8_t y) {

	uint16_t yx = y * MAP_WIDTH + x;

	uint16_t* space = find_empty_heatmap(hm);

	if (space == NULL) return 1; else *space = yx;

	return 0;
}
int heatmap_clear(uint16_t* hm) {

	memset(hm,0xff,sizeof(uint16_t) * HEATMAP_SIZE);
	return 0;
}
int find_closest_on_heatmap(uint8_t x, uint8_t y, uint16_t* heatmap_old, uint16_t* heatmap_new, uint8_t* o_x, uint8_t* o_y) {

	int8_t dx = 0, dy = 0, rx = x, ry = y;

	uint8_t sm_x = 255, sm_y = 255; uint16_t sm_d = UINT16_MAX; //smallest distance, x and y

	for (int i = 0; i < HEATMAP_SIZE; i++) {

		if (heatmap_old[i] == 65535) continue;

		rx = (heatmap_old[i] % MAP_WIDTH); dx = rx - x;
		ry = (heatmap_old[i] / MAP_WIDTH); dy = ry - y;

		uint16_t d = (abs(dx) * abs(dx)) + (abs(dy) * abs(dy));

		if (d < sm_d) { sm_x = (heatmap_old[i] % MAP_WIDTH);sm_y = (heatmap_old[i] / MAP_WIDTH); sm_d = d;}
	}

	for (int i = 0; i < HEATMAP_SIZE; i++) {

		if (heatmap_new[i] == 65535) continue;

		rx = (heatmap_new[i] % MAP_WIDTH); dx = rx - x;
		ry = (heatmap_new[i] / MAP_WIDTH); dy = ry - y;

		uint16_t d = (abs(dx) * abs(dx)) + (abs(dy) * abs(dy));

		if (d < sm_d) { sm_x = rx; sm_y = ry; sm_d = d;}
	}

	*o_x = sm_x;
	*o_y = sm_y;

	return 0;	
}
int spread_heatmap(struct t_map* map, uint8_t x, uint8_t y, uint16_t* heatmap_old, uint16_t* heatmap_new) {

	enum movedirections md = 0;

	uint8_t dx = x, dy = y;

	int r = 0;

	for (md = 0; md < MD_COUNT; md++) {
		dx = x + movediff[md][0];
		dy = y + movediff[md][1];
		if ( (vtile(dx,dy)) && ((tflags[map->sq[dy * MAP_WIDTH + dx].type] & TF_SOLID) == 0) && (heatmap_exists(heatmap_old,heatmap_new,dx,dy) == 0) ) r = heatmap_add(heatmap_new,dx,dy); //new tile.
		if (r == 1) return 1;
	}

	return 0;
}
int update_heatmap(struct t_map* map, uint8_t x, uint8_t y, uint16_t* h_old, uint16_t* h_new) {

	int r = 0;

	//if (can_see_entity(map,me,target)) heatmap[(target->y) * MAP_WIDTH + (target->x)] = 2; //new tile

	uint16_t h_add [ HEATMAP_SIZE]; memset(h_add,0xff,sizeof(uint16_t) * HEATMAP_SIZE);

	if ( vtile(x,y) ) { r = heatmap_add(h_new,x,y); if (r != 0) return 1; }

	for (int i=0; i < HEATMAP_SIZE; i++) {

		if (h_new[i] != 65535) {

			uint8_t y = (h_new[i] / MAP_WIDTH);
			uint8_t x = (h_new[i] % MAP_WIDTH);

			h_new[i] = 65535;

			if (heatmap_exists(h_old,NULL,x,y) == 0) { r = heatmap_add(h_old,x,y); if (r != 0) return 1; }
			r = spread_heatmap(map,x,y,h_old,h_add); if (r != 0) return 1;

		}
	}

	for (int i=0; i < HEATMAP_SIZE; i++) {

		if (h_add[i] != 65535) {

			uint8_t y = (h_add[i] / MAP_WIDTH);
			uint8_t x = (h_add[i] % MAP_WIDTH);

			if (heatmap_exists(h_new,NULL,x,y) == 0) { r = heatmap_add(h_new,x,y); if (r != 0) return 1; }

		}
	}
	
	for (int i=0; i < HEATMAP_SIZE; i++) {

		if ( (h_old[i] < (MAP_WIDTH * MAP_HEIGHT)) && (map->aidata.e_viewarr[h_old[i]] >= 3)) h_old[i] = 65535;
		if ( (h_new[i] < (MAP_WIDTH * MAP_HEIGHT)) && (map->aidata.e_viewarr[h_new[i]] >= 3)) h_new[i] = 65535;

	}	

	
	return 0;
}

// -- end of heatmap functions
