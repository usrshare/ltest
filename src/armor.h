#ifndef ARMOR_H
#define ARMOR_H

#include <stdbool.h>

#include "entity.h"

struct t_armor_type {
	int durability;
	bool has_fireprotection;
        int quality_levels;
	bool covers[EB_COUNT];
};

struct t_armor {

	struct t_armor_type* type;
	bool bloody;
	bool damaged;
	int quality;
};
#endif
