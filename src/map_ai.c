#include "map_ai.h"
#include "mapmode.h"
#include "map_fov.h"
#include "map_path.h"

#include "map_ui.h"

#include "globals.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

int can_see_entity (struct t_map* map, struct t_map_entity* who, struct t_map_entity* whom) {
	int yx = (whom->y) * MAP_WIDTH + (whom->x);
	if (yx >= MAP_WIDTH * MAP_HEIGHT) return 0; //invalid space.
	return (who->aidata->viewarr[yx] >= 3) ? 1 : 0;

}

int trymove(struct t_map* map, struct t_map_entity* whom, int8_t dx, int8_t dy) {

	if ((dx < -1) || (dx > 1)) return 1;
	if ((dy < -1) || (dy > 1)) return 1; //this is only for simple movements.

	int time = (abs(dx) + abs(dy)) == 2 ? 6 : 4;

	int rx = whom->x + dx;
	int ry = whom->y + dy;

	if (map->sq[ry * MAP_WIDTH + rx].type == TT_DOOR_CLOSED) {
		map->sq[ry * MAP_WIDTH + rx].type = TT_DOOR_OPEN;
		return 8;
	}

	if (space_busy(map,rx,ry)) return -1; //solid

	whom->x = rx;
	whom->y = ry;

	return time;
}

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
int update_heatmap(struct t_map* map, struct t_map_entity* me, uint8_t x, uint8_t y) {

	uint16_t* h_old = me->aidata->heatmap_old;
	uint16_t* h_new = me->aidata->heatmap_new;


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

		if ( (h_old[i] < (MAP_WIDTH * MAP_HEIGHT)) && (me->aidata->viewarr[h_old[i]] >= 3) ) h_old[i] = 65535;
		if ( (h_new[i] < (MAP_WIDTH * MAP_HEIGHT)) && (me->aidata->viewarr[h_new[i]] >= 3) ) h_new[i] = 65535;

	}	

	
	return 0;
}

// -- end of heatmap functions

int knock_out (struct t_map* map, struct t_map_entity* me, uint8_t x, uint8_t y) {
	return 1;
}

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me) {

	if (me->aidata == NULL) return 1;

	int entities_sz = 0;
	do_fov(map,me,12,FA_NORMAL,me->aidata->viewarr,&entities_sz);

	struct t_map_entity* visible_ent[entities_sz];
	memset(visible_ent,0,sizeof(struct t_map_entity*) * entities_sz);
	find_visible_entities(map, me->aidata->viewarr, visible_ent, entities_sz);

	uint8_t dx, dy;

	if (me->aidata->target == NULL) {

		// there's no target.
		for (int i=0; i < entities_sz; i++) {

			struct t_map_entity* curent = visible_ent[i]; //let's check visible entities.
			if (curent == NULL) continue; //this shouldn't really happen, but fine.

			if (curent->type == ET_PLAYER) {

				if ( map->sq[(curent->y) * MAP_WIDTH + (curent->x)].type != TT_RESTRICTED_SPACE ) continue;


				me->aidata->alert_state = 5; me->aidata->task = AIT_CHECKING_OUT; // "?"
				me->aidata->target = curent;

				me->aidata->dx = me->aidata->lx = dx =curent->x; 
				me->aidata->dy = me->aidata->ly = dy =curent->y; 
				me->aidata->path_plotted = 0;

				if (map->alert_state == AL_NORMAL) {
					map->alert_state = AL_SUSPICIOUS; map->alert_time = 60;
					me->aidata->task = AIT_PLEASE_LEAVE; me->aidata->timer = 60;
				} else {
					me->aidata->task = AIT_CHECKING_OUT; me->aidata->timer = 60; }
			}
		}

	} else {
		

		if (can_see_entity(map,me,me->aidata->target)) {
	
			
			// we don't need a heatmap if we can see the entity.
			if (me->aidata->task == AIT_LOOKING_FOR) {heatmap_clear(me->aidata->heatmap_old); heatmap_clear(me->aidata->heatmap_new);}
				
			if (me->aidata->task >= AIT_CHECKING_OUT) {
			me->aidata->alert_state = 50;
			me->aidata->task = AIT_PURSUING; }

			dx = me->aidata->target->x; dy = me->aidata->target->y;

			if ((dx != me->aidata->dx) || (dy != me->aidata->dy)) {
				me->aidata->dy = dy;
				me->aidata->dx = dx;
				me->aidata->path_plotted = 0;
			}
				
			me->aidata->ly = dy;
			me->aidata->lx = dx;


		} else {
			
			update_heatmap(map,me,me->aidata->lx,me->aidata->ly);
			me->aidata->lx = 255; me->aidata->ly = 255;
				
			find_closest_on_heatmap(me->x,me->y,me->aidata->heatmap_old,me->aidata->heatmap_new,&dx,&dy);
			if ((dx != me->aidata->dx) || (dy != me->aidata->dy)) {
				me->aidata->dy = dy;
				me->aidata->dx = dx;
				me->aidata->path_plotted = 0;
			}

			me->aidata->alert_state--;
			if (me->aidata->task == AIT_PURSUING) me->aidata->task = AIT_LOOKING_FOR;

			if (me->aidata->alert_state <= 0) { me->aidata->task = AIT_PATROLLING; me->aidata->target = NULL; heatmap_clear(me->aidata->heatmap_old); heatmap_clear(me->aidata->heatmap_new); }

		}

		// there is a target.
	}

	if ( (me->aidata->path_plotted == 0) && (vtile(dx,dy)) )
	{ plot_path(map,me,dx,dy,me->aidata->patharr,me->aidata->pathprev); me->aidata->path_plotted = 1; }
	
	switch(me->aidata->task) {

		case AIT_WORKING:
			break;

		case AIT_PATROLLING:
			break;

		case AIT_PLEASE_LEAVE:
			switch(me->aidata->timer) {
				case 60:
				statsay(me,"Please leave the restricted area.\n"); break;
				case 30:
				statsay(me,"I repeat: please leave the restricted area.\n"); break;
				case 15:
				statsay(me,"This is the last warning. If you don't leave this area, I'll use force.\n"); break;
			}
			break;

		case AIT_CHECKING_OUT:
			break;

		case AIT_PURSUING:
			break;

		case AIT_LOOKING_FOR:
			break;
	}

	return 0;
}
uint16_t enemy_actFunc(struct t_map* map, struct t_map_entity* me) {

	if (me->aidata == NULL) return 0;

	int r = 1;

	uint8_t dx = me->aidata->dx,dy = me->aidata->dy;

	enum movedirections md;
	switch(me->aidata->task) {

		case AIT_WORKING:
			r = 16;
			break;

		case AIT_PATROLLING:

			me->aidata->viewdir = (me->aidata->viewdir + 1) % 8;
			r = 16;
			break;

		case AIT_PLEASE_LEAVE:

			md = plot_follow(me->x,me->y,me->aidata->pathprev);
			if (md < MD_COUNT) r = trymove(map,me,movediff[md][0],movediff[md][1]); else r = 4;

			if (me->aidata->viewarr[dy * MAP_WIDTH +dx] >= 3) {
				md = face_square(me->x,me->y,dx,dy);
				if (md < MD_COUNT) { me->aidata->viewdir = md; r++; } }

			if ((map->sq[dy * MAP_WIDTH + dx].type != TT_RESTRICTED_SPACE)) {
				me->aidata->task = AIT_PATROLLING;
				me->aidata->timer = 0;
			}

			break;

		case AIT_CHECKING_OUT:

			md = plot_follow(me->x,me->y,me->aidata->pathprev);
			if (md < MD_COUNT) r = trymove(map,me,movediff[md][0],movediff[md][1]); else r = 4;
			if ( (me->aidata->viewarr[ dy * MAP_WIDTH + dx ] == 3) && (find_entity(map,dx,dy) == NULL) ) { me->aidata->alert_state = 0; me->aidata->task = AIT_PATROLLING;}
			break;

		case AIT_PURSUING:

			md = plot_follow(me->x,me->y,me->aidata->pathprev);
			if (md < MD_COUNT) r = trymove(map,me,movediff[md][0],movediff[md][1]); else r = 4;

			if (me->aidata->viewarr[dy * MAP_WIDTH +dx] >= 3) {
				md = face_square(me->x,me->y,dx,dy);
				if (md < MD_COUNT) { me->aidata->viewdir = md; r++; } }
			break;
		case AIT_LOOKING_FOR:
			
			md = plot_follow(me->x,me->y,me->aidata->pathprev);
			if (md < MD_COUNT) { 
				me->aidata->viewdir = md;
				r = trymove(map,me,movediff[md][0],movediff[md][1]);}  else r = 4;

			break;
	}

	if (r < 0) {return 0;} else return r;
}

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me) {

	if (!(me->aidata)) return 0;
	
	do_fov(map,me,25,FA_WIDE,me->aidata->viewarr,NULL);	
	draw_map(map, me,1,dbgmode ? 1 : 0, dbgmode ? 1 : 0,1);
	
	return 0;
}
uint16_t player_actFunc(struct t_map* map, struct t_map_entity* me) {

	if (!(me->aidata)) return 0;
	
	do_fov(map,me,25,FA_WIDE,me->aidata->viewarr,NULL);	
	draw_map(map, me,1,dbgmode ? 1 : 0, dbgmode ? 1 : 0,1);
	
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

		case 'q':
			statprintw("A really long sequence of text. 1\n");
			statprintw("A really long sequence of text. 2\n");
			statprintw("A really long sequence of text. 3\n");
			statprintw("A really long sequence of text. 4\n");
			statprintw("A really long sequence of text. 5\n");
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

		case 'f': {
				  enum movedirections dir = askdir();
				  uint8_t dx = me->x + movediff[dir][0];
				  uint8_t dy = me->y + movediff[dir][1];
				  me->aidata->viewdir = dir; 
				  struct t_map_entity* enemy = find_entity(map,dx,dy);
				  if ((enemy) && (enemy->aidata) && (enemy->aidata->alert_state == 0)) {
					  kill_entity(enemy); r = 16;
				  } else {statprintw("You can't disable an alerted enemy.\n"); r = 4;}

				  break; }

		case 'w': //wait
			  r = 1;
	}

	do_fov(map,me,25,FA_WIDE,me->aidata->viewarr,NULL);	
	draw_map(map, me,1,dbgmode ? 1 : 0, dbgmode ? 1 : 0,0);
	
	if (r < 0) { nc_beep(); return 0;} else return r;
}

struct TILE {
	uint8_t y;
	uint8_t x;
};

