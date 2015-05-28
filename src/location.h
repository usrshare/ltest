#ifndef LOCATION_H
#define LOCATION_H
#include "globals.h"

#include <stdbool.h>

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

enum SiteTypes
{
   SITE_CITY_SEATTLE, // first are the cities
   SITE_CITY_LOS_ANGELES,
   SITE_CITY_NEW_YORK,
   SITE_CITY_CHICAGO,
   SITE_CITY_DETROIT,
   SITE_CITY_ATLANTA,
   SITE_CITY_MIAMI,
   SITE_CITY_WASHINGTON_DC,

   SITE_DOWNTOWN, // then are the districts
   SITE_COMMERCIAL,
   SITE_UDISTRICT,
   SITE_OUTOFTOWN,
   SITE_INDUSTRIAL,
   SITE_TRAVEL,

   SITE_HOSPITAL_CLINIC, // then come sites which cannot be mapped
   SITE_HOSPITAL_UNIVERSITY,
   SITE_BUSINESS_PAWNSHOP,
   SITE_BUSINESS_DEPTSTORE,
   SITE_BUSINESS_HALLOWEEN,
   SITE_BUSINESS_ARMSDEALER,
   SITE_BUSINESS_CARDEALERSHIP,

   SITE_RESIDENTIAL_SHELTER, // this has to be in this spot - sites before this cannot be mapped and sites after this can be mapped

   SITE_RESIDENTIAL_TENEMENT, // sites starting at this point CAN be mapped
   SITE_RESIDENTIAL_APARTMENT,
   SITE_RESIDENTIAL_APARTMENT_UPSCALE,
   SITE_RESIDENTIAL_BOMBSHELTER,
   SITE_LABORATORY_COSMETICS,
   SITE_LABORATORY_GENETIC,
   SITE_GOVERNMENT_POLICESTATION,
   SITE_GOVERNMENT_COURTHOUSE,
   SITE_GOVERNMENT_PRISON,
   SITE_GOVERNMENT_INTELLIGENCEHQ,
   SITE_GOVERNMENT_FIRESTATION,
   SITE_INDUSTRY_SWEATSHOP,
   SITE_INDUSTRY_POLLUTER,
   SITE_INDUSTRY_NUCLEAR,
   SITE_INDUSTRY_WAREHOUSE,
   SITE_CORPORATE_HEADQUARTERS,
   SITE_CORPORATE_HOUSE,
   SITE_MEDIA_AMRADIO,
   SITE_MEDIA_CABLENEWS,
   SITE_BUSINESS_CRACKHOUSE,
   SITE_BUSINESS_JUICEBAR,
   SITE_BUSINESS_CIGARBAR,
   SITE_BUSINESS_LATTESTAND,
   SITE_BUSINESS_VEGANCOOP,
   SITE_BUSINESS_INTERNETCAFE,
   SITE_BUSINESS_BARANDGRILL,
   SITE_OUTDOOR_PUBLICPARK,
   SITE_OUTDOOR_BUNKER,
   SITE_GOVERNMENT_ARMYBASE,
   SITE_BUSINESS_BANK,
   SITE_GOVERNMENT_LIBERAL_PARTY_HQ,
   SITE_GOVERNMENT_WHITE_HOUSE,

   SITENUM // this comes last
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

int sitetype;

#endif
