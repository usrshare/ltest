// vim: cin:sts=4:sw=4 
#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "globals.h"
#include "random.h"
#include "item.h"

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

enum AnimalGlosses{
    ANIMALGLOSS_NONE,//IMPORTANT THAT THIS BE HERE AT ZERO
    ANIMALGLOSS_TANK,
    ANIMALGLOSS_ANIMAL,
    ANIMALGLOSSNUM
};

enum creature_flags {
    CREATUREFLAG_WHEELCHAIR = 1,
    CREATUREFLAG_JUSTESCAPED = 2,
    CREATUREFLAG_MISSING = 4,
    CREATUREFLAG_KIDNAPPED = 8,
    CREATUREFLAG_SLEEPER = 16,
    CREATUREFLAG_ILLEGALALIEN = 32,
    CREATUREFLAG_LOVESLAVE = 64,
    CREATUREFLAG_BRAINWASHED = 128,
    CREATUREFLAG_CONVERTED = 256,
};

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
enum entity_bodyparts{
    EB_HEAD,
    EB_BODY,
    EB_ARM_RIGHT,
    EB_ARM_LEFT,
    EB_LEG_RIGHT,
    EB_LEG_LEFT,
    EB_COUNT
};
enum entity_specialwounds{
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
    WOUND_NONE=0,
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
    ES_P_ESCAPEDRIVE = -2,
    ES_P_DODGEDRIVE,
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

enum Alignment{
    ALIGN_ARCHCONSERVATIVE = -2,
    ALIGN_CONSERVATIVE,
    ALIGN_MODERATE,
    ALIGN_LIBERAL,
    ALIGN_ELITELIBERAL,
    ALIGN_STALINIST
};
enum CheckDifficulty{
   DIFFICULTY_AUTOMATIC    = 1,
   DIFFICULTY_VERYEASY     = 3,
   DIFFICULTY_EASY         = 5,
   DIFFICULTY_AVERAGE      = 7,
   DIFFICULTY_CHALLENGING  = 9,
   DIFFICULTY_HARD         = 11,
   DIFFICULTY_FORMIDABLE   = 13,
   DIFFICULTY_HEROIC       = 15,
   DIFFICULTY_SUPERHEROIC  = 17,
   DIFFICULTY_IMPOSSIBLE   = 19
};

struct t_location;  //just saying.

enum Activity{
   ACTIVITY_NONE,
   ACTIVITY_VISIT,
   ACTIVITY_HOSTAGETENDING,
   ACTIVITY_TROUBLE,
   ACTIVITY_GRAFFITI,
   ACTIVITY_COMMUNITYSERVICE,
   ACTIVITY_SELL_ART,
   ACTIVITY_SELL_MUSIC,
   ACTIVITY_SELL_TSHIRTS,
   ACTIVITY_DONATIONS,
   ACTIVITY_SELL_DRUGS,
   ACTIVITY_PROSTITUTION,
   ACTIVITY_POLLS,
   ACTIVITY_CCFRAUD,
   ACTIVITY_DOS_RACKET,
   ACTIVITY_DOS_ATTACKS,
   ACTIVITY_HACKING,
   ACTIVITY_REPAIR_ARMOR,
   ACTIVITY_MAKE_ARMOR,
   ACTIVITY_STEALCARS,
   ACTIVITY_WHEELCHAIR,
   ACTIVITY_BURY,
   ACTIVITY_WRITE_BLOG,
   ACTIVITY_WRITE_LETTERS,
   ACTIVITY_WRITE_GUARDIAN,
   ACTIVITY_TEACH_POLITICS,
   ACTIVITY_TEACH_FIGHTING,
   ACTIVITY_TEACH_COVERT,
   ACTIVITY_STUDY_DEBATING,
   ACTIVITY_STUDY_MARTIAL_ARTS,
   ACTIVITY_STUDY_DRIVING,
   ACTIVITY_STUDY_PSYCHOLOGY,
   ACTIVITY_STUDY_FIRST_AID,
   ACTIVITY_STUDY_LAW,
   ACTIVITY_STUDY_DISGUISE,
   ACTIVITY_STUDY_SCIENCE,
   ACTIVITY_STUDY_BUSINESS,
   //ACTIVITY_STUDY_COOKING,
   ACTIVITY_STUDY_GYMNASTICS,
   ACTIVITY_STUDY_MUSIC,
   ACTIVITY_STUDY_ART,
   ACTIVITY_STUDY_TEACHING,
   ACTIVITY_STUDY_WRITING,
   ACTIVITY_CLINIC,
   ACTIVITY_HEAL,
   ACTIVITY_SLEEPER_LIBERAL,
   ACTIVITY_SLEEPER_CONSERVATIVE,
   ACTIVITY_SLEEPER_SPY,
   ACTIVITY_SLEEPER_RECRUIT,
   ACTIVITY_SLEEPER_SCANDAL,
   ACTIVITY_SLEEPER_EMBEZZLE,
   ACTIVITY_SLEEPER_STEAL,
   ACTIVITY_SLEEPER_JOINLCS,
   ACTIVITY_STUDY_LOCKSMITHING,
   ACTIVITY_RECRUITING,
   ACTIVITYNUM
};
struct activityst {
    int type;
    long arg, arg2;
};
struct t_creature {

    uint32_t id;

    uint8_t attributes[EA_COUNT]; //attributes
    uint16_t skills[ES_COUNT]; //skills
    int skill_experience[ES_COUNT];

    char firstname[32];   // current first name (liberal)
    char o_firstname[32]; // original first name (conservative)
    char middlename[32];  // 
    char lastname[32];    //
    char nickname[32];    // in-squad nickname
    bool name_known;      // does the squad know this entity's name?

    enum entity_gender gender_bio; //biological gender (conservative)
    enum entity_gender gender_id; // gender identity (liberal)

    int squadid;
    uint32_t hireid;

    int age;
    int birthday_month;
    int birthday_day;

    bool exists;
    char align;
    char orig_align;
    bool alive;
    short type;

    unsigned char wound[EB_COUNT];
    char special[ESW_COUNT];
    short blood;
    
    struct t_item inventory[INVENTORY_SIZE];

    // these two pointers should only point to NULL or entries in inventory!
    struct t_item* weapon;
    struct t_item* armor;

   

    int heat;
    int location;
    int worklocation;

    struct activityst activity;

    int16_t juice;
    int32_t money;

    char cantbluff;
    float infiltration;
    char animalgloss;
    short specialattack;

    struct t_creature* prisoner;

    char forceinc;
    
    int stunned;
    bool has_thrown_weapon;
   
    int crimes_suspected[LAWFLAGNUM];
    
    int flag;
};

#define RANDATTRS 7
#define RANDSKILLS 10
#define RANDWEAPONS 7
#define RANDARMORS 3

struct t_creature_generate_rules {

    // this is a list of rules/limits the entity generator will have to obey.
    // its functionality roughly corresponds with the rules given in
    // Liberal Crime Squad's art/creatures.xml file.
    short type;
    char* type_name;
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

    struct t_range attrpts; //attribute points. 32 is default.

    short weapons[RANDWEAPONS];
    struct t_range ammo[RANDWEAPONS];
    short armors[RANDARMORS];


};

#define SQUAD_NAMELEN 40
#define SQUAD_MAXITEMS 60

struct t_squad {
    char name [SQUAD_NAMELEN];
    struct t_creature *squad[6];
    //struct t_activity activity;
    int id;
    //vector <Item*> loot;

    char stance;
};

extern struct t_squad* activesquad;
extern struct t_creature* encounter[ENCMAX];

#define POOLSIZE 1024

extern struct t_creature* pool[POOLSIZE];

/// --- /// --- ///

enum entity_attr assoc_attr (enum entity_skill skill_type);

int random_gender_and_age(int* o_age, enum entity_gender* o_gender);

//Creates a creature
int creature_init(struct t_creature* o_entity, struct t_creature_generate_rules* genrules);
//Returns either a name, or ???
const char* safe_name(const char* nameptr);

//Writes a string that describes the entity (name / profession) into the memory stored
int describe_entity(struct t_creature* me, char* const restrict o_name, size_t strsize);

const char* describe_entity_static(struct t_creature* me);

//Gets the value of an attribute, according to all the changes caused by age, juice and health.
int entity_get_attribute(struct t_creature* me, enum entity_attr attribute, bool usejuice);

int entity_get_skill(struct t_creature* me, enum entity_skill skill);

const char* entity_heshe(struct t_creature* e,bool capitalize);
const char* entity_hisher(struct t_creature* e,bool capitalize);
const char* entity_himher(struct t_creature* e,bool capitalize);

struct t_creature* getChaseDriver(struct t_creature* e);
struct t_vehicle* getChaseVehicle(struct t_creature* e);

int entity_attr_roll(struct t_creature* e,enum entity_attr attribute);
int entity_skill_roll (struct t_creature* e, enum entity_skill skill);
bool entity_attr_check(struct t_creature* e, enum entity_attr attr, int difficulty);
bool entity_skill_check(struct t_creature* e, enum entity_skill skill, int difficulty);

void entity_train(struct t_creature* e, int trainedskill, int experience);
void entity_train4(struct t_creature* e, int trainedskill, int experience, int upto);

int entity_count_weapons(struct t_creature* e);

void addjuice(struct t_creature* e, long juice, long cap);

void creature_die(struct t_creature* e);

bool enemy(struct t_creature* e);

bool entity_can_reload(struct t_creature* e);

struct t_item* entity_get_weapon (struct t_creature* e);
struct t_item* entity_get_armor (struct t_creature* e);
bool can_reload(struct t_creature* e);

void creature_conservatize( struct t_creature *cr);
void creature_liberalize(struct t_creature *cr);

void entity_drop_weapon(struct t_creature* e, struct t_item* loot);
void creature_strip(struct t_creature* cr, struct t_item* lootpile);
#endif
