// vim: cin:sts=4:sw=4 
#include "map_ai.h"
#include "mapmode.h"
#include "map_fov.h"
#include "map_path.h"

#include "log.h"
#include "item_ui.h"
#include "map_ui.h"

#include "globals.h"

#include "fight.h"

#include "stealth.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define PERCEPTION_RADIUS 80

struct t_ent_ai_data aient[MAX_AI_ENTITIES];

void map_ai_init (struct t_map* map) {

    heatmap_reset(map->aidata.e_hm);
    heatmap_clear_solid(map,map->aidata.e_hm);
}

enum movedirections face_square (uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy) {
    // this function tries to figure out the best direction in which someone at (sx,sy) has to face to see (dx,dy).

    int diff_x = (dx - sx);
    int diff_y = (dy - sy);

    if ((diff_x == 0) && (diff_y == 0)) return MD_COUNT; //the spots are the same!

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

bool is_enemy(struct t_map_entity* m, struct t_map_entity* e) {
    return (e->type == ET_PLAYER);
}

int space_busy(struct t_map* map, uint8_t x, uint8_t y) {

    if ((x >= MAP_WIDTH) || (y >= MAP_HEIGHT)) return 1;
    if (tflags[map->sq[y*MAP_WIDTH+x].type] & TF_SOLID) return 1; //solid

    if (space_taken(map,x,y)) return 1;
    return 0;
}

int can_see_entity (struct t_map* map, struct t_map_entity* who, struct t_map_entity* whom) {
    return !lineofsight(map, who->x, who->y, whom->x, whom->y, los_default_cb, NULL); 
}

int trymove(struct t_map* map, struct t_map_entity* whom, int8_t dx, int8_t dy) {

    if ((dx < -1) || (dx > 1)) return 1;
    if ((dy < -1) || (dy > 1)) return 1; //this is only for simple movements.

    int agility = whom->ent ? entity_get_attribute(whom->ent,EA_AGI,true) : 5;

    int time = (abs(dx) + abs(dy)) == 2 ? 30 : 20;

    int rx = whom->x + dx;
    int ry = whom->y + dy;

    if (map->sq[ry * MAP_WIDTH + rx].type == TT_DOOR_CLOSED) {
	map->sq[ry * MAP_WIDTH + rx].type = TT_DOOR_OPEN;
	return 40 / agility;
    }

    if (space_busy(map,rx,ry)) {

    lprintf("Entity %p tried to move into busy spot @ (%2d,%2d)\n",whom,rx,ry);
	return -1; //solid
    }

    whom->x = rx;
    whom->y = ry;

    whom->didntmove = 0;
    return time / agility;
}

int e_moveDir(struct t_map* map, struct t_map_entity* whom, enum movedirections md) {

    if (md >= MD_COUNT) return -1;

    int agility = whom->ent ? entity_get_attribute(whom->ent,EA_AGI,true) : 5;

    int time = MD_DIAGONAL(md) ? 30 : 20;

    int rx = whom->x + movediff[md][0];
    int ry = whom->y + movediff[md][1];

    if (map->sq[ry * MAP_WIDTH + rx].type == TT_DOOR_CLOSED) {
	map->sq[ry * MAP_WIDTH + rx].type = TT_DOOR_OPEN;
	return 40 / agility;
    }

    if (space_busy(map,rx,ry)) return -1; //solid

    whom->x = rx;
    whom->y = ry;

    whom->didntmove = 0;
    return time / agility;
}

int ai_setTargetCoord(struct t_map_entity* me, uint8_t x, uint8_t y) {

    if (!me->aidata) return 1;    
    if ((x != me->aidata->dx) || (y != me->aidata->dy)) {
	me->aidata->dy = y;
	me->aidata->dx = x;
	me->aidata->path_plotted = 0;
    }
    return 0;
}

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me) {

    if (me->aidata == NULL) return 1;

    int entities_sz = 0;
    struct t_map_entity* visible_ent[MAX_ENTITIES];
    memset(visible_ent,0,sizeof visible_ent[0] * MAX_ENTITIES);

    do_fov(map,me,12,FA_NORMAL,map->aidata.e_viewarr,&entities_sz,visible_ent);

    if (me->aidata->target == NULL) {

	// there's no target.
	for (int i=0; i < entities_sz; i++) {

	    struct t_map_entity* curent = visible_ent[i]; //let's check visible entities.
	    assert (curent); 

	    if (is_enemy(me,curent) && (me->aidata->timer == 0)) {

		if (disguisecheck(map, curent->ent, me->ent, map->sitealarmtimer, (map->sq[(curent->y) * MAP_WIDTH + (curent->x)].type == TT_RESTRICTED_SPACE)) == 0) { me->aidata->timer = 32; continue; }
		me->aidata->alert_state = 5; me->aidata->task = AIT_CHECKING_OUT; // "?"
		me->aidata->target = curent;

		ai_setTargetCoord(me,curent->x,curent->y);

		if (map->sitealarm == 0) {
		    map->sitealarmtimer = 0;
		} else {
		    me->aidata->task = AIT_CHECKING_OUT;
		    me->aidata->timer = 60;
		}
	    }
	}

    } else {

	// there is a target.

	if (can_see_entity(map,me,me->aidata->target)) {

	    // we don't need a heatmap if we can see the entity.
	    if (map->sitealarm) {
		me->aidata->alert_state = 50;
		me->aidata->task = AIT_ATTACKING;
	    }

	    ai_setTargetCoord(me,me->aidata->target->x, me->aidata->target->y);

	} else {

	    uint8_t dx=255, dy=255;

	    find_closest_on_heatmap(me->x,me->y,map->aidata.e_hm,&dx,&dy);

	    ai_setTargetCoord(me,dx,dy);

	    me->aidata->alert_state--;
	    if (me->aidata->task == AIT_PURSUING) me->aidata->task = AIT_LOOKING_FOR;

	    if (me->aidata->alert_state <= 0) { me->aidata->task = AIT_PATROLLING; me->aidata->target = NULL; heatmap_clear(map->aidata.e_hm);}

	}

    }
    switch(me->aidata->task) {

	case AIT_WORKING:
	    break;

	case AIT_PATROLLING: {
				 //querying the global heatmap for visible spots.

				 uint8_t dx=255, dy=255;

				 find_closest_on_hm_with_path(map,me->x,me->y,map->aidata.e_hm,map->aidata.e_viewarr,&dx,&dy);

				 ai_setTargetCoord(me,dx,dy);
			     }
			     break;

	case AIT_PLEASE_LEAVE:
			     switch(me->aidata->timer) {
				 case 90:
				     msgsay(me,"Please leave the restricted area.\n"); break;
				 case 60:
				     msgsay(me,"I repeat: please leave the restricted area.\n"); break;
				 case 30:
				     msgsay(me,"This is the last warning. If you don't leave this area, I'll use force.\n"); break;
			     }
			     break;

	case AIT_CHECKING_OUT:
	case AIT_PURSUING:
	case AIT_ATTACKING:
	case AIT_FLEEING:
			     break;

	case AIT_LOOKING_FOR:
			     break;
    }

    if ( (me->aidata->path_plotted == 0) && (vtile(me->aidata->dx,me->aidata->dy)) )
    { 
	struct plotflags view = {.persp = me, .eightdir = true};

	memset(me->aidata->ent_target_arr,255,sizeof(uint16_t) * MAP_WIDTH * MAP_HEIGHT);
	plot_path(map,me->x, me->y, me->aidata->dx, me->aidata->dy, NULL, me->aidata->ent_target_arr,0,&view); me->aidata->path_plotted = 1; }

    return 0;
}

int distance_sq(struct t_map_entity* a, struct t_map_entity* b) {

    int dx = (a->x - b->x);
    int dy = (a->y - b->y);

    return dx*dx + dy*dy;
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

	    if (me->aidata->path_plotted) {  
		if (can_see(map,me,dy,dx)) {
		    md = face_square(me->x,me->y,dx,dy);
		    if (md < MD_COUNT) { me->aidata->viewdir = md; r++; } }
		md = roll_downhill(me->aidata->ent_target_arr,me->x,me->y);
		r = e_moveDir(map,me,md); if (r < 0) r = 4;

	    }

	    break;

	case AIT_PLEASE_LEAVE:

	    md = roll_downhill(me->aidata->ent_target_arr,me->x,me->y);
	    r = e_moveDir(map,me,md); if (r < 0) r = 4;

	    if (map->aidata.e_viewarr[dy * MAP_WIDTH +dx] >= 3) {
		md = face_square(me->x,me->y,dx,dy);
		if (md < MD_COUNT) { me->aidata->viewdir = md; r++; } }

	    if ((map->sq[dy * MAP_WIDTH + dx].type != TT_RESTRICTED_SPACE)) {
		me->aidata->task = AIT_PATROLLING;
		me->aidata->timer = 0;
	    } else {
		me->aidata->target->aidata->crimes++;
		if (me->aidata->target->aidata->crimes > 25) me->aidata->task = AIT_PURSUING;
	    }

	    break;

	case AIT_CHECKING_OUT:

	    md = roll_downhill(me->aidata->ent_target_arr,me->x,me->y);
	    r = e_moveDir(map,me,md); if (r < 0) r = 4;
	    if ( (map->aidata.e_viewarr[ dy * MAP_WIDTH + dx ] == 3) && (find_entity(map,dx,dy) == NULL) ) { me->aidata->alert_state = 0; me->aidata->task = AIT_PATROLLING;}
	    break;

	case AIT_ATTACKING: {

				bool noweapons = false;
				if (!me->aidata->target->ent->weapon) noweapons = true;

				if (can_attack(map,me,me->aidata->target,noweapons,0)) {

				    if (can_see(map,me,dy,dx)) {
					md = face_square(me->x,me->y,dx,dy);
					if (md < MD_COUNT) { me->aidata->viewdir = md; r++; } }

				    char actual;
				    attack(map,me->ent,me->aidata->target->ent,0,&actual,noweapons,distance_sq(me,me->aidata->target));
				    r = 16;
				    break;
				}

				//if can't attack, pursue.
			    }

	case AIT_PURSUING:

			    md = roll_downhill(me->aidata->ent_target_arr,me->x,me->y);
			    r = e_moveDir(map,me,md); if (r < 0) r = 4;

			    if (can_see(map,me,dy,dx)) {
				md = face_square(me->x,me->y,dx,dy);
				if (md < MD_COUNT) { me->aidata->viewdir = md; r++; } }
			    break;
	case AIT_FLEEING:
			    //TODO!
			    break;

	case AIT_LOOKING_FOR:

			    md = roll_downhill(me->aidata->ent_target_arr,me->x,me->y);
			    if (md < MD_COUNT) { 
				me->aidata->viewdir = md;
				r = e_moveDir(map,me,md); if (r < 0) r = 4; }
				else r = 4;

				break;
			    }

			    if (r < 0) {return 0;} else return r;
    }

    void map_attack (struct t_map* map, struct t_map_entity* a, uint8_t dx, uint8_t dy) {

	uint8_t bearing = get_bearing(a->x,a->y,dx,dy);

	uint8_t dx_act = a->x, dy_act = a->y;

	int r = aim_bearing(map,&dx_act,&dy_act, bearing);

	struct t_map_entity* enemy_i = find_entity(map,dx,dy); //intended
	struct t_map_entity* enemy_a = find_entity(map,dx_act,dy_act); //actual


	if (r == 0) { 
	    mapaddch("*",dy_act,dx_act);
	    maprefresh();

	    ms_sleep(100);
	}

	if ((r == 0) && (enemy_a) && (enemy_a->ent)) {
	    char actual;
	    char printed;
	    if (!incapacitated(a->ent,0,&printed)) {
		attack(map,a->ent,enemy_a->ent,0,&actual,0,distance_sq(a,enemy_a));
		if (actual) r = 16;
	    } else r = 0;
	} else {


	    if (enemy_i) msgprintw("Your attack missed.\n"); else msgprintw("You shot empty space.\n");
	}
    }

    uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me) {

	if (!(me->aidata)) return 0;

	do_fov(map,me,25,FA_FULL,map->aidata.p_viewarr,NULL,NULL);	
	update_player_map(map, me,1);

	if (me->didntmove == 0) {
	    // look for loot.
	    struct t_map_entity* e = NULL; int i=0;
	    e = find_entity_ind(map,me->x,me->y,i);

	    while (e) {
		if (e->type == ET_LOOT) {

		    if (!(e->loot)) { msgprintw("ERROR: this loot item has no actual loot.\n"); return 0; }

		    msgprintw("You see: %s (%d)\n", describe_item(e->loot),e->loot->itemcount);
		}
		i++;
		e = find_entity_ind(map,me->x,me->y,i);
	    };
	}

	me->didntmove = 1;
	return 0;
    }
    uint16_t player_actFunc(struct t_map* map, struct t_map_entity* me) {

	if (!(me->aidata)) return 0;

	do_fov(map,me,25,FA_FULL,map->aidata.p_viewarr,NULL,NULL);	
	update_player_map(map, me,1);

	// check for input
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
		msgprintw("A really long sequence of text. 1\n");
		msgprintw("A really long sequence of text. 2\n");
		msgprintw("A really long sequence of text. 3\n");
		msgprintw("A really long sequence of text. 4\n");
		msgprintw("A really long sequence of text. 5\n");
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
			  //enum movedirections dir = askdir();
			  uint8_t dx = me->x;
			  uint8_t dy = me->y;
			  int r = askpos(&dy, &dx);
			  if (r == 0) {
			      map_attack(map,me,dx,dy);
			  }

			  break; }
	    case ',': {
			  int i=0;
			  struct t_map_entity* e = NULL;
			  e = find_entity_ind(map,me->x,me->y,i);

			  while (e) {

			      if (e->type == ET_LOOT) {

				  if (!(e->loot)) { msgprintw("ERROR: this loot item has no actual loot.\n"); return 0; }
				  msgprintw("Picked up %s (%d)\n", describe_item(e->loot),e->loot->itemcount);
				  inv_join(me->ent->inventory, e->loot);
				  remove_map_entity(e); 
				  return 16;
			      }
			      i++;
			      e = find_entity_ind(map,me->x,me->y,i);
			  }

			  msgprintw("Nothing to pick up.\n");
			  return 0;

			  break; }

	    case 'w': //wait
		      r = 1;
	}

	do_fov(map,me,25,FA_FULL,map->aidata.p_viewarr,NULL,NULL);	
	update_player_map(map, me,0);

	char printed;	
	incapacitated(me->ent,1,&printed);

	if (r < 0) { nc_beep(); return 0;} else return r;
    }

    int static_plot(struct t_map* map) {

	update_heatmap(map,map->aidata.e_hm,255,255);

	if (map->aidata.targets_moved ) {
	    struct plotflags pf = {.eightdir = 1, .no_clear_patharr = 1};

	    memset(map->aidata.e_targets_arr, 255, sizeof(uint16_t) * MAP_WIDTH * MAP_HEIGHT);

	    for (int i=0; i < SQUAD_MAX; i++)
		if (map->aidata.targets[i])
		    map->aidata.e_targets_arr [ map->aidata.targets_y[i] * MAP_WIDTH + map->aidata.targets_y[i] ] =0;

	    plot_dijkstra_map(map,NULL,map->aidata.e_targets_arr,0,&pf); 

	    map->aidata.targets_moved = false;
	}

	//this function plots paths for everyone.

	return 0;
    }
