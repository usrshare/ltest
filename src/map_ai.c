#include "map_ai.h"
#include "map_fov.h"

#include <stdbool.h>
#include <stdlib.h>

#define PERCEPTION_RADIUS 80

struct t_map_ai_data aient[MAX_AI_ENTITIES];

bool needs_ai [ET_COUNT] = {
	0, //none
	1, //player
	1, //CPU
	0, //static entity
};

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me) {



}

int space_busy(struct t_map* map, uint8_t x, uint8_t y) {

	if ((x >= MAP_WIDTH) || (y >= MAP_HEIGHT)) return 1;
	if (tflags[map->sq[y*MAP_WIDTH+x].type] & TF_SOLID) return 1; //solid

	if (space_taken(map,x,y)) return 1;
	return 0;
}

int trymove(struct t_map* map, struct t_map_entity* whom, int8_t dx, int8_t dy) {

	if ((dx < -1) || (dx > 1)) return 1;
	if ((dy < -1) || (dy > 1)) return 1; //this is only for simple movements.

	int time = (abs(dx) + abs(dy)) == 2 ? 6 : 4;

	int rx = whom->x + dx;
	int ry = whom->y + dy;

	if (space_busy(map,rx,ry)) return -1; //solid

	whom->x = rx;
	whom->y = ry;

	return time;
}

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me) {

	if (!(me->aidata)) return 0;

	int pch = mapgetch();
	int r = 0;

	switch (pch) {

		case 'h':
			me->aidata->viewdir = MD_WEST; 
			r = trymove(map,me,MOVE(WEST)); //left
			break;

		case 'j':
			me->aidata->viewdir = MD_SOUTH; 
			r = trymove(map,me,0,1); //down
			break;

		case 'k':
			me->aidata->viewdir = MD_NORTH; 
			r = trymove(map,me,0,-1); //up
			break;

		case 'l':
			me->aidata->viewdir = MD_EAST; 
			r = trymove(map,me,1,0); //right
			break;

		case 'y':
			me->aidata->viewdir = MD_NORTHWEST; 
			r = trymove(map,me,-1,-1); //northwest
			break;

		case 'u':
			me->aidata->viewdir = MD_NORTHEAST; 
			r = trymove(map,me,1,-1); //northeast
			break;

		case 'b':
			me->aidata->viewdir = MD_SOUTHWEST; 
			r = trymove(map,me,-1,1); //southwest
			break;

		case 'n':
			me->aidata->viewdir = MD_SOUTHEAST; 
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

