#include "entity.h"
#include "entity_name.h"

#include <malloc.h>
#include <string.h>
#include "globals.h"
#include "random.h"

uint32_t curcreatureid = 0;

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

// this file based on creature/creature.cpp from Liberal Crime Squad

struct t_entity* creature_init() {

	struct t_entity* newent = malloc(sizeof(struct t_entity));

	newent->age = 18 + randval(40);
	newent->gender_bio = randbetween(EG_MALE,EG_FEMALE);
	if (randval(100) < 1) 
		newent->gender_id = randbetween(EG_NEUTRAL,EG_FEMALE); else newent->gender_id = newent->gender_bio;

	newent->birthday_month = randval(12) + 1;

	switch(newent->birthday_month)
	{
		case 4:
		case 6:
		case 9:
		case 11:
			newent->birthday_day=randval(30)+1;
			break;
		case 2:
			newent->birthday_day=randval(28)+1;
			break;
		default:
			newent->birthday_day=randval(31)+1;
			break;
	}

   	newent->id=curcreatureid++;

	for(int w=0;w<EB_COUNT;w++)newent->wound[w]=0;

   for(int a=0;a<EA_COUNT;a++)
   {
      newent->attributes[a]=1;
   }
   int attnum=32;
   while(attnum>0)
   {
      int a=randval(EA_COUNT);
      if(newent->attributes[a]<10)
      {
         newent->attributes[a]++;
         attnum--;
      }
   }

   newent->special[ESW_TEETH]=TOOTHNUM;
   newent->special[ESW_RIGHTEYE]=1;
   newent->special[ESW_LEFTEYE]=1;
   newent->special[ESW_NOSE]=1;
   newent->special[ESW_TONGUE]=1;
   newent->special[ESW_RIGHTLUNG]=1;
   newent->special[ESW_LEFTLUNG]=1;
   newent->special[ESW_HEART]=1;
   newent->special[ESW_LIVER]=1;
   newent->special[ESW_STOMACH]=1;
   newent->special[ESW_RIGHTKIDNEY]=1;
   newent->special[ESW_LEFTKIDNEY]=1;
   newent->special[ESW_SPLEEN]=1;
   newent->special[ESW_RIBS]=RIBNUM;
   newent->special[ESW_NECK]=1;
   newent->special[ESW_UPPERSPINE]=1;
   newent->special[ESW_LOWERSPINE]=1;

}

int entity_name(struct t_entity* who) {
	random_first_name(who->firstname,who->gender_id);
	random_last_name(who->lastname,who->align == ALIGN_ARCHCONSERVATIVE,who->gender_id);
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

char* entitytypesstr[] = {
	"Default Entity Type",
	"Type 1",
	"Type 2",
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

		if ((strlen(me->firstname) != 0) || (strlen(me->firstname) != 0)) {
		char fullname[66];
		strncpy(fullname,safe_name(me->firstname),32);
		strncat(fullname," ",1);
		strncat(fullname,safe_name(me->lastname),32);
		strncpy(o_name,fullname,strsize);
		return 0; }

		if (me->type != 0) {
			strncpy(o_name,entitytypesstr[me->type],strsize); 
			return 0; }
		
	}
	strncpy(o_name, "Generic Entity",32);
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
