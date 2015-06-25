/*

   Copyright (c) 2002,2003,2004 by Tarn Adams                                            //
//
This file is part of Liberal Crime Squad.                                             //
//
Liberal Crime Squad is free software; you can redistribute it and/or modify     //
it under the terms of the GNU General Public License as published by            //
the Free Software Foundation; either version 2 of the License, or               //
(at your option) any later version.                                             //
//
Liberal Crime Squad is distributed in the hope that it will be useful,          //
but WITHOUT ANY WARRANTY; without even the implied warranty of                  //
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the                  //
GNU General Public License for more details.                                    //
//
You should have received a copy of the GNU General Public License               //
along with Liberal Crime Squad; if not, write to the Free Software              //
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   02111-1307   USA     //
 */

/*
   This file was created by Chris Johnson (grundee@users.sourceforge.net)
   by copying code from game.cpp.
   To see descriptions of files and functions, see the list at
   the bottom of includes.h in the top src folder.
 */

/* checks if your liberal activity is noticed */

#include "globals.h"

#include "entity_types.h"
#include "weapon.h"
#include "armor.h"

#include "location.h"
#include "mapdefs.h"
#include "ui.h"

#include "stealth.h"

void noticecheck(struct t_creature* p, struct t_creature* e, int exclude,int difficulty)
{
	// p = player, e = enemy.
	if(sitealarm) return;

	//Prisoners shouldn't shout for help.
	if (e->type == ET_PRISONER) return;

	if (entity_skill_check(p,ES_STEALTH,difficulty)) return;

	g_attrset(CP_RED);

	g_printw("%s observes your Liberal activity",describe_entity_static(e));
	if (e->align == ALIGN_CONSERVATIVE)
		g_printw("and lets forth a piercing Conservative alarm cry!");
	else g_printw("and shouts for help!", gamelog);

	sitealarm = 1;
}

/* checks if your liberal behavior/attack alienates anyone */
/*
   char alienationcheck(char mistake)
   {
   if(location[cursite]->siege.siege)return 0;

   char alienate=0,alienatebig=0;

   int oldsitealienate=sitealienate;

   vector<int> noticer;
   for(int e=0;e<ENCMAX;e++)
   {
// Prisoners should never be alienated by your crimes, as
// they're happy to have you attacking their place of holding
//if(encounter[e].type==CREATURE_PRISONER)continue;

// ...but Prisoners are now spawned with a variety of creature
// types, so we'll go by name instead
if(!strcmp(encounter[e].name,"Prisoner")) continue;

if(encounter[e].exists&&encounter[e].alive&&(encounter[e].align==0||(encounter[e].align==1&&mistake)))
noticer.push_back(e);
}

if(len(noticer))
{
int n,an;

do
{
an=LCSrandom(len(noticer));
n=noticer[an];
noticer.erase(noticer.begin() + an);

if(e.align==1) alienatebig=1;
else alienate=1;
} while(len(noticer));

if(alienatebig) sitealienate=2;
if(alienate&&sitealienate!=2) sitealienate=1;

if(oldsitealienate<sitealienate)
{
set_color(COLOR_YELLOW,COLOR_BLACK,1);

move(16,1);
if(sitealienate==1)addstr("We've alienated the masses here!              ", gamelog);
else addstr("We've alienated absolutely everyone here!               ", gamelog);
gamelog.newline();
move(17,1);
addstr("                                                        ");

sitealarm=1;

for(int i=0;i<ENCMAX;i++)
if(encounter[i].exists && encounter[i].align != ALIGN_CONSERVATIVE)
if(encounter[i].align == ALIGN_MODERATE || alienatebig)
conservatise(encounter[i]);

if(mode==GAMEMODE_CHASECAR||
mode==GAMEMODE_CHASEFOOT) printchaseencounter();
else printencounter();

getkey();
}
}

return alienate;
}
 */

/* checks if conservatives see through your disguise */
void disguisecheck(struct t_creature* p, struct t_creature* e, int timer, bool restricted_space)
{
	static const char *blew_stealth_check[] =
	{
		" coughs.",
		" accidentally mumbles the slogan.",
		" paces uneasily.",
		" stares at the Conservatives.",
		" laughs nervously."
	};

	int weapon=0,blew_it=-1;
	bool forcecheck=false,spotted=false;
	//int weaponar[6]={0};

	// Only start to penalize the player's disguise/stealth checks after the first turn.
	timer--;

	weapon=weaponcheck(p,0);

	// Nothing suspicious going on here
	if(sitealarmtimer==-1 && weapon<1 && !forcecheck)
	{
		if(!restricted_space) return;
	}

	char noticed=0;

	if(e->type==ET_PRISONER) return;

	if(e->exists&&e->alive&&enemy(e)) noticed = 1;

	if(!noticed) return;

	int n,an;

	int stealth_difficulty;
	int disguise_difficulty;

	char msg[256]; msg[0] = 0;

	// Determine difficulty based on enemy type
	switch(e->type)
	{
		default:
			stealth_difficulty = DIFFICULTY_VERYEASY;
			disguise_difficulty = DIFFICULTY_VERYEASY;
			break;
		case ET_SWAT:
		case ET_COP:
		case ET_GANGUNIT:
		case ET_DEATHSQUAD:
			stealth_difficulty = DIFFICULTY_EASY;
			disguise_difficulty = DIFFICULTY_EASY;
			break;
		case ET_PRISONGUARD:
		case ET_BOUNCER:
		case ET_SECURITYGUARD:
			stealth_difficulty = DIFFICULTY_AVERAGE;
			disguise_difficulty = DIFFICULTY_EASY;
			break;
		case ET_AGENT:
			stealth_difficulty = DIFFICULTY_AVERAGE;
			disguise_difficulty = DIFFICULTY_AVERAGE;
			break;
		case ET_NEWSANCHOR:
		case ET_RADIOPERSONALITY:
		case ET_CORPORATE_CEO:
		case ET_JUDGE_CONSERVATIVE:
		case ET_CCS_ARCHCONSERVATIVE:
		case ET_SCIENTIST_EMINENT:
			stealth_difficulty = DIFFICULTY_EASY;
			disguise_difficulty = DIFFICULTY_HARD;
			break;
		case ET_GUARDDOG:
			stealth_difficulty = DIFFICULTY_HEROIC;
			disguise_difficulty = DIFFICULTY_AVERAGE;
			break;
		case ET_SECRET_SERVICE:
			stealth_difficulty = DIFFICULTY_FORMIDABLE;
			disguise_difficulty = DIFFICULTY_FORMIDABLE;
			break;
	}

	// Increase difficulty if Conservatives suspicious...
	if(sitealarmtimer==1)
	{
		stealth_difficulty += 6;
		disguise_difficulty += 6;
	}
	else if(sitealarmtimer>1)
	{
		stealth_difficulty += 3;
		disguise_difficulty += 3;
	}

	// Try to sneak.
	if(!spotted)
	{
		int result = entity_skill_roll(p,ES_STEALTH);
		result -= timer;
		if (fieldskillrate == FIELDSKILLRATE_HARD && result + 1 == stealth_difficulty)
		{// Hard more = You only learn if you just missed, and realize what you did wrong.
			entity_train(p,ES_STEALTH, 10);
		}
		if(result < stealth_difficulty)
			spotted = true;
	}

	// Spotted! Act casual.
	if(spotted)
	{
		// Scary weapons are not very casual.
		if(weaponcheck(p,0) == 2)
		{
			noticed = true;
		} else {
			int result = entity_skill_roll(p,ES_DISGUISE);
			result -= timer;
			if (fieldskillrate == FIELDSKILLRATE_HARD && result + 1 == disguise_difficulty)
			{// Hard more = You only learn if you just missed, and realize what you did wrong.
				entity_train(p,ES_DISGUISE, 10);
			}
			if(result<disguise_difficulty)
			{
				// That was not very casual, dude.
				if(result<0)blew_it=1;

				noticed = true;
			}
		}
	}

	// Give feedback on the Liberal Performance
	if(!spotted)
	{
		for(int i=0;i<6;i++)
		{
			if(p == NULL) break;
			switch (fieldskillrate)
			{
				case FIELDSKILLRATE_FAST:
					entity_train(p,ES_STEALTH, 40);break;
				case FIELDSKILLRATE_CLASSIC:
					entity_train(p,ES_STEALTH, 10);break;
				case FIELDSKILLRATE_HARD:
					entity_train(p,ES_STEALTH, 0);break;
			}
		}

		if(timer == 0)
		{
			g_attrset(CP_CYAN);
			g_printw("%s fades into the shadows.",describe_entity_static(p));
		}
	}
	else
	{
		if(blew_it == -1)
		{
			int i;
			for(i=0;i<6;i++)
			{
				if(p == NULL) break;
				if(hasdisguise(*(p)))
				{
					switch (fieldskillrate)
					{
						case FIELDSKILLRATE_FAST:
							entity_train(p,ES_DISGUISE, 50);break;
						case FIELDSKILLRATE_CLASSIC:
							entity_train(p,ES_DISGUISE, 10);break;
						case FIELDSKILLRATE_HARD:
							entity_train(p,ES_DISGUISE, 0);break;
					}
				}
			}
		}

		if(blew_it != -1 && randval(2))
		{
			g_attrset(CP_YELLOW);

			g_printw("%s%s",describe_entity_static(p),pickrandom(blew_stealth_check));

		}
		else if(!noticed)
		{
			g_attrset(CP_CYAN);

			g_printw("%s acts natural.",describe_entity_static(p));
		}
	}

	if(!noticed)return;

	g_attrset(CP_RED);
	if(sitealarmtimer!=0 && weapon<1 && e->type!=ET_GUARDDOG)
	{
		if((sitetype==SITE_RESIDENTIAL_TENEMENT||
					sitetype==SITE_RESIDENTIAL_APARTMENT||
					sitetype==SITE_RESIDENTIAL_APARTMENT_UPSCALE)&&
				restricted_space)
		{
			sitealarm=1;

			g_printw("%s shouts in alarm at the squad's Liberal Trespassing!", describe_entity_static(e));
		}
		else
		{
			g_printw("%s looks at %s suspiciously.", describe_entity_static(e), describe_entity_static(p));

			int time;

			time = 20 + randval(10) - entity_get_attribute(e,EA_INT,true)
				- entity_get_attribute(e,EA_WIS,true);

			if(time<1)time=1;

			if(sitealarmtimer>time||sitealarmtimer==-1)sitealarmtimer=time;
			else
			{
				if(sitealarmtimer>5) sitealarmtimer-= 5;
				if(sitealarmtimer<=5)sitealarmtimer = 0;
			}
		}
	}
	else
	{
		if(weapon&&e->type!=ET_GUARDDOG)
		{
			g_printw("%s sees %s's Liberal Weapons", describe_entity_static(e),describe_entity_static(p));
			
			if(e->align==ALIGN_CONSERVATIVE)
				g_addstr("and lets forth a piercing Conservative alarm cry!", gamelog);
			else
				g_addstr("and shouts for help!", gamelog);
		}
		else
		{
			g_printw("%s looks at %s with Intolerance", describe_entity_static(e),describe_entity_static(p));
			if(e->align==ALIGN_CONSERVATIVE)
			{
				if(e->type==ET_GUARDDOG)
					g_addstr("and launches into angry Conservative barking!", gamelog);
				else
					g_addstr("and lets forth a piercing Conservative alarm cry!", gamelog);

			}
			else
				g_addstr("and shouts for help!", gamelog);
		}
		sitealarm=1;
	}
}

char weapon_in_character(enum weapon_types wtype, enum armortype atype)
{
	if(atype == ARMOR_LABCOAT && wtype == WT_SYRINGE)
		return ET_SCIENTIST_LABTECH;

	if(atype == ARMOR_BLACKROBE && wtype == WT_GAVEL)
		return ET_JUDGE_LIBERAL;

	if(atype == ARMOR_SECURITYUNIFORM && (wtype == WT_REVOLVER_38 ||
				wtype == WT_REVOLVER_44    || wtype == WT_DESERT_EAGLE ||
				wtype == WT_SEMIPISTOL_9MM || wtype == WT_SEMIPISTOL_45 ||
				wtype == WT_NIGHTSTICK     || wtype == WT_SMG_MP5))
		return ET_SECURITYGUARD;

	if((atype == ARMOR_POLICEUNIFORM   || atype == ARMOR_POLICEARMOR) &&
			(wtype == WT_REVOLVER_38   || wtype == WT_REVOLVER_44 ||
			 wtype == WT_DESERT_EAGLE   || wtype == WT_SEMIPISTOL_9MM ||
			 wtype == WT_SEMIPISTOL_45  || wtype == WT_NIGHTSTICK ||
			 wtype == WT_SHOTGUN_PUMP))
		return ET_COP;

	if(wtype == WT_SMG_MP5 || wtype == WT_CARBINE_M4)
	{
		if(atype == ARMOR_SWATARMOR)
			return ET_SWAT;
		else if(atype == ARMOR_SECURITYUNIFORM && law[LAW_GUNCONTROL]==-2)
			return ET_SECURITYGUARD;
		else if(atype == ARMOR_MILITARY || atype == ARMOR_ARMYARMOR)
			return ET_SOLDIER;
	}

	if(wtype == WT_AUTORIFLE_M16 || wtype == WT_SEMIRIFLE_AR15)
	{
		// Military
		if(atype == ARMOR_MILITARY || atype == ARMOR_ARMYARMOR)
			return ET_SOLDIER;
		if(atype == ARMOR_DEATHSQUADUNIFORM)
			return ET_DEATHSQUAD;
	}

	if(wtype == WT_AXE && atype == ARMOR_BUNKERGEAR)
		return ET_FIREFIGHTER;

	if(wtype == WT_FLAMETHROWER && atype == ARMOR_BUNKERGEAR && law[LAW_FREESPEECH]==-2)
		return ET_FIREFIGHTER;

	if(atype == ARMOR_PRISONGUARD && (wtype == WT_SMG_MP5 ||
				wtype == WT_SHOTGUN_PUMP || wtype == WT_NIGHTSTICK))
		return ET_PRISONGUARD;

	if((atype == ARMOR_OVERALLS || atype == ARMOR_WIFEBEATER) &&
			(wtype == WT_TORCH || wtype == WT_PITCHFORK ||
			 (law[LAW_GUNCONTROL]==-2 && wtype == WT_SHOTGUN_PUMP)))
		return ET_HICK;

	if(wtype == WT_SHANK && atype == ARMOR_PRISONER)
		return ET_PRISONER;

	if(wtype == WT_CHAIN &&
			(atype == ARMOR_WORKCLOTHES || atype == ARMOR_HARDHAT) )
		return ET_WORKER_FACTORY_UNION;

	if(wtype == WT_CARBINE_M4 && atype == ARMOR_SEALSUIT)
		return ET_SEAL;

	return -1;
}

/* checks if a creature's weapon is suspicious */
char weaponcheck(struct t_creature* cr, bool metaldetect) {
	bool suspicious = w_type(cr->weapon)->is_suspicious;
	bool concealed = a_type(cr->armor)->conceal_weapon_size >= w_type(cr->weapon)->size;
	char incharacter = weapon_in_character(w_type_id(cr->weapon), a_type_id(cr->armor));
	//bool illegal = cr.get_weapon().get_legality() < law[LAW_GUNCONTROL];

	// If your disguise is inappropriate to the current location,
	// then being in character isn't sufficient
	if(hasdisguise(cr) == false)
		incharacter = -1;

	if(suspicious)
	{
		if(concealed && !metaldetect)
			return 0; // Hidden weapon, nothing to see
		else if(incharacter >= 0)
			return 1; // You look like you're supposed to have that weapon
		else
			return 2; // Looks like trouble
	}
	return 0; // Nothing to see here
}


/* returns true if the entire site is not open to public */
char disguisesite(long type)
{
	switch(type)
	{
		case SITE_LABORATORY_COSMETICS:
		case SITE_LABORATORY_GENETIC:
		case SITE_GOVERNMENT_PRISON:
		case SITE_GOVERNMENT_INTELLIGENCEHQ:
		case SITE_INDUSTRY_SWEATSHOP:
		case SITE_INDUSTRY_POLLUTER:
		case SITE_CORPORATE_HEADQUARTERS:
		case SITE_CORPORATE_HOUSE:
		case SITE_BUSINESS_CIGARBAR:
			return 1;
	}

	return 0;
}
