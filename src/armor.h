#ifndef ARMOR_H
#define ARMOR_H

#include <stdbool.h>

#include "entity.h"

struct t_armor_type {
	const char* name;
	int durability;
	bool has_fireprotection;
        int quality_levels;
	bool covers[EB_COUNT];
	bool is_mask;
};

struct t_armor {

	struct t_armor_type* type;
	bool bloody;
	bool damaged;
	int quality;
};

enum armortype {
    ARMOR_NONE,
    ARMOR_ARMYARMOR,
    ARMOR_MILITARY,
    ARMOR_CATSUIT,
    ARMOR_BLACKDRESS,
    ARMOR_LEATHER,
    ARMOR_BLACKROBE,
    ARMOR_BLACKSUIT,
    ARMOR_BLACKCLOTHES,
    ARMOR_CIVILLIANARMOR,
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

extern struct t_armor_type armortypes[ARMOR_COUNT];

#endif
