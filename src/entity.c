// vim: cin:sts=4:sw=4 
#include "entity.h"
#include "entity_name.h"
#include "entity_types.h"

#include <malloc.h>
#include <string.h>
#include "globals.h"
#include "random.h"

uint32_t curcreatureid = 0;

/*
// Copyright (c) 2002,2003,2004 by Tarn Adams
// Copyright (c) 2015 usr_share
//
// This file is part of Liberal Crime Squad.
//
// Liberal Crime Squad is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Liberal Crime Squad is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
// GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License
// along with Liberal Crime Squad; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   02111-1307   USA
 */

/*
   This file was created by Chris Johnson (grundee@users.sourceforge.net)
   by copying code from game.cpp.
   To see descriptions of files and functions, see the list at
   the bottom of includes.h in the top src folder.
 */

// this file based on creature/creature.cpp from Liberal Crime Squad

bool vrange(struct t_range* range) {
    if ((range->min == 0) && (range->max == 0)) return false;
    if (range->min > range->max) return false;
    return true;
}

int creature_init(struct t_entity* o_entity, struct t_entity_generate_rules* genrules) {

    memset(o_entity,0,sizeof(struct t_entity));

    if ((genrules == NULL) || (genrules->gender = EG_RANDOM) || !vrange(&genrules->age)) {
	random_gender_and_age(&o_entity->age, &o_entity->gender_id); }
		else {
		if ((genrules) || vrange(&genrules->age)) {
		o_entity->age = randrange(&genrules->age); } else o_entity->age = 18 + randval(40);


		enum entity_gender g_gender = genrules ? genrules->gender : EG_RANDOM;

		switch (g_gender) {
		case EG_MALE: o_entity->gender_id = EG_MALE; break;
		case EG_FEMALE: o_entity->gender_id = EG_FEMALE; break;
		case EG_WMPATRIARCH: o_entity->gender_id = EG_WMPATRIARCH; break; //will be replaced with male after the name is generated.
		case EG_NEUTRAL:
				     o_entity->gender_id = EG_NEUTRAL; o_entity->gender_bio = randbetween(EG_MALE,EG_FEMALE); break;
		case EG_MBIAS:
		o_entity->gender_id = (randval(100) < 75) ? EG_MALE : EG_FEMALE; break;
		case EG_FBIAS:
		o_entity->gender_id = (randval(100) < 75) ? EG_MALE : EG_FEMALE; break;
		case EG_RANDOM:
		default:
		o_entity->gender_id = (randval(100) < 52) ? EG_FEMALE : EG_MALE; break; //52% females according to U.S. Census data
		}
		}

    if ((randval(100) < 1) && (o_entity->gender_bio == EG_RANDOM))
	o_entity->gender_bio = randbetween(EG_NEUTRAL,EG_FEMALE); else o_entity->gender_bio = o_entity->gender_id; //will result in mismatches for 0.67% of cases.

    o_entity->birthday_month = randval(12) + 1;

    switch(o_entity->birthday_month)
    {
	case 4:
	case 6:
	case 9:
	case 11:
	    o_entity->birthday_day=randval(30)+1;
	    break;
	case 2:
	    o_entity->birthday_day=randval(28)+1;
	    break;
	default:
	    o_entity->birthday_day=randval(31)+1;
	    break;
    }

    o_entity->id=curcreatureid++;

    for(int w=0;w<EB_COUNT;w++)o_entity->wound[w]=0;

    for(int a=0;a<EA_COUNT;a++)o_entity->attributes[a]=1;
    
    int attnum=32;
    while(attnum>0)
    {
	int a=randval(EA_COUNT);
	if(o_entity->attributes[a]<10)
	{
	    o_entity->attributes[a]++;
	    attnum--;
	}
    }

    if (genrules) {

	for (int i=0; i < RANDATTRS; i++)
	    if (vrange(&genrules->attrlim[i])) o_entity->attributes[genrules->attrs[i]] = randrange(&genrules->attrlim[i]);
	for (int i=0; i < RANDSKILLS; i++)
	    if (vrange(&genrules->skilllim[i])) o_entity->skills[genrules->skill[i]] = randrange(&genrules->skilllim[i]);

	enum Alignment align = genrules->align;

	if (genrules->random_align) align += randbetween(-2,2);

	if (align < ALIGN_ARCHCONSERVATIVE) align = ALIGN_ARCHCONSERVATIVE;
	if (align > ALIGN_ELITELIBERAL) align = ALIGN_ELITELIBERAL;

	o_entity->align = align;

	if (vrange(&genrules->juice)) o_entity->juice = randrange(&genrules->juice);
	if (vrange(&genrules->money)) o_entity->money = randrange(&genrules->money);
    }

    //at this point, both the entity's alignment and gender are fixed.
    //let's generate a name.

    random_first_name(o_entity->firstname,o_entity->gender_id);
    random_last_name(o_entity->lastname,o_entity->align == ALIGN_ARCHCONSERVATIVE,o_entity->gender_id);

    o_entity->special[ESW_TEETH]=TOOTHNUM;
    o_entity->special[ESW_RIGHTEYE]=1;
    o_entity->special[ESW_LEFTEYE]=1;
    o_entity->special[ESW_NOSE]=1;
    o_entity->special[ESW_TONGUE]=1;
    o_entity->special[ESW_RIGHTLUNG]=1;
    o_entity->special[ESW_LEFTLUNG]=1;
    o_entity->special[ESW_HEART]=1;
    o_entity->special[ESW_LIVER]=1;
    o_entity->special[ESW_STOMACH]=1;
    o_entity->special[ESW_RIGHTKIDNEY]=1;
    o_entity->special[ESW_LEFTKIDNEY]=1;
    o_entity->special[ESW_SPLEEN]=1;
    o_entity->special[ESW_RIBS]=RIBNUM;
    o_entity->special[ESW_NECK]=1;
    o_entity->special[ESW_UPPERSPINE]=1;
    o_entity->special[ESW_LOWERSPINE]=1;

    o_entity->blood = 100;

    return 0;
}

int entity_name(struct t_entity* who) {
    random_first_name(who->firstname,who->gender_id);
    random_last_name(who->lastname,who->align == ALIGN_ARCHCONSERVATIVE,who->gender_id);
    return 0;
}

char* entityattrstr[EA_COUNT] = {
    "STR",
    "INT",
    "WIS",
    "AGI",
    "CON",
    "CHA",
    "HRT"
};

const char* safe_name(const char* nameptr) {
    if ((nameptr == NULL) || (strlen(nameptr) == 0)) return "???";
    return nameptr;
}

int describe_entity(struct t_entity* me, char* const restrict o_name, size_t strsize) {
    if (me != NULL) {
	if (strlen(me->nickname) != 0) {
	    strncpy(o_name,me->nickname,strsize);
	    return 0;}

	if (((strlen(me->firstname) != 0) || (strlen(me->firstname) != 0)) && (me->name_known)) {
	    char fullname[66];
	    strncpy(fullname,safe_name(me->firstname),32);
	    strncat(fullname," ",1);
	    strncat(fullname,safe_name(me->lastname),32);
	    strncpy(o_name,fullname,strsize);
	    return 0; }

	const char* td = type_description(me);

	if (td) { strncpy(o_name,type_description(me),strsize); return 0; }

    }
    strncpy(o_name, "*",32);
    return 0;
}

// This table is based on 2010 age/gender distribution data from the U.S. Census Bureau
// (http://www.census.gov/prod/cen2010/briefs/c2010br-03.pdf)
// each row's numbers are a sum of this and all previous rows' values from Table 2.

uint32_t us_2010_data[21][2] = {
    // male   //  female
    { 10319427,  9881935}, // 0 0~4
    { 20709065, 19840954}, // 1 5~9
    { 31288927, 29938286}, // 2 10~14
    { 42592593, 40674963}, // 3 15~19
    { 53606769, 51246786}, // 4 20~24
    { 64242360, 61713044}, // 5 25~29
    { 74238860, 71678643}, // 6 30~34
    { 84280882, 81816263}, // 7 35~39
    { 94674859, 92313250}, // 8 40~44
    {105883944,103812756}, // 9 45~49
    {116817218,115177607}, //10 50~54
    {126340866,125318764}, //11 55~59
    {134418366,134059188}, //12 60~64
    {140270913,140641904}, //13 65~69
    {144514885,145676098}, //14 70~74
    {147697273,149811505}, //15 75~79
    {149991647,153260458}, //16 80~84
    {151265514,155607050}, //17 85~89
    {151689901,156631029}, //18 90~94
    {151772164,156920010}, //19 95~99
    {151781326,156964212}, //20 100+ / total
};

int random_gender_and_age(int* o_age, enum entity_gender* o_gender) {

    enum entity_gender eg = ((uint32_t)randval(us_2010_data[20][0] + us_2010_data[20][1]) > us_2010_data[20][0]) ? EG_FEMALE : EG_MALE;

    int column = (eg == EG_FEMALE) ? 1 : 0;

    int age = randval(5);

    uint32_t randpop = randval(us_2010_data[20][column]);

    for (int i=0; i < 20; i++) {
	if (randpop > us_2010_data[i][column]) age += 5;
    }

    *o_age = age;
    *o_gender = eg;

    return 0;
}

int entity_get_attribute(struct t_entity* me, enum entity_attr attribute, bool usejuice) {

    int age = me->age;

    int ret=me->attributes[attribute];
    // Special modifications to attributes based on age
    switch(attribute)
    {
	case EA_STR:
	    if(age<11)ret>>=1;     // Strength is lowest at the beginning and end of life
	    else if(age<16)ret-=1;
	    else if(age>70)ret-=6;
	    else if(age>52)ret-=3;
	    else if(age>35)ret-=1;
	    break;
	case EA_AGI:
	    if(age>70)ret-=6;      // Agility is weakened with age
	    else if(age>52)ret-=3;
	    else if(age>35)ret-=1;
	    break;
	case EA_CON:
	    if(age<11)ret-=2;
	    else if(age<16)ret-=1; // Physical immaturity weakens health
	    // Aging actually damages base health and eventually kills, so no aging effects here
	    break;
	case EA_CHA:
	    if(age<11)ret+=2;      // Lots of folks like kids
	    else if(age<16)ret-=1; // Teenagers have communication difficulties and image issues
	    else if(age>70)ret+=3; // Authority and experience in life then enhance Charisma with age
	    else if(age>52)ret+=2;
	    else if(age>35)ret+=1;
	    break;
	case EA_INT:
	    if(age<11)ret-=3;      // Experience enhances Intelligence with age
	    else if(age<16)ret-=1;
	    else if(age>70)ret+=3;
	    else if(age>52)ret+=2;
	    else if(age>35)ret+=1;
	    break;
	case EA_WIS:
	    if(age<11)ret-=2;      // Experience grants Wisdom with age
	    else if(age<16)ret-=1;
	    else if(age>70)ret+=2;
	    else if(age>52)ret+=1;
	    break;
	case EA_HRT:
	    if(age<11)ret+=2;      // Experience saps Heart with age due to cynicism
	    else if(age<16)ret+=1; // No wonder it's typically the young who are most Liberal...
	    else if(age>70)ret-=2;
	    else if(age>52)ret-=1;
	    break;
    }

    // Physical stats want to know: Are you paralyzed?
    if(attribute==EA_STR||
	    attribute==EA_AGI||
	    attribute==EA_CON)
    {
	if(me->special[ESW_NECK]!=1||
		me->special[ESW_UPPERSPINE]!=1)
	    ret=1;
	else if(me->special[ESW_LOWERSPINE]!=1)ret>>=2;
    }

    // Agility wants to know: Do you have legs?
    if(attribute==EA_AGI)
    {
	int legok=2;
	if((me->wound[EB_LEG_RIGHT] & WOUND_NASTYOFF)||
		(me->wound[EB_LEG_RIGHT] & WOUND_CLEANOFF))legok--;
	if((me->wound[EB_LEG_LEFT] & WOUND_NASTYOFF)||
		(me->wound[EB_LEG_LEFT] & WOUND_CLEANOFF))legok--;

	if(legok==0)ret>>=2;
	else if(legok==1)ret>>=1;
    }

    // Charisma wants to know: How fucked up does your face look?
    if(attribute==EA_CHA)
    {
	long disfigs=0;
	if(me->special[ESW_TEETH]<TOOTHNUM)disfigs++;
	if(me->special[ESW_TEETH]<TOOTHNUM/2)disfigs++;
	if(me->special[ESW_TEETH]==0)disfigs++;
	if(me->special[ESW_RIGHTEYE]==0)disfigs+=2;
	if(me->special[ESW_LEFTEYE]==0)disfigs+=2;
	if(me->special[ESW_TONGUE]==0)disfigs+=3;
	if(me->special[ESW_NOSE]==0)disfigs+=3;

	ret-=disfigs;
    }

    // Finish now if not using juice to avoid bounds check.
    if(!usejuice)return ret;

    // Never use juice to increase stats for the opposite ideology!
    if(attribute==EA_WIS && me->align!=ALIGN_CONSERVATIVE)usejuice=false;
    if(attribute==EA_HRT && me->align!=ALIGN_LIBERAL)usejuice=false;

    // Effects of juice on the character's attributes
    if(usejuice)
    {
	if(me->juice<=-50)ret=1; // Damn worthless
	else if(me->juice<=-10)ret=(ret*6/10); // Society's dregs
	else if(me->juice<0)ret=(ret*8/10);    // Punk
	else if(me->juice>=10)
	{
	    if(me->juice<50)ret+=1; // Activist
	    else if(me->juice<100)ret=(ret*11/10+2); // Socialist Threat
	    else if(me->juice<200)ret=(ret*12/10+3); // Revolutionary
	    else if(me->juice<500)ret=(ret*13/10+4); // Urban Guerrilla
	    else if(me->juice<1000)ret=(ret*14/10+5); // Liberal Guardian
	    else ret=(ret*15/10+6); // Elite Liberal
	}

	// Debilitations for temporary injuries in attributes based
	// on physical appearance or performance, because people who
	// are bleeding all over are less strong, agile, and charismatic
	// it is also hard to think when you are in severe pain
	if(attribute==EA_STR||
		attribute==EA_AGI||
		attribute==EA_CHA||
		attribute==EA_INT)
	{
	    //         if(blood<=20)ret>>=2;
	    //         else if(blood<=50){ret>>=1;}
	    //         else if(blood<=75){ret*=3;ret>>=2;}
	    ret = (int)((0.5 + (float)ret)*(float)(me->blood)/100.0);
	}
    }

    // Bounds check attributes
    if(ret<1) ret=1;
    if(ret>EA_COUNT) ret=EA_COUNT;

    return ret;
}
