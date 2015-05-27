#ifndef LOCATION_H
#define LOCATION_H

#define LOC_COUNT 128 //arbitrary number, TODO find real amount

struct t_location_type {
};

struct siegest
{
	char siege;
	//Puzz:  Temporary siege thing for organizations
	int orgID;
	short siegetype;
	char underattack;
	int attacktime;
	short kills;
	short tanks;
	short escalationstate;
	char lights_off;
	char cameras_off;
	short timeuntillocated;
	short timeuntilcorps;
	short timeuntilcia;
	short timeuntilccs;
	short timeuntilfiremen;
};

const struct siegest siegest_def = {
	.siege = 0,
	.siegetype = -1,
	.underattack = 0,
	.escalationstate = 0,
	.timeuntillocated = -1,
	.timeuntilcorps = -1,
	.timeuntilcia = -1,
	.timeuntilccs = -1,
	.timeuntilfiremen = -1,
};

struct siteblockst
{
   short special;
   int flag;
   char siegeflag;
};

struct sitechangest
{
   char x,y,z;
   int flag;
};

enum RentingTypes
{
   RENTING_CCS=-2,
   RENTING_NOCONTROL=-1,
   RENTING_PERMANENT=0
   // positive values of renting are considered monthly rent prices
};

#define CITY_NAMELEN 80
#define LOCATION_NAMELEN 40
#define LOCATION_SHORTNAMELEN 20

#define MAX_LOOT 128

struct t_location
{
   
   char name[LOCATION_NAMELEN];
   char shortname[LOCATION_SHORTNAMELEN];
   char type;
   int city;
   int area;
   int parent;
   int id;

   struct t_item* loot[MAX_LOOT];
   //vector<sitechangest> changes;
   int renting;
   char newrental;
   char needcar;
   int closed;
   bool hidden;
   bool mapped;
   bool upgradable;
   int highsecurity;
   struct siegest siege;
   int heat;
   int heat_protection;
   int compound_walls;
   int compound_stores;
   char front_business;
   char front_name[LOCATION_NAMELEN];
   char front_shortname[LOCATION_SHORTNAMELEN];
   bool haveflag;

   unsigned long mapseed[RNG_SIZE];
};

extern struct t_location* location[LOC_COUNT]; 

#endif
