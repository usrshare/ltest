// vim: cin:sts=4:sw=4 

#include "globals.h"

#include "mapmode.h"
#include "entity_types.h"
#include "mapgen.h"
#include "map_fov.h"

#include "random.h"
#include "map_ui.h"

#include "location.h"
#include "armor.h"
#include "weapon.h"

#include <stdarg.h>
#include <string.h> //memset

enum terrainflags tflags[TT_ELEMENT_COUNT] = {
    TF_OUTSIDE, //outside
    TF_OUTSIDE, //grass
    TF_OKAY_DOOR, //corridor
    TF_OKAY_DOOR, //space
    TF_OKAY_DOOR, //restricted space
    TF_NOSPAWN, //special space
    TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_SOUND | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //wall
    TF_SOLID | TF_BLOCKS_SOUND | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //window
    TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //closed door
    TF_NOSPAWN, //open door
    TF_SOLID | TF_NOSPAWN, //bars
    TF_SOLID | TF_DODGE | TF_NOSPAWN | TF_CONTAINS_LOOT, //table,
    TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_SOUND | TF_NOSPAWN | TF_CONTAINS_LOOT, //locker
    TF_NOSPAWN, //custom
    TF_NOSPAWN, //stairs
    TF_SOLID | TF_BLOCKS_VISION | TF_BLOCKS_SOUND | TF_BLOCKS_ATTACKS | TF_NOSPAWN, //unknown
};

int movediff[MD_COUNT][2] = {
    {0,-1},
    {1,-1},
    {1,0},
    {1,1},
    {0,1},
    {-1,1},
    {-1,0},
    {-1,-1}};

int vtile(uint8_t x, uint8_t y) { return ((x < MAP_WIDTH) && (y < MAP_HEIGHT)); }

struct t_item* next_empty_loot(struct t_map* map) {
    for (int i=0; i < MAX_LOOTS; i++)
	if (map->inventory[i*INVENTORY_SIZE].type == IT_NONE) return &(map->inventory[i*INVENTORY_SIZE]);

    return NULL;
}

struct t_map_entity* next_empty_entity(struct t_map* map) {
    for (int i=0; i < MAX_ENTITIES; i++)
	if (map->ent[i].type == ET_NONE) return &(map->ent[i]);

    return NULL;
}

struct t_creature* next_empty_temp_entity(struct t_map* map) {
    for (int i=0; i < MAX_ENTITIES; i++)
	if (map->creat[i].type == 0) return &(map->creat[i]);

    return NULL;
}

struct t_ent_ai_data* next_empty_ai_data(void) {
    for (int i=0; i < MAX_AI_ENTITIES; i++)
	if (aient[i].usedby == NULL) return &(aient[i]);

    return NULL;
}


int make_turn(struct t_map* map) {
    for (int i=0; i < MAX_ENTITIES; i++) {
	struct t_map_entity* e = (map->ent)+i;
	if (e->type != ET_NONE) {

	    if (e->twait > 0) e->twait--;
	    if (e->turn) e->turn(map,e);
	    if ((e->twait == 0) && (e->act)) e->twait += e->act(map,e);

	}
    }
    return 0;
}

int check_conditions(struct t_map* map) {

    //checks if conditions are OK for the next move:
    // at least one player entity should be alive.

    int living_players = 0;

    for (int i=0; i < MAX_ENTITIES; i++) {
	struct t_map_entity* e = &map->ent[i];
	if (e->type == ET_PLAYER) living_players++;


	if ((e->ent) && (!e->ent->alive)) kill_entity(e);
    }

    if (living_players == 0) return 0;
    return 1;
}

struct t_map_entity* find_entity(struct t_map* map, uint8_t x, uint8_t y) {

    for (int i=0; i < MAX_ENTITIES; i++) {
	struct t_map_entity* e = &map->ent[i];
	if ((e->x == x) && (e->y == y)) return e;
    }
    return NULL;
}

bool can_attack(struct t_map* map, struct t_map_entity* a, struct t_map_entity* t, bool no_ranged, bool no_melee) {

    int dx = abs(a->x - t->x);
    int dy = abs(a->y - t->y);

    if (!a->ent) return false;

    bool melee_allowed = !no_melee;
    bool ranged_allowed = !no_ranged;

    if ((dx > 1) || (dy > 1)) melee_allowed = 0; //melee can only go so far.

    //TODO if no line of sight between a and t, ranged_allowed = 0;
    if (lineofsight(map,a->x,a->y,t->x,t->y,los_default_cb,NULL) > 0) ranged_allowed = 0;

    if ((!melee_allowed) && (!ranged_allowed)) return false;

    const struct t_attackst* att = get_attack(a->ent->weapon, !melee_allowed, !ranged_allowed, 0);

    if (att) return true; else return false;

}

int space_taken(struct t_map* map, uint8_t x, uint8_t y) {

    for (int i=0; i < MAX_ENTITIES; i++) {
	struct t_map_entity* e = &map->ent[i];
	if ((e->type != ET_NONE) && (e->x == x) && (e->y == y)) return 1;
    }

    return 0;
}

struct t_map_entity* spawn_entity(struct t_map* map, enum entitytypes type, struct spawnflags sf) {

    struct t_map_entity* newent = next_empty_entity(map);
    if (newent == NULL) return NULL;

    if (type == ET_LOOT) {
	struct t_item* loot = next_empty_loot(map);
	if (loot == NULL) return NULL; else newent->loot = loot;
    }

    if (needs_ai[type]) {
	struct t_ent_ai_data* newai = next_empty_ai_data();
	if (newai == NULL) return NULL;

	newent->aidata = newai;
	newai->dx = 255;
	newai->dy = 255;
	newai->usedby = newent;
    }

    newent->type = type;

    int x,y;

    if (sf.gen_creature) {

	struct t_creature* newcr = next_empty_temp_entity(map);
	if (newcr == NULL) return NULL;

	creature_init(newcr,sf.genrules);
	newent->ent = newcr;

	if (newent->type == ET_PLAYER) newent->ent->name_known = 1;
    }

    switch(sf.position) {

	case SF_DEFAULT: {

			     int i=0;

			     do {
				 x = map->spawn_points[i].x; y = map->spawn_points[i].y;
				 i++;
			     } while ( space_taken(map,x,y) );
			     break; }
	case SF_FIXED:
			 x = sf.x; y = sf.y;
			 break;
	case SF_RANDOM:

			 do {
			     x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
			 } while ( ( tflags[map->sq[y*MAP_WIDTH+x].type] & TF_NOSPAWN ) || space_taken(map,x,y) );
			 break;

	case SF_RANDOM_INSIDE: {

				   int tf = 0;
				   do {
				       x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
				       tf = tflags[map->sq[y*MAP_WIDTH+x].type];
				   } while ( (tf & TF_OUTSIDE) || (tf & TF_NOSPAWN) || space_taken(map,x,y) );
				   break; }
	case SF_RANDOM_RESTRICTED:

			       do {
				   x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
			       } while ( (map->sq[y*MAP_WIDTH+x].type != TT_RESTRICTED_SPACE) || space_taken(map,x,y) );
			       break;
	case SF_RANDOM_SPECIAL:

			       do {
				   x = randval(MAP_WIDTH); y = randval(MAP_HEIGHT);
			       } while ( (map->sq[y*MAP_WIDTH+x].type != TT_SPECIAL_SPACE) || space_taken(map,x,y) );
			       break;


    }

    newent->x = x;
    newent->y = y;
    newent->turn = sf.tf;
    newent->act = sf.af;
    return newent;
}

int kill_entity(struct t_map_entity* ent) {

    //this removes the entity and all references to it.

    ent->type = ET_NONE;

    struct t_ent_ai_data* ai = ent->aidata;
    ent->turn = ent->act = NULL;
    if (ai) memset(ai,0,sizeof(struct t_ent_ai_data));
    ent->aidata = NULL;
    return 0;
}

int mapmode() {

    struct t_map map1;

    memset(&(map1.sq), 0, sizeof(struct t_square) * MAP_WIDTH * MAP_HEIGHT); 
    memset(&(map1.ent), 0, sizeof(struct t_map_entity) * MAX_ENTITIES); 
    memset(&(map1.creat), 0, sizeof(struct t_creature) * MAX_ENTITIES); 
    memset(aient, 0, sizeof(struct t_ent_ai_data) * MAX_AI_ENTITIES); 

    map1.sitealarm = 0; map1.sitealarmtimer = -1;

    generate_buildings(&map1,GM_SINGLE);

#define PLAYERS_COUNT 1

    struct t_map_entity* players[PLAYERS_COUNT];

    for (int i=0; i < PLAYERS_COUNT; i++) {
	players[i] = spawn_entity(&map1,ET_PLAYER,(struct spawnflags){
		.gen_creature = true,
		.genrules = &type_rules[ET_HIPPIE],
		.position = SF_DEFAULT,
		.tf = player_turnFunc,
		.af = player_actFunc}); //temporary entity
	if (players[i]) {
	    players[i]->flags |= EF_ALWAYSVISIBLE;
	    players[i]->e_id = i;
	    players[i]->aidata->wideview = 1;

	    struct t_item clothes;
	    new_armor(ARMOR_CLOTHES,&clothes);
	    give_armor(players[i]->ent,clothes);

	    struct t_item knife;
	    new_weapon(WT_COMBATKNIFE,&knife,0);
	    give_weapon(players[i]->ent,knife);
	    
	}
    }

#define ENEMIES_COUNT 10

    struct t_map_entity* enemies[ENEMIES_COUNT];

    for (int i=0; i < ENEMIES_COUNT; i++) {
	enemies[i] = spawn_entity(&map1,ET_CPU,(struct spawnflags){
		.gen_creature = true,
		.genrules = &type_rules[ET_SECURITYGUARD],
		.position = SF_RANDOM_INSIDE,
		.tf = enemy_turnFunc,
		.af = enemy_actFunc});
	if (enemies[i]) {enemies[i]->aidata->task = AIT_PATROLLING;}
    }

    for (int i=0; i < PLAYERS_COUNT; i++) {
	do_fov(&map1,players[i],25,FA_FULL,map1.aidata.p_viewarr,NULL);
	draw_map(&map1,players[i],1,dbgmode ? 1 : 0, dbgmode ? 1 : 0,0);
    }

    map_ui_init(&map1);

    int loop = 1;
    map1.time = 0;
    do {
	updheader(&map1);
	update_status(&map1);
	update_ui(&map1);
	make_turn(&map1);
	loop = check_conditions(&map1);
	map1.time++;



	if (map1.sitealarmtimer > 0) { 
	    map1.sitealarmtimer--;

	if (map1.sitealarmtimer <= 0) msgprintw("The squad smells Conservative panic.");
	}

	for (int i = 0; i < MAX_ENTITIES; i++) {
	    if ((map1.ent[i].type != ET_NONE) && (map1.ent[i].aidata) && (map1.ent[i].aidata->timer > 0)) map1.ent[i].aidata->timer--; }
    } while (loop);

    msgprintw("Your entire squad has been eliminated. Game over, I guess.");
    update_status(&map1);
    update_ui(&map1);
    mapgetch();

    map_ui_free(&map1);
    return 0;
}

char hasdisguise(struct t_creature* cr, bool restricted) {
    short type = -1;
    return 0;
    if(cursite>=0)type = location[cursite]->type;
    char uniformed=0;

    // Never uniformed in battle colors
    //if(activesquad->stance==SQUADSTANCE_BATTLECOLORS)
    //   return 0;

    if(location[cursite]->siege.siege)
    {
	switch(location[cursite]->siege.siegetype)
	{
	    case SIEGE_CIA:
		{
		    if(a_type(cr->armor)->type == ARMOR_BLACKSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_BLACKDRESS)uniformed=1;
		    break;
		}
	    case SIEGE_CORPORATE:
		{
		    if(a_type(cr->armor)->type == ARMOR_MILITARY)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_ARMYARMOR)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SEALSUIT)uniformed=1;
		    break;
		}
	    case SIEGE_HICKS:
		{
		    if(a_type(cr->armor)->type == ARMOR_CLOTHES)uniformed=2;
		    if(a_type(cr->armor)->type == ARMOR_OVERALLS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_WIFEBEATER)uniformed=1;
		    break;
		}
	    case SIEGE_CCS:
		{
		    // CCS has trained in anticipation of this tactic
		    // There is no fooling them
		    // (They pull this shit all the time in their own sieges)
		    uniformed=0;
		    break;
		}
	    case SIEGE_POLICE:
		{
		    if(a_type(cr->armor)->type == ARMOR_SWATARMOR&&
			    location[cursite]->siege.escalationstate==0)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_MILITARY&&
			    location[cursite]->siege.escalationstate>0)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_ARMYARMOR&&
			    location[cursite]->siege.escalationstate>0)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SEALSUIT&&
			    location[cursite]->siege.escalationstate>0)uniformed=1;
		    break;
		}
	    case SIEGE_FIREMEN:
		{
		    if(a_type(cr->armor)->type == ARMOR_BUNKERGEAR)uniformed=1;
		    break;
		}
	}
    }
    else
    {
	if(((cr->armor != NULL)||cr->animalgloss==ANIMALGLOSS_ANIMAL)
		&&a_type(cr->armor)->type != ARMOR_HEAVYARMOR)uniformed=1;

	switch(type)
	{

	    case SITE_INDUSTRY_WAREHOUSE:
	    case SITE_RESIDENTIAL_SHELTER:
		uniformed=1;
		break;
	    case SITE_LABORATORY_COSMETICS:
	    case SITE_LABORATORY_GENETIC:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_LABCOAT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=(location[cursite]->highsecurity?1:2);
		}
		break;
	    case SITE_GOVERNMENT_POLICESTATION:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_POLICEUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_POLICEARMOR)uniformed=1;
		    if(law[LAW_POLICEBEHAVIOR]==-2 && law[LAW_DEATHPENALTY]==-2 &&
			    a_type(cr->armor)->type == ARMOR_DEATHSQUADUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SWATARMOR)uniformed=(location[cursite]->highsecurity?1:2);
		}
		break;
	    case SITE_GOVERNMENT_WHITE_HOUSE:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_BLACKSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_BLACKDRESS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_CHEAPSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_CHEAPDRESS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVEDRESS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_MILITARY)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_ARMYARMOR)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SEALSUIT)uniformed=1;
		}
		break;
	    case SITE_GOVERNMENT_COURTHOUSE:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_BLACKROBE)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_BLACKSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_BLACKDRESS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_CHEAPSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_CHEAPDRESS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVEDRESS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_POLICEUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_POLICEARMOR)uniformed=1;
		    if(law[LAW_POLICEBEHAVIOR]==-2 && law[LAW_DEATHPENALTY]==-2 &&
			    a_type(cr->armor)->type == ARMOR_DEATHSQUADUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SWATARMOR)uniformed=(location[cursite]->highsecurity?1:2);
		}
		break;
	    case SITE_GOVERNMENT_PRISON:
		if(restricted)
		{
		    uniformed=0;
		    if(law[LAW_POLICEBEHAVIOR]==-2 && law[LAW_DEATHPENALTY]==-2)
		    {
			if(a_type(cr->armor)->type == ARMOR_LABCOAT)uniformed=1;
		    }
		    else if(a_type(cr->armor)->type == ARMOR_PRISONGUARD)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_PRISONER)uniformed=1;
		}
		break;
	    case SITE_GOVERNMENT_ARMYBASE:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_MILITARY)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_ARMYARMOR)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SEALSUIT)uniformed=1;
		}
		break;
	    case SITE_GOVERNMENT_INTELLIGENCEHQ:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_BLACKSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_BLACKDRESS)uniformed=1;
		}
		break;
	    case SITE_GOVERNMENT_FIRESTATION:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_BUNKERGEAR)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_WORKCLOTHES)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_OVERALLS)uniformed=1;
		    if(location[cursite]->highsecurity)
		    {
			if(a_type(cr->armor)->type == ARMOR_POLICEUNIFORM)uniformed=1;
			if(a_type(cr->armor)->type == ARMOR_POLICEARMOR)uniformed=1;
			if(law[LAW_POLICEBEHAVIOR]==-2 && law[LAW_DEATHPENALTY]==-2 &&
				a_type(cr->armor)->type == ARMOR_DEATHSQUADUNIFORM)uniformed=1;
			if(a_type(cr->armor)->type == ARMOR_SWATARMOR)uniformed=1;
		    }
		}
		break;
	    case SITE_BUSINESS_BANK:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_CHEAPSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_POLICEUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_POLICEARMOR)uniformed=1;
		    if(law[LAW_POLICEBEHAVIOR]==-2 && law[LAW_DEATHPENALTY]==-2 &&
			    a_type(cr->armor)->type == ARMOR_DEATHSQUADUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SWATARMOR)uniformed=(location[cursite]->highsecurity?1:2);
		    if(location[cursite]->highsecurity)
		    {
			if(a_type(cr->armor)->type == ARMOR_CIVILIANARMOR)uniformed=1;
		    }
		}
		break;
	    case SITE_BUSINESS_CIGARBAR:
		uniformed=0;
		if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_CHEAPSUIT)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_EXPENSIVEDRESS)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_CHEAPDRESS)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_BLACKSUIT)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_BLACKDRESS)uniformed=1;
		break;
	    case SITE_INDUSTRY_SWEATSHOP:
		uniformed=0;
		if(a_type(cr->armor) == NULL) uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		break;
	    case SITE_INDUSTRY_POLLUTER:
		uniformed=0;
		if(a_type(cr->armor)->type == ARMOR_WORKCLOTHES)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_HARDHAT)uniformed=1;
		if(location[cursite]->highsecurity)
		{
		    if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		}
		break;
	    case SITE_INDUSTRY_NUCLEAR:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_LABCOAT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_CIVILIANARMOR)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_HARDHAT)uniformed=1;
		}
		break;
	    case SITE_CORPORATE_HEADQUARTERS:
		uniformed=0;
		if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_CHEAPSUIT)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_EXPENSIVEDRESS)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_CHEAPDRESS)uniformed=1;
		break;
	    case SITE_CORPORATE_HOUSE:
		uniformed=0;
		if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_EXPENSIVEDRESS)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		if(a_type(cr->armor)->type == ARMOR_SERVANTUNIFORM)uniformed=1;
		if(location[cursite]->highsecurity)
		{
		    if(a_type(cr->armor)->type == ARMOR_MILITARY)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_ARMYARMOR)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_SEALSUIT)uniformed=1;
		}
		break;
	    case SITE_MEDIA_AMRADIO:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_CHEAPSUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVEDRESS)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_CHEAPDRESS)uniformed=1;
		}
		break;
	    case SITE_MEDIA_CABLENEWS:
		if(restricted)
		{
		    uniformed=0;
		    if(a_type(cr->armor)->type == ARMOR_SECURITYUNIFORM)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVESUIT)uniformed=1;
		    if(a_type(cr->armor)->type == ARMOR_EXPENSIVEDRESS)uniformed=1;
		}
		break;
	    case SITE_RESIDENTIAL_TENEMENT:
	    case SITE_RESIDENTIAL_APARTMENT:
	    case SITE_RESIDENTIAL_APARTMENT_UPSCALE:
		if(restricted)uniformed=0;
		break;
	    default:
		break;
	}
    }

    if(!uniformed)
    {
	if(a_type(cr->armor)->type == ARMOR_POLICEUNIFORM)uniformed=2;
	if(a_type(cr->armor)->type == ARMOR_POLICEARMOR)uniformed=2;
	if(law[LAW_POLICEBEHAVIOR]==-2 && law[LAW_DEATHPENALTY]==-2 &&
		a_type(cr->armor)->type == ARMOR_DEATHSQUADUNIFORM)uniformed=2;
	if(location[cursite]->highsecurity &&
		a_type(cr->armor)->type == ARMOR_SWATARMOR)uniformed=2;
	// Loop over adjacent locations to check if fire is anywhere in sight?
	// Or perhaps have a site fire alarm? - Nick
	/*
	   if((curmap->sq[(mcr->y) * MAP_WIDTH + (mcr->x)].type & SITEBLOCK_FIRE_START ||
	   curmap->sq[(mcr->y) * MAP_WIDTH + (mcr->x)].type & SITEBLOCK_FIRE_END ||
	   curmap->sq[(mcr->y) * MAP_WIDTH + (mcr->x)].type & SITEBLOCK_FIRE_PEAK) &&
	   a_type(cr->armor)->type == ARMOR_BUNKERGEAR)uniformed=1;
	 */
    }

    if(uniformed)
    {
	int qlmax = a_type(cr->armor)->quality_levels;
	int ql = armor_get_quality(cr->armor) + (cr->armor->a_flags & AD_DAMAGED);
	if (ql > qlmax)  // Shredded clothes are obvious
	{
	    uniformed = 0;
	}else if ((ql-1) *2 > qlmax) // poor clothes make a poor disguise
	{
	    uniformed++;
	}
	if (uniformed >2)
	    uniformed = 0;
    }

    return uniformed;
}
