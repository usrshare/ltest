#include "map_ai.h"
#include "mapmode.h"
#include "map_fov.h"
#include "map_path.h"

#include <stdbool.h>
#include <stdlib.h>

#define PERCEPTION_RADIUS 80

struct t_map_ai_data aient[MAX_AI_ENTITIES];

enum movedirections face_square (uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy) {

	// this function tries to figure out the best direction in which someone at (sx,sy) has to face to see (dx,dy).

	int diff_x = (dx - sx);
	int diff_y = (dy - sy);

	if ((diff_x == 0) && (diff_y == 0)) return -1; //the spots are the same!

	if (abs(diff_x) > 2 * abs(diff_y)) return (diff_x > 0) ? MD_EAST: MD_WEST;

	if (abs(diff_y) > 2 * abs(diff_x)) return (diff_y > 0) ? MD_SOUTH: MD_NORTH;

	// at this point, all remaining directions are diagonal.

	if (diff_x > 0) return (diff_y > 0) ? MD_SOUTHEAST : MD_NORTHEAST; else return (diff_y > 0) ? MD_SOUTHWEST : MD_NORTHWEST;

}

bool needs_ai [ET_COUNT] = {
	0, //none
	1, //player
	1, //CPU
	0, //static entity
};

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

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me) {

	int r = 0;

	if (me->aidata == NULL) return 16;

	int entities_sz = 0;

	do_fov(map,me,me->aidata->viewarr,&entities_sz);

	struct t_map_entity* visible_ent[entities_sz];

	find_visible_entities(map, me->aidata->viewarr, visible_ent, entities_sz);

	for (int i=0; i < (entities_sz); i++ ) {

		struct t_map_entity* curent = visible_ent[i];

		if (curent) {

		if (curent->type == ET_PLAYER) {

			int x = curent->x; int y = curent->y;

			(me->aidata->alert_state)++;

			if ((me->aidata->alert_state) == 1) me->aidata->task = AIT_CHECKING_OUT;

			if ((me->aidata->alert_state) >= 3) me->aidata->task = AIT_PURSUING; (me->aidata->alert_state) = 20;

			me->aidata->target = curent;

			if ((me->aidata->dx != x) || (me->aidata->dy != y)) {

				me->aidata->dx = x; me->aidata->dy = y;
				me->aidata->path_plotted = 0;
			}

		}
		}
	}



	enum movedirections md;

	switch(me->aidata->task) {

		case AIT_WORKING:
			r = 16;

			break;

		case AIT_PATROLLING:

			me->aidata->viewdir = (me->aidata->viewdir + 1) % 8;
			r = 16;

			break;

		case AIT_CHECKING_OUT:

			if (me->aidata->path_plotted == 0) { plot_path(map,me,me->aidata->dx,me->aidata->dy,me->aidata->patharr,me->aidata->pathprev); me->aidata->path_plotted = 1; }

			md = plot_follow(map,me,me->aidata->patharr,me->aidata->pathprev);
			if (md != -1) r = trymove(map,me,movediff[md][0],movediff[md][1]); else r = 4;

			if ( (me->aidata->viewarr[ (me->aidata->dy) * MAP_WIDTH + (me->aidata->dx) ] == 3) && (find_entity(map,(me->aidata->dx),(me->aidata->dy)) == NULL) ) { me->aidata->alert_state = 0; me->aidata->task = AIT_PATROLLING;}

			break;

		case AIT_PURSUING:

			if (me->aidata->path_plotted == 0) { plot_path(map,me,me->aidata->dx,me->aidata->dy,me->aidata->patharr,me->aidata->pathprev); me->aidata->path_plotted = 1; }

			md = plot_follow(map,me,me->aidata->patharr,me->aidata->pathprev);
			if (md != -1) r = trymove(map,me,movediff[md][0],movediff[md][1]); else r = 4;

			md = face_square(me->x,me->y,me->aidata->dx,me->aidata->dy);
			if (md != -1) { me->aidata->viewdir = md; r++; }

			me->aidata->alert_state--;
			if (me->aidata->alert_state == 0) me->aidata->task = AIT_PATROLLING;

			break;
	}

	if (r < 0) {return 0;} else return r;

	return r;
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
				  if (map->sq[dy * MAP_WIDTH + dx].type == TT_DOOR_CLOSED) { map->sq[dy * MAP_WIDTH + dx].type = TT_DOOR_OPEN; r = 8;} else { nc_beep(); r = 1; }
				  break; }
		case 'c': {

				  enum movedirections dir = askdir();
				  uint8_t dx = me->x + movediff[dir][0];
				  uint8_t dy = me->y + movediff[dir][1];
				  if (map->sq[dy * MAP_WIDTH + dx].type == TT_DOOR_OPEN) { map->sq[dy * MAP_WIDTH + dx].type = TT_DOOR_CLOSED; r = 8;} else { nc_beep(); r = 1;}
				  break; }

		case 'w': //wait
			  r = 1;
	}

	do_fov(map,me,me->aidata->viewarr,NULL);	

	if (r < 0) { nc_beep(); return 0;} else return r;
}

struct TILE {
	uint8_t y;
	uint8_t x;
};

