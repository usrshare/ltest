#ifndef ARMOR_H
#define ARMOR_H

#include <stdbool.h>

#include "entity.h"
#include "item.h"

struct t_protection {
	int body;
	int head;
	int arms;
	int legs;
};

#define APPRWEAPONS 7

struct t_armor_type {
	int type;
	const char* name;
	int make_difficulty;
	int make_price;
	bool deathsquad_legality;
	bool can_get_bloody;
	bool can_get_damaged;
	struct t_protection armor;
	struct t_protection body_covering;
	int durability;
	bool has_fireprotection;
        int quality_levels;
	int stealth_value;
	int professionalism;
	int conceal_weapon_size;
	int appropriate_weapon[APPRWEAPONS];
	bool covers[EB_COUNT];
	bool is_mask;
	int fencevalue;

	int armor_body;
	int armor_head;
	int armor_limbs;
	bool fireprotection;
	bool cover_head;
	bool cover_body;
	bool cover_arms;
	bool cover_legs;
	bool conceal_face;

	const char* shortname;
	bool shortname_defined;

	int interrogation_basepower;
	int interrogation_assaultbonus;
	int interrogation_drugbonus;

	int conceal_weaponsize;
	//vector<string> appropriate_weapon;

	//For masks:
	bool mask;
	bool surprise_mask;
	const char* description;

};

enum armordmgflags {
	AD_BLOODY = 32,
	AD_DAMAGED = 16,
	//0 -- 15 represent quality.
	//all quality_levels defined are lower than 15.
};

#define AD_QUALITY 7 //quality mask

enum armortype {
	ARMOR_NAKED = -2, //for generation only
	ARMOR_CIVILIAN = -1, //for generation only
	ARMOR_NONE,
	ARMOR_ARMYARMOR,
	ARMOR_MILITARY,
	ARMOR_CATSUIT,
	ARMOR_BLACKDRESS,
	ARMOR_LEATHER,
	ARMOR_BLACKROBE,
	ARMOR_BLACKSUIT,
	ARMOR_BLACKCLOTHES,
	ARMOR_CIVILIANARMOR,
	ARMOR_BONDAGEGEAR,
	ARMOR_BUNKERGEAR,
	ARMOR_CHEAPDRESS,
	ARMOR_CHEAPSUIT,
	ARMOR_CLOTHES,
	ARMOR_CLOWNSUIT,
	ARMOR_DEATHSQUADUNIFORM,
	ARMOR_DONKEYSUIT,
	ARMOR_ELEPHANTSUIT,
	ARMOR_EXPENSIVEDRESS,
	ARMOR_EXPENSIVESUIT,
	ARMOR_PRISONGUARD,
	ARMOR_HEAVYARMOR,
	ARMOR_LABCOAT,
	ARMOR_MASK,
	ARMOR_MITHRIL,
	ARMOR_NINJASUIT,
	ARMOR_PRISONER,
	ARMOR_OVERALLS,
	ARMOR_POLICEARMOR,
	ARMOR_POLICEUNIFORM,
	ARMOR_SEALSUIT,
	ARMOR_SECURITYUNIFORM,
	ARMOR_SERVANTUNIFORM,
	ARMOR_SWATARMOR,
	ARMOR_TOGA,
	ARMOR_TRENCHCOAT,
	ARMOR_HARDHAT,
	ARMOR_WIFEBEATER,
	ARMOR_WORKCLOTHES,
	ARMOR_COUNT
};

extern const struct t_armor_type armortypes[ARMOR_COUNT];

struct t_item* get_armor(struct t_creature* e);
const struct t_armor_type* a_type(struct t_item* armor);

bool armor_type_covers(const struct t_armor_type* t, int bodypart);
int get_armor_type_value(const struct t_armor_type* t, int bodypart);
int armor_get_value(struct t_item* armor, int bodypart);

int armor_get_quality(struct t_item* armor);
void armor_set_quality(struct t_item* armor, int quality);

int new_armor(const struct t_armor_type* type,struct t_item* o_item);
int give_armor(struct t_creature* cr, struct t_item* armor);

#endif
