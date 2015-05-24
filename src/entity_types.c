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

struct t_entity_generate_rules type_rules[ENTT_COUNT] = {
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
	{.type = ET_SWAT,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_CON,EA_STR,EA_WIS},
	.attrlim = {{3,10},{3,10},{3,10},{4,10}},
	.gender = EG_MBIAS,
	.infiltration = {30,60},
	.juice = {40,90},
	.skill = {ES_DRIVING,ES_HANDTOHAND,ES_PISTOL,ES_PSYCHOLOGY,ES_RIFLE,ES_SHOTGUN},
	.skilllim = {{1,2},{1,2},{1,4},{1,4},{1,4},{1,4}},
	},
	{.type = ET_DEATHSQUAD,
	.age = A_YOUNGADULT,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_CON,EA_STR,EA_WIS},
	.attrlim = {{3,10},{3,10},{3,10},{6,10}},
	.gender = EG_MBIAS,
	.infiltration = {50,80},
	.juice = {90,140},
	.skill = {ES_DRIVING,ES_HANDTOHAND,ES_PISTOL,ES_PSYCHOLOGY,ES_RIFLE,ES_SHOTGUN},
	.skilllim = {{1,2},{1,2},{1,2},{2,5},{2,5},{1,2}},
	},
	{.type = ET_FIREFIGHTER,
	.age = A_MATURE,
	.gender = EG_MBIAS,
	.attrs = {EA_AGI,EA_CON,EA_STR},
	.attrlim = {{3,10},{3,10},{3,10}},
	.infiltration = {0,30},
	},
	{.type = ET_EDUCATOR,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_CON,EA_STR,EA_WIS},
	.attrlim = {{3,10},{3,10},{3,10},{4,10}},
	.infiltration = {0,30},
	.skill = {ES_CLUB,ES_DRIVING,ES_HANDTOHAND,ES_PSYCHOLOGY,ES_RIFLE,ES_SHOTGUN},
	.skilllim = {{1,2},{0,1},{1,2},{3,6},{1,3},{1,3}},
	},
	{.type = ET_GANGUNIT,
	.age = A_YOUNGADULT,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_CON,EA_STR,EA_WIS},
	.attrlim = {{3,10},{3,10},{3,10},{5,10}},
	.infiltration = {30,60},
	.juice = {40,90},
	.gender = EG_MBIAS,
	.skill = {ES_DRIVING,ES_HANDTOHAND,ES_PISTOL,ES_PSYCHOLOGY,ES_RIFLE,ES_SHOTGUN},
	.skilllim = {{1,2},{1,2},{1,4},{2,5},{1,3},{1,3}},
	},
	{.type = ET_JUDGE_LIBERAL,
	.age = A_MIDDLEAGED,
	.align = ALIGN_LIBERAL,
	.attrs = {EA_HRT,EA_INT},
	.attrlim = {{10,10},{5,10}},
	.money = {20,60},
	.skill = {ES_LAW,ES_WRITING},
	.skilllim = {{5,10},{1,3}},
	},
	{.type = ET_JUDGE_CONSERVATIVE,
	.age = A_MIDDLEAGED,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_HRT,EA_INT,EA_WIS},
	.attrlim = {{1,1},{5,10},{10,10}},
	.gender = EG_MBIAS,
	.money = {20,60},
	.infiltration = {50,80},
	.juice = {100,150},
	.skill = {ES_LAW,ES_WRITING},
	.skilllim = {{5,10},{1,3}},
	},
	{.type = ET_AGENT,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_CON,EA_INT,EA_STR,EA_WIS},
	.attrlim = {{7,10},{5,10},{4,10},{5,10},{5,10}},
	.gender = EG_MBIAS,
	.infiltration = {0,150},
	.juice = {200,350},
	.skill = {ES_CLUB,ES_DODGE,ES_DRIVING,ES_PISTOL,ES_PSYCHOLOGY,ES_RIFLE,ES_SHOTGUN,ES_SMG},
	.skilllim = {{2,9},{4,7},{4,9},{2,9},{2,9},{4,9},{2,9},{2,9},{2,9}},
	},
	{.type = ET_SECRET_SERVICE,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_CON,EA_INT,EA_STR,EA_WIS},
	.attrlim = {{7,10},{5,10},{4,10},{5,10},{5,10}},
	.gender = EG_MBIAS,
	.infiltration = {0,150},
	.juice = {200,300},
	.skill = {ES_DODGE,ES_DRIVING,ES_PISTOL,ES_PSYCHOLOGY,ES_RIFLE,ES_SMG},
	.skilllim = {{4,9},{4,9},{4,9},{4,9},{4,9},{4,9}},
	},
	{.type = ET_RADIOPERSONALITY,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_CHA,EA_WIS},
	.attrlim = {{10,10},{8,10}},
	.infiltration = {60,90},
	.juice = {100,150},
	.money = {50,100},
	.skill = {ES_PERSUASION},
	.skilllim = {{1,8}},
	},
	{.type = ET_NEWSANCHOR,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_CHA,EA_WIS},
	.attrlim = {{10,10},{8,10}},
	.infiltration = {60,90},
	.juice = {100,150},
	.money = {50,100},
	},
	{.type = ET_GENETIC,
	.age = A_DOGYEARS,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_CHA,EA_HRT,EA_INT,EA_STR,EA_WIS},
	.attrlim = {{1,20},{1,2},{1,6},{1,1},{1,20},{1,6}},
	},
	{.type = ET_GUARDDOG,
	.age = A_DOGYEARS,
	.align = ALIGN_CONSERVATIVE,
	.attrs = {EA_AGI,EA_HRT,EA_INT,EA_STR,EA_WIS},
	.attrlim = {{9,15},{1,6},{1,1},{7,12},{1,6}},
	},
	{.type = ET_PRISONER,
	.gender = EG_MBIAS,
	.juice = {-5,-5},
	.money = {0,1},
	},
	{.type = ET_JUROR,
	.age = A_MATURE,
	.align = ALIGN_CONSERVATIVE,
	.money = {0,1},
	},
	{.type = ET_LAWYER,
	.age = A_GRADUATE,
	.attrs = {EA_CHA,EA_INT},
	.attrlim = {{4,10},{6,10}},
	.money = {50,100},
	.skill = {ES_LAW,ES_PERSUASION},
	.skilllim = {{4,7},{2,5}},
	},
	{.type = ET_SEWERWORKER,
	.age = A_MATURE,
	.gender = EG_MBIAS,
	.money = {20,50},
	},
	{.type = ET_COLLEGESTUDENT,
	.age = {18,23},
	.money = {20,50},
	.skill = {ES_ART,ES_COMPUTERS,ES_MUSIC,ES_SCIENCE,ES_WRITING},
	.skilllim = {{0,2},{0,1},{0,2},{0,2},{1,2}},
	},
	{.type = ET_MUSICIAN,
	.age = A_MATURE,
	.money = {0,10},
	.skill = {ES_MUSIC,ES_SEDUCTION},
	.skilllim = {{2,5},{0,1}},
	},
	{.type = ET_MATHEMATICIAN,
	.age = A_MATURE,
	.attrs = {EA_INT},
	.attrlim = {{7,10}},
	.money = {20,50},
	.skill = {ES_COMPUTERS,ES_SCIENCE},
	.skilllim = {{0,2},{1,3}},
	},
	{.type = ET_TEACHER,
	.age = A_MATURE,
	.gender = EG_FBIAS,
	.money = {20,50},
	.skill = {ES_TEACHING},
	.skilllim = {{3,6}},
	},
	{.type = ET_HSDROPOUT,
	.age = A_TEENAGER,
	.money = {20,50},
	},
	{.type = ET_BUM,
	.age = A_YOUNGADULT,
	.money = {20,50},
	.align = ALIGN_ELITELIBERAL,
	.random_align = true, //will return anything but conservative.
	},
	{.type = ET_GANGMEMBER,
	.age = A_YOUNGADULT,
	.gender = EG_MBIAS,
	.juice = {-5,-5},
	.money = {20,50},
	.skill = {ES_PISTOL,ES_RIFLE,ES_SHOTGUN,ES_STREETSENSE},
	.skilllim = {{1,2},{1,2},{1,2},{2,3}},
	},
	{.type = ET_CRACKHEAD,
	.age = A_YOUNGADULT,
	.align = ALIGN_ELITELIBERAL,
	.random_align = true, //will return anything but conservative.
	//.attrpts = 30,
	.attrs = {EA_INT},
	.attrlim = {{1,1}},
	.juice = {-20,-20},
	.money = {20,50},
	},
	{.type = ET_PRIEST,
	.age = A_MATURE,
	.gender = EG_MBIAS,
	.money = {20,50},
	.skill = {ES_RELIGION},
	.skilllim = {{3,7}},
	},
	{.type = ET_FASTFOODWORKER,
	.money = {20,50},
	},
	{.type = ET_BARISTA,
	.age = A_YOUNGADULT,
	.money = {20,50},
	},
	{.type = ET_BARTENDER,
	.age = A_MATURE,
	.gender = EG_MBIAS,
	.money = {20,50},
	},
	{.type = ET_TELEMARKETER,
	.age = A_YOUNGADULT,
	.money = {20,50},
	.skill = {ES_PERSUASION},
	.skilllim = {{1,3}},
	},
	{.type = ET_CARSALESMAN,
	.age = A_MATURE,
	.money = {40,80},
	.skill = {ES_BUSINESS,ES_DRIVING,ES_PERSUASION},
	.skilllim = {{1,4},{1,3},{1,4}},
	},
	{.type = ET_OFFICEWORKER,
	.age = A_MATURE,
	.money = {20,50},
	.skill = {ES_BUSINESS},
	.skilllim = {{0,2}},
	},
	{.type = ET_FOOTBALLCOACH,
	.age = A_MIDDLEAGED,
	.gender = EG_MBIAS,
	.money = {20,50},
	.skill = {ES_PERSUASION,ES_TEACHING},
	.skilllim = {{2,5},{2,4}},
	},
	{.type = ET_PROSTITUTE,
	.age = A_YOUNGADULT,
	.align = ALIGN_LIBERAL,
	.random_align = true,
	.juice = {-5, -5},
	.money = {20, 50},
	.skill = {ES_PERSUASION,ES_SEDUCTION,ES_STREETSENSE},
	.skilllim = {{2,5},{2,5},{2,3}},
	},
	{.type = ET_MAILMAN,
	.age = A_MATURE,
	.money = {20,50},
	},
	{.type = ET_GARBAGEMAN,
	.age = A_MATURE,
	.gender = EG_MBIAS,
	.money = {20,50},
	.skill = {ES_DRIVING},
	.skilllim = {{1,2}},
	},
	{.type = ET_PLUMBER,
	.age = A_MATURE,
	.gender = EG_MBIAS,
	.money = {20,50},
	},
	{.type = ET_CHEF,
	.age = A_MATURE,
	.gender = EG_MBIAS,
	.money = {20,50},
	},
	{.type = ET_CONSTRUCTIONWORKER,
	.age = A_MATURE,
	.attrs = {EA_CON,EA_STR},
	.attrlim = {{7,10},{7,10}},
	.gender = EG_MBIAS,
	.money = {20,50},
	.skill = {ES_DRIVING},
	.skilllim = {{1,2}},
	},
	{.type = ET_AMATEURMAGICIAN,
	.age = A_MATURE,
	.money = {20,50},
	},
	{.type = ET_TANK,
	.age = A_TEENAGER,
	.align = ALIGN_CONSERVATIVE,
	.gender = EG_NEUTRAL,
	.skill = {ES_HANDTOHAND},
	.skilllim = {{20,20}},
	},

};
