// vim: cin:sts=4:sw=4 
#include "entity.h"
#include "entity_name.h"
#include "entity_types.h"

#include <malloc.h>
#include <string.h>
#include "globals.h"
#include "random.h"

#define MAXATTRIBUTE 99

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

int creature_init(struct t_creature* o_entity, struct t_creature_generate_rules* genrules) {

    memset(o_entity,0,sizeof(struct t_creature));

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

    if (genrules) {

	o_entity->type = genrules->type;

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

	for (int i=0; i < RANDATTRS; i++)
	   if (vrange(&genrules->attrlim[i])) o_entity->attributes[genrules->attrs[i]] = randrange(&genrules->attrlim[i]);
	for (int i=0; i < RANDSKILLS; i++)
	   if (vrange(&genrules->skilllim[i])) o_entity->skills[genrules->skill[i]] = randrange(&genrules->skilllim[i]);
	if (vrange(&genrules->attrpts)) attnum = randrange(&genrules->attrpts); 

    }
    
    while(attnum>0)
    {
	int a=randval(EA_COUNT);
	if(o_entity->attributes[a]<10)
	{
	    o_entity->attributes[a]++;
	    attnum--;
	}
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

int entity_name(struct t_creature* who) {
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

int describe_entity(struct t_creature* me, char* const restrict o_name, size_t strsize) {
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

char temp_name[128];

const char* describe_entity_static(struct t_creature* me) {
    int r = describe_entity(me,temp_name,128);
    return (r == 0 ? temp_name : NULL);
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

int entity_get_attribute(struct t_creature* me, enum entity_attr attribute, bool usejuice) {

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

const char* entity_heshe(struct t_creature* e,bool capitalize)
{  // subject pronoun (nominative case)
   switch(e->gender_id)
   {
   case EG_MALE: return capitalize?"He":"he";
   case EG_FEMALE: return capitalize?"She":"she";
   default: return capitalize?"Xe":"xe"; // Elite Liberal gender-neutral pronoun... it is pronounced "zee" rhyming with "he" and "she"
   // see http://homepage.ntlworld.com/jonathan.deboynepollard/FGA/sex-neutral-pronouns.html (great reference on this)
   // or http://en.wiktionary.org/wiki/xe or http://en.wikipedia.org/wiki/Gender-specific_and_gender-neutral_pronouns#Summary (wiki references)
   // or http://genderneutralpronoun.wordpress.com/about/alice/xe/ (examples of it being used in text)

   // full conjugation of "xe"/"xyr"/"xem" (the "x"es are pronounced like "z"s):
   // subject prononoun (nominative case):          xe      (pronounced "zee" rhyming with "he" and "she")
   // pronominal adjective (possessive determiner): xyr     (pronounced "zur" rhyming with "her")
   // object pronoun (oblique case);                xem     (pronounced "zem" rhyming with "them")
   // possessive pronoun:                           xyrs    (pronounced "zurz" rhyming with "hers")
   // reflexive pronoun:                            xemself (pronounced "zemself" rhyming with "themself")

   // public schools in Vancouver, British Columbia in Canada officially use these pronouns:
   // http://news.nationalpost.com/2014/06/17/vancouver-school-boards-genderless-pronouns-not-likely-to-stick-if-history-is-any-indication/
   }
}

const char* entity_hisher(struct t_creature* e,bool capitalize)
{  // pronominal adjective (possessive determiner)
   switch(e->gender_id)
   {
   case EG_MALE: return capitalize?"His":"his";
   case EG_FEMALE: return capitalize?"Her":"her";
   default: return capitalize?"Xyr":"xyr"; // Elite Liberal gender-neutral pronoun... it is pronounced "zur" rhyming with "her"
   // see http://homepage.ntlworld.com/jonathan.deboynepollard/FGA/sex-neutral-pronouns.html (great reference on this)
   // or http://en.wiktionary.org/wiki/xyr or http://en.wikipedia.org/wiki/Gender-specific_and_gender-neutral_pronouns#Summary (wiki references)
   // or http://genderneutralpronoun.wordpress.com/about/alice/xe/ (examples of it being used in text)

   // the possessive pronoun is based on this pronominal adjective in all standard third-person pronouns (so "xyrs" is correct):
   // his -> his, her -> hers, their -> theirs, and likewise xyr -> xyrs... just add "s" at the end if it doesn't already have an "s" at the end
   }
}
const char* entity_himher(struct t_creature* e,bool capitalize) {
// object pronoun (oblique case)
   switch(e->gender_id)
   {
   case EG_MALE: return capitalize?"Him":"him";
   case EG_FEMALE: return capitalize?"Her":"her";
   default: return capitalize?"Xem":"xem"; // Elite Liberal gender-neutral pronoun... it is pronounced "zem" rhyming with "them"
   // see http://homepage.ntlworld.com/jonathan.deboynepollard/FGA/sex-neutral-pronouns.html (great reference on this)
   // or http://en.wiktionary.org/wiki/xem or http://en.wikipedia.org/wiki/Gender-specific_and_gender-neutral_pronouns#Summary (wiki references)
   // or http://genderneutralpronoun.wordpress.com/about/alice/xe/ (examples of it being used in text)

   // the reflexive pronoun is based on this object pronoun in all standard third-person pronouns (so "xemself" is correct):
   // him -> himself, her -> herself, them -> themselves, it -> itself, one -> oneself, and likewise xem -> xemself... just add "self" unless plural in which case add "selves"

   // some people mistakenly use xyrself instead of xemself but this is wrong as it doesn't follow the pattern used by ALL standard third-person pronouns,
   // instead following the first-and-second-person pronoun pattern (my -> myself, your -> yourself/yourselves, our -> ourselves, thy -> thyself, and likewise xyr -> xyrself)
   }
}

struct t_creature* getChaseDriver(struct t_creature* e) {
    return NULL;
}

struct t_vehicle* getChaseVehicle(struct t_creature* e) {
    return NULL;
}



void entity_train(struct t_creature* e, int trainedskill, int experience) {
    return entity_train4(e,trainedskill,experience,MAXATTRIBUTE);
}

void entity_train4(struct t_creature* e, int trainedskill, int experience, int upto) {
   // Do we allow animals to gain skills? Right now, yes
   //if(animalgloss==ANIMALGLOSS_ANIMAL)return;

   // Don't give experience if already maxed out or requested to give none
   if(skill_cap(trainedskill,true)<=skills[trainedskill].value || upto<=skills[trainedskill].value || experience==0)
      return;
   // Skill gain scaled by ability in the area
   skill_experience[trainedskill]+=max(1,static_cast<int>(experience * skill_cap(trainedskill,false) / 6.0));

   int abovenextlevel;
   // only allow gaining experience on the new level if it doesn't put us over a level limit
   if (skills[trainedskill].value >= (upto - 1) ||
       skills[trainedskill].value >= (skill_cap(trainedskill,true) - 1))
     abovenextlevel = 0;
   else
     abovenextlevel = 50 + 5*(1+skills[trainedskill].value); // enough skill points to get halfway through the next skill level

   skill_experience[trainedskill] = min(skill_experience[trainedskill], 100 + 10*skills[trainedskill].value + abovenextlevel);

}

int entity_count_weapons(struct t_creature* e) {
    return 0;
}

void creature_die(struct t_creature* e) {

   e->alive=0,e->blood=0;
/*   if(e->id==uniqueCreatures.CEO().id)
      uniqueCreatures.newCEO();
   if(e->id==uniqueCreatures.President().id)
   {
      strcpy(oldPresidentName, execname[EXEC_PRESIDENT]);
      promoteVP();
      uniqueCreatures.newPresident();
   } */
}

void addjuice(struct t_creature* e, long juice, long cap) {
   
    if(juice==0) return;

   // Check against cap
   if((juice>0 && e->juice>=cap) ||
      (juice<0 && e->juice<=cap))
      return;

   // Apply juice gain
   e->juice+=juice;

   // Pyramid scheme of juice trickling up the chain
   if(e->hireid!=-1)
      for(int i=0;i<len(pool);i++)
         if(pool[i]->id==e->hireid)
         {
            addjuice(*pool[i],juice/5,cr.juice);
            break;
         }

   // Bounds check
   if(e->juice>1000)e->juice=1000;
   if(e->juice<-50)e->juice=-50;

}

bool enemy(struct t_creature* e){
   if(e->align==ALIGN_CONSERVATIVE)
      return true;
   else if(e->type==ET_COP && e->align==ALIGN_MODERATE)
   {
      for(int i=0;i<len(pool);i++)
         if(pool[i]==e)
            return false;
      return true;
   }
   else return false;
}

int entity_attr_roll(struct t_creature* e,enum entity_attr attribute){
   int return_value = roll_check(entity_get_attribute(e,attribute,true));
   // Roll on the attribute value
   return return_value;
}
int entity_skill_roll (struct t_creature* e, enum entity_skill skill) {
   int pseudoskill = 0;
   // Handle Pseudoskills
   if (skill < 0)
   {
      switch (skill)
      {
      default:
         g_setattr(CP_YELLOW);
         g_addstr("-=ILLEGAL SKILL ROLL=-", gamelog);
         g_getkey();
         break;
      case PSEUDOSKILL_ESCAPEDRIVE:
      case PSEUDOSKILL_DODGEDRIVE:
         pseudoskill = skill;   // Remember the details.
         skill = SKILL_DRIVING; // Base skill is driving.
         break;
      }
   }
   // Take skill strength
   int skill_value = skills[skill].value;
   // plus the skill's associate attribute
   int attribute_value = get_attribute(skills[skill].get_attribute(),true);

   int adjusted_attribute_value;
   switch(skill)
   {
   // most attributes get halved when applied to skills, capped by relative skill level...
   default:
      adjusted_attribute_value = MIN(attribute_value/2, skill_value+3);
      break;
   // ...and some may be so specialized that they ignore attributes, instead counting skill double
   case SKILL_SECURITY:
      adjusted_attribute_value = skill_value;
      break;
   }

   Vehicle* v = getChaseVehicle(*this);
   switch(pseudoskill)
   {
      case PSEUDOSKILL_ESCAPEDRIVE:
         if (v != NULL)
         {
            skill_value = v->modifieddriveskill(skill_value+adjusted_attribute_value); // combine values and modify by vehicle stats
            adjusted_attribute_value = 0;
         }
         else
         {
            skill_value = adjusted_attribute_value = 0; // Can't drive without a car
         }
         break;
      case PSEUDOSKILL_DODGEDRIVE:
         if (v != NULL)
         {
            skill_value = v->modifieddodgeskill(skill_value+adjusted_attribute_value); // combine values and modify by vehicle stats
            adjusted_attribute_value = 0;
         }
         else
         {
            skill_value = adjusted_attribute_value = 0; // Can't drive without a car
         }
         break;
   }
   // add the adjusted attribute and skill to get the adjusted skill total
   // that will be rolled on
   int return_value = roll_check(skill_value + adjusted_attribute_value);

   // Special skill handling
   switch(skill)
   {
   // Skills that cannot be used if zero skill:
   case SKILL_PSYCHOLOGY:
   case SKILL_LAW:
   case SKILL_SECURITY:
   case SKILL_COMPUTERS:
   case SKILL_MUSIC:
   case SKILL_ART:
   case SKILL_RELIGION:
   case SKILL_SCIENCE:
   case SKILL_BUSINESS:
   case SKILL_TEACHING:
   case SKILL_FIRSTAID:
      if(skills[skill].value == 0)
      {
         return_value = 0; // Automatic failure
         break;
      }
      break;
   // Skills that should depend on clothing:
   case SKILL_STEALTH:
      {
         float stealth = get_armor().get_stealth_value();
         for (int i=1; i < get_armor().get_quality();i++) stealth *= 0.8;
         if (get_armor().is_damaged()) stealth *= 0.5;

         return_value *= static_cast<int>(stealth);
         return_value /= 2;
         // Shredded clothes get you no stealth.
         if (get_armor().get_quality() > get_armor().get_quality_levels())
            return_value = 0;
      }
      break;
   case SKILL_SEDUCTION:
   case SKILL_PERSUASION:
      break;
   // Unique disguise handling
   case SKILL_DISGUISE:
      {
         // Check for appropriate uniform
         char uniformed = hasdisguise(*this);

         // Ununiformed disguise checks automatically fail
         if(!uniformed) { return_value = 0; break; }
         // reduce effectiveness for 'partial' uniforms (police uniforms when trespassing)
         else { if(uniformed==2) return_value>>=1; }

         // Bloody, damaged clothing hurts disguise check
         if(get_armor().is_bloody()) { return_value>>=1; }
         if(get_armor().is_damaged()) { return_value>>=1; }

         // Carrying corpses or having hostages is very bad for disguise
         if(prisoner!=NULL) { return_value>>=2; break; }
      }
   }
   #ifdef SHOWMECHANICS
   addstr(8,1," SkillRoll(");
   addstr(Skill::get_name(skill));
   addstr(", Skill Value ");
   addstr(skills[skill].value);
   addstr(", ");
   if(return_value==0)
      addstr("automatic failure");
   else
   {
      addstr("Adjusted Attribute Value ");
      addstr(adjusted_attribute_value);
      addstr(", Outcome of ");
      addstr(return_value);
   }
   addstr(")");

   getkey();
   #endif
   return return_value;
}
bool entity_attr_check(struct t_creature* e, enum entity_attr attr, int difficulty) {
   return(entity_attr_roll(e,attr) >= difficulty);
}
bool entity_skill_check(struct t_creature* e, enum entity_skill skill, int difficulty) {
   return(entity_skill_roll(e,skill) >= difficulty);
}
