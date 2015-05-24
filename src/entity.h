// vim: cin:sts=4:sw=4 
#ifndef ENTITY_H
#define ENTITY_H
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "random.h"

/*
   Copyright (c) 2002,2003,2004 by Tarn Adams                                         //
   Copyright (c) 2015 usr_share
//
This file is part of Liberal Crime Squad.                                           //
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

// this file based on creature/creature.h from Liberal Crime Squad

enum entity_gender {
	EG_RANDOM = 0, //generation only
	EG_NEUTRAL,
	EG_MALE,
	EG_FEMALE,
	EG_COUNT, //not a real value
	EG_WMPATRIARCH, //generation and naming only
	EG_MBIAS, //generation only
	EG_FBIAS, //generation only
};

enum entity_bodyparts
{
	EB_HEAD,
	EB_BODY,
	EB_ARM_RIGHT,
	EB_ARM_LEFT,
	EB_LEG_RIGHT,
	EB_LEG_LEFT,
	EB_COUNT
};

enum entity_specialwounds
{
	ESW_TEETH,
	ESW_RIGHTEYE,
	ESW_LEFTEYE,
	ESW_NOSE,
	ESW_TONGUE,
	ESW_RIGHTLUNG,
	ESW_LEFTLUNG,
	ESW_HEART,
	ESW_LIVER,
	ESW_STOMACH,
	ESW_RIGHTKIDNEY,
	ESW_LEFTKIDNEY,
	ESW_SPLEEN,
	ESW_RIBS,
	ESW_NECK,
	ESW_UPPERSPINE,
	ESW_LOWERSPINE,
	ESW_COUNT
};

#define RIBNUM 10
#define TOOTHNUM 32

enum entity_wounds {
	WOUND_SHOT=1,
	WOUND_CUT=2,
	WOUND_BRUISED=4,
	WOUND_BURNED=8,
	WOUND_BLEEDING=16,
	WOUND_TORN=32,
	WOUND_NASTYOFF=64,
	WOUND_CLEANOFF=128,	
};


enum entity_attr {
	EA_STR,
	EA_INT,
	EA_WIS,
	EA_AGI,
	EA_CON, //health
	EA_CHA,
	EA_HRT, //heart
	EA_COUNT, 
};

struct t_vmax {
	uint16_t val;
	uint16_t max;
};

enum entity_skill {
	ES_ART = 0,
	ES_AXE,
	ES_BUSINESS,
	ES_CLUB,
	ES_COMPUTERS,
	ES_DISGUISE,
	ES_DODGE,
	ES_DRIVING,
	ES_FIRSTAID,
	ES_HEAVYWEAPONS,
	ES_KNIFE,
	ES_LAW,
	ES_HANDTOHAND, // actually this is called "Martial Arts"
	ES_MUSIC,
	ES_PERSUASION,
	ES_PISTOL,
	ES_PSYCHOLOGY,
	ES_RELIGION,
	ES_RIFLE,
	ES_SCIENCE,
	ES_SECURITY,
	ES_SEDUCTION,
	ES_SHOTGUN,
	ES_SMG,
	ES_STEALTH,
	ES_STREETSENSE,
	ES_SWORD,
	ES_TAILORING,
	ES_TEACHING,
	ES_THROWING,
	ES_WRITING,
	ES_COUNT
};

extern char* entitytypesstr[];

enum Alignment
{
   ALIGN_ARCHCONSERVATIVE = -2,
   ALIGN_CONSERVATIVE,
   ALIGN_MODERATE,
   ALIGN_LIBERAL,
   ALIGN_ELITELIBERAL,
   ALIGN_STALINIST
};

struct t_entity {

	uint32_t id;

	uint8_t attributes[EA_COUNT]; //attributes
	uint16_t skills[ES_COUNT]; //skills

	char firstname[32];   // current first name (liberal)
	char o_firstname[32]; // original first name (conservative)
	char middlename[32];  // 
	char lastname[32];    //
	char nickname[32];    // in-squad nickname
	bool name_known;      // does the squad know this entity's name?

	enum entity_gender gender_bio; //biological gender (conservative)
	enum entity_gender gender_id; // gender identity (liberal)

	int squadid;

	int age;
	int birthday_month;
	int birthday_day;

	bool exists;
	char align;
	bool alive;
	short type;

	unsigned char wound[EB_COUNT];
	char special[ESW_COUNT];
   	short blood;

	short weapon;
	short armor;

	int16_t juice;
	int32_t money;
};

#define RANDATTRS 5
#define RANDSKILLS 10
#define RANDWEAPONS 7
#define RANDARMORS 3

struct t_entity_generate_rules {

	// this is a list of rules/limits the entity generator will have to obey.
	// its functionality roughly corresponds with the rules given in
	// Liberal Crime Squad's art/creatures.xml file.
	short type;
	enum entity_gender gender;
	bool generate_name;
	struct t_range age;
	bool random_align; //TODO: if on, alignment will vary +/-2 from one specified
			   //in "align", saturated at +2/-2.
			   //so, if align = 2, random will not create conservatives,
			   //and if -2, will not create liberals.
	enum Alignment align;
	enum entity_attr attrs[RANDATTRS];
	struct t_range attrlim[RANDATTRS];
	enum entity_skill skill[RANDSKILLS];
	struct t_range skilllim[RANDSKILLS];
	struct t_range infiltration;
	struct t_range juice;
	struct t_range money;
	
	short weapons[RANDWEAPONS];
	struct t_range ammo[RANDWEAPONS];
	short armors[RANDARMORS];

};

int random_gender_and_age(int* o_age, enum entity_gender* o_gender);

//Creates a creature
int creature_init(struct t_entity* o_entity, struct t_entity_generate_rules* genrules);

//Returns either a name, or ???
const char* safe_name(const char* nameptr);

//Writes a string that describes the entity (name / profession) into the memory stored
int describe_entity(struct t_entity* me, char* const restrict o_name, size_t strsize);

//Gets the value of an attribute, according to all the changes caused by age, juice and health.
int entity_get_attribute(struct t_entity* me, enum entity_attr attribute, bool usejuice);

#endif
