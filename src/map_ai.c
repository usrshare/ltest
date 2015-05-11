#include "map_ai.h"
#include "math.h"

#include <stdbool.h>
#include <stdlib.h>

#define PERCEPTION_RADIUS 80

struct t_map_ai_data aient[MAX_AI_ENTITIES];

bool needs_ai [ET_COUNT] = {
	0, //none
	1, //player,
};

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
		uint yy, uint8_t* mem_array) {
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
				mem_array[ay * MAP_WIDTH + ax] = 2;
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
						xy, yx, yy,mem_array);
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
	if (mem_array == NULL) return;

	int xoff, yoff; /* x offset, y offset */

	/* un-see everything */
	//1 means "remembered", 2 means "seen".
	for (int j = 0; j < MAP_HEIGHT; j++) {
		for (int i = 0; i < MAP_WIDTH; i++) {
			if (mem_array[j * MAP_WIDTH + i] == 2) {
				mem_array[j * MAP_WIDTH + i] = 1;
			}
		}
	}
	
	mem_array[(e->y) * MAP_WIDTH + (e->x)] = 2;

	int oc = (e->wideview ? 4 : 2);

	for (int i=0; i < oc; i++) {
		int co = octants[e->viewdir][i];
		cast_light(map, e->x, e->y, 32, 1, 1.0, 0.0, multipliers[0][co], multipliers[1][co], multipliers[2][co], multipliers[3][co], mem_array);
	}

}

// end of shamelessly borrowed code.

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me) {



}

int trymove(struct t_map* map, struct t_map_entity* whom, int8_t dx, int8_t dy) {

	if ((dx < -1) || (dx > 1)) return 1;
	if ((dy < -1) || (dy > 1)) return 1; //this is only for simple movements.

	int time = (abs(dx) + abs(dy)) == 2 ? 6 : 4;

	int rx = whom->x + dx;
	int ry = whom->y + dy;

	if (tflags[map->sq[ry*MAP_WIDTH+rx].type] & TF_SOLID) return -1; //solid

	whom->x = rx;
	whom->y = ry;

	return time;
}

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me) {


	int pch = mapgetch();
	int r = 0;

	switch (pch) {

		case 'h':
			me->viewdir = MD_WEST; 
			r = trymove(map,me,MOVE(WEST)); //left
			break;

		case 'j':
			me->viewdir = MD_SOUTH; 
			r = trymove(map,me,0,1); //down
			break;

		case 'k':
			me->viewdir = MD_NORTH; 
			r = trymove(map,me,0,-1); //up
			break;

		case 'l':
			me->viewdir = MD_EAST; 
			r = trymove(map,me,1,0); //right
			break;

		case 'y':
			me->viewdir = MD_NORTHWEST; 
			r = trymove(map,me,-1,-1); //northwest
			break;

		case 'u':
			me->viewdir = MD_NORTHEAST; 
			r = trymove(map,me,1,-1); //northeast
			break;

		case 'b':
			me->viewdir = MD_SOUTHWEST; 
			r = trymove(map,me,-1,1); //southwest
			break;

		case 'n':
			me->viewdir = MD_SOUTHEAST; 
			r = trymove(map,me,1,1); //southeast
			break;

		case 'o': {

				  enum movedirections dir = askdir();
				  uint8_t dx = me->x + movediff[dir][0];
				  uint8_t dy = me->y + movediff[dir][1];
				  if (map->sq[dy * MAP_WIDTH + dx].type == TT_DOOR_CLOSED) { map->sq[dy * MAP_WIDTH + dx].type = TT_DOOR_OPEN; r = 8;} else { beep(); r = 1;}
				  break; }

		case 'w': //wait
			  r = 1;
	}

	do_fov(map,me,me->aidata->viewarr);	

	if (r < 0) { beep(); return 0;} else return r;
}

struct TILE {
	uint8_t y;
	uint8_t x;
};

