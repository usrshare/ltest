#ifndef ENTITY_H
#define ENTITY_H
#include <stdint.h>
#include <stdbool.h>
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
	EG_NEUTRAL,
	EG_MALE,
	EG_FEMALE,
	EG_COUNT,
	EG_WMPATRIARCH, //generation only
	EG_MBIAS, //generation only
	EG_FBIAS, //generation only
	EG_RANDOM, //generation only
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

#define WOUND_SHOT BIT1
#define WOUND_CUT BIT2
#define WOUND_BRUISED BIT3
#define WOUND_BURNED BIT4
#define WOUND_BLEEDING BIT5
#define WOUND_TORN BIT6
#define WOUND_NASTYOFF BIT7
#define WOUND_CLEANOFF BIT8

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

struct t_entity {

	uint32_t id;

	uint8_t attrvalue[EA_COUNT]; //attributes
	struct t_vmax skillvalue[ES_COUNT]; //skills

	char firstname[32];   // current first name (liberal)
	char o_firstname[32]; // original first name (conservative)
	char middlename[32];  // 
	char lastname[32];    //
	char nickname[32];    // in-squad nickname

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

	uint16_t juice;
};

#endif
