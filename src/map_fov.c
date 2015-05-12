#include "map_fov.h"
#include "map_ai.h"
#include "mapmode.h"

#include <math.h>
#include <stdlib.h>
int vispass(struct t_map* map, uint8_t x, uint8_t y) {

	return !(tflags[map->sq[y * MAP_WIDTH + x].type] & TF_BLOCKS_VISION);
}

int vtile(uint8_t x, uint8_t y) {

	return ((x < MAP_WIDTH) && (y < MAP_HEIGHT));
}

// the following code is shamelessly borrowed from the shadowcasting
// code at http://www.roguebasin.com/index.php?title=C%2B%2B_shadowcasting_implementation.
// the only change made is the list of octants, so that the vision area is a 90
// degree wide one instead of a full circle.

typedef unsigned int uint;

static int multipliers[4][8] = {
	{1, 0, 0, -1, -1, 0, 0, 1},
	{0, 1, -1, 0, 0, -1, 1, 0},
	{0, 1, 1, 0, 0, -1, -1, 0},
	{1, 0, 0, 1, -1, 0, 0, -1}
};

static int octants[MD_COUNT][4] = {
	{0,3,1,2}, //n
	{2,3,0,5}, //ne
	{2,5,4,3}, //e
	{4,5,2,7}, //se
	{4,7,5,6}, //s
	{6,7,1,4}, //sw
	{1,6,0,7}, //w
	{0,1,3,6}, //nw
};

void cast_light(struct t_map* map, uint x, uint y, uint radius, uint row,
		float start_slope, float end_slope, uint xx, uint xy, uint yx,
		uint yy, uint8_t* mem_array, bool* flag_updated) {
	if (start_slope < end_slope) {
		return;
	}
	float next_start_slope = start_slope;
	for (uint i = row; i <= radius; i++) {
		bool blocked = false;
		for (int dx = -i, dy = -i; dx <= 0; dx++) {
			float l_slope = (dx - 0.5) / (dy + 0.5);
			float r_slope = (dx + 0.5) / (dy - 0.5);
			if (start_slope < r_slope) {
				continue;
			} else if (end_slope > l_slope) {
				break;
			}

			int sax = dx * xx + dy * xy;
			int say = dx * yx + dy * yy;
			if ((sax < 0 && (uint)abs(sax) > x) ||
					(say < 0 && (uint)abs(say) > y)) {
				continue;
			}
			uint ax = x + sax;
			uint ay = y + say;
			if (ax >= MAP_WIDTH || ay >= MAP_HEIGHT) {
				continue;
			}

			uint radius2 = radius * radius;
			if ((uint)(dx * dx + dy * dy) < radius2) {
				
				if (flag_updated) {

				int ov = mem_array[ay * MAP_WIDTH + ax];
				if (ov == 0) (*flag_updated) = 1; }

				mem_array[ay * MAP_WIDTH + ax] = 2;

				struct t_map_entity* ent = find_entity(map,ax,ay);
				if (ent) mem_array [ay * MAP_WIDTH + ax] = 3;
			}

			if (blocked) {
				if (!vispass(map,ax, ay)) {
					next_start_slope = r_slope;
					continue;
				} else {
					blocked = false;
					start_slope = next_start_slope;
				}
			} else if (!vispass(map,ax, ay)) {
				blocked = true;
				next_start_slope = r_slope;
				cast_light(map, x, y, radius, i + 1, start_slope, l_slope, xx,
						xy, yx, yy,mem_array, flag_updated);
			}
		}
		if (blocked) {
			break;
		}
	}
}

/* calculate which tiles can be seen by the player */
void do_fov(struct t_map* map, struct t_map_entity* e, uint8_t* mem_array) {

	if (e == NULL) return;
	if (e->aidata == NULL) return;
	if (mem_array == NULL) return;

	int flag_updated = 0;

	/* un-see everything */
	//1 means "remembered", 2 means "seen".
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			if (mem_array[j * MAP_WIDTH + i] >= 2) {
				mem_array[j * MAP_WIDTH + i] = 1;
			}
		}
	}
	
	mem_array[(e->y) * MAP_WIDTH + (e->x)] = 2;

	int oc = (e->aidata->wideview ? 4 : 2);

	for (int i=0; i < oc; i++) {
		int co = octants[e->aidata->viewdir][i];
		cast_light(map, e->x, e->y, 32, 1, 1.0, 0.0, multipliers[0][co], multipliers[1][co], multipliers[2][co], multipliers[3][co], mem_array, &flag_updated);
	}

	if (flag_updated) e->aidata->viewarr_updated = 1;
	
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			if (mem_array[j * MAP_WIDTH + i] == 3) {
				if (e->vision_cb) e->vision_cb(map,e,j,i,find_entity(map,i,j));
			}
		}
	}
}

// end of shamelessly borrowed code.
