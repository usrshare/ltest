#include "entity_types.h"

//The data in this file is based on Liberal Crime Squad's art/creatures.xml file.

//Turns out, you don't have to define all the elements of an array, the rest is filled with zeroes.
//This makes creating such definitions much easier.

#define A_DOGYEARS {2,6}
#define A_CHILD {7,10}
#define A_TEENAGER {14,17}
#define A_YOUNGADULT {18,35}
#define A_MATURE {20,59}
#define A_GRADUATE {26,59}
#define A_MIDDLEAGED {35,59}
#define A_SENIOR {65,94}

struct t_entity_generate_rules type_rules[ET_COUNT] = {
	{.type = ET_BOUNCER, .gender = EG_MBIAS,
	.attrs = {EA_AGI,EA_CON,EA_STR},
	.attrlim = {{3,10},{3,10},{4,10}},
	.skill = {ES_HANDTOHAND},
	.skilllim = {{1,3}}
	},
	{.type = ET_SECURITYGUARD, .gender= EG_MBIAS,
	.attrs = {EA_AGI,EA_CON,EA_STR},
	.attrlim = {{3,10},{3,10},{2,10}},
	.infiltration = {0,20},
	.skill = {ES_PISTOL},
	.skilllim = {{1,3}},
	},
	{.type = ET_SCIENTIST_LABTECH, .gender = EG_MBIAS,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_INT},
	.attrlim = {{5,10}},
	.infiltration = {0,30},
	.skill = {ES_COMPUTERS,ES_SCIENCE},
	.skilllim = {{1,2},{3,6}},
	},
	{.type = ET_SCIENTIST_EMINENT, .gender = EG_MBIAS,
	.age = A_MIDDLEAGED,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_INT, EA_WIS},
	.attrlim = {{10,10},{6,10}},
	.infiltration = {50,80},
	.juice = {100,150},
	.money = {20,60},
	.skill = {ES_SCIENCE,ES_WRITING},
	.skilllim = {{6,11},{1,3}},
	},
	{.type = ET_POLITICIAN, .gender = EG_MBIAS,
	.age = A_MIDDLEAGED,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_WIS},
	.attrlim = {{6,10}},
	.infiltration = {80,95},
	.juice = {400,800},
	.money = {40,80},
	.skill = {ES_LAW,ES_PERSUASION},
	.skilllim = {{1,7},{5,8}},
	},
	{.type = ET_CORPORATE_MANAGER, .gender = EG_MBIAS,
	.age = A_MATURE, .align = ALIGN_CONSERVATIVE,
	.attrs = {EA_CHA, EA_INT, EA_WIS},
	.attrlim = {{5,10},{4,10},{5,10}},
	.infiltration = {0,50},
	.money = {40,80},
	.skill = {ES_BUSINESS},
	.skilllim = {{3,6}},
	},
	{.type = ET_CORPORATE_CEO, .gender = EG_MBIAS,
	.age = A_MIDDLEAGED, .align = ALIGN_CONSERVATIVE,
	.attrs = {EA_CHA,EA_INT,EA_WIS},
	.attrlim = {{7,10},{7,10},{12,12}},
	.infiltration = {100,100},
	.juice = {1000,1000},
	.money = {1000,2000},
	.skill = {ES_BUSINESS,ES_DODGE,ES_PISTOL},
	.skilllim = {{10,15},{10,15},{10,15}},
	},
	{.type = ET_WORKER_SERVANT,
	.age = A_MATURE, .align = ALIGN_LIBERAL, .money = {0,1}
	},
	{.type = ET_WORKER_JANITOR,
	.age = A_MATURE},
	{.type = ET_WORKER_SWEATSHOP,
	.age = A_MATURE, .align = ALIGN_LIBERAL,
	.juice = {-20,-20},
	.skill = {ES_TAILORING},
	.skilllim = {{1,5}},
	},
	{.type = ET_WORKER_FACTORY_NONUNION,
	.age = A_MATURE, .gender = EG_MBIAS,
	.attrs = {EA_STR}, .attrlim = {{5,10}},
	},
	{.type = ET_WORKER_FACTORY_CHILD,
	.age = A_CHILD, .align = ALIGN_LIBERAL,
	.attrs = {EA_HRT},
	.attrlim = {{8,10}},
	.money = {0,1},
	},
	{.type = ET_WORKER_SECRETARY,
	.age = A_MATURE,
	.attrs = {EA_INT},
	.attrlim = {{2,10}},
	.skill = {ES_WRITING},
	.skilllim = {{1,2}},
	},
	{.type = ET_WORKER_FACTORY_UNION,
	.age = A_MATURE,
	.align = ALIGN_LIBERAL,
	.gender = EG_MBIAS,
	.attrs = {EA_STR},
	.attrlim = {{5,10}},
	},
	{.type = ET_LANDLORD,
	.age = A_MATURE,
	.align = ALIGN_MODERATE,
	.attrs = {EA_INT,EA_WIS},
	.attrlim = {{4,10},{4,10}},
	.gender = EG_MBIAS,
	.money = {120,240},
	.skill = {ES_BUSINESS},
	.skilllim = {{3,6}},
	},
	{.type = ET_BANK_TELLER,
	.age = A_MATURE,
	.align = ALIGN_MODERATE,
	},
	{.type = ET_BANK_MANAGER,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_INT,EA_WIS},
	.attrlim = {{4,10},{4,10}},
	.money = {120,240},
	.skill = {ES_BUSINESS},
	.skilllim = {{3,6}},
	},
	{.type = ET_TEENAGER,
	.age = A_TEENAGER,
	},
	{.type = ET_COP,
	.age = A_MATURE,
	.attrs = {EA_AGI,EA_CON,EA_STR},
	.attrlim = {{3,10},{3,10},{3,10}},
	.gender = EG_MBIAS,
	.infiltration = {30,60},
	.juice = {10,60},
	.skill = {ES_DRIVING,ES_PSYCHOLOGY},
	.skilllim = {{1,2},{2,3}},
	},
};
