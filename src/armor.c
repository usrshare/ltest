#include "armor.h"

const struct t_armor_type armortypes[ARMOR_COUNT] = {
	{ /*ARMOR_NONE*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.name = "None (naked)", .shortname = "Naked",
		.body_covering = {.head = 0, .body = 0, .arms = 0, .legs = 0}, .can_get_bloody = false, .can_get_damaged = false,
		.professionalism = 0, .conceal_weapon_size = 0,
	},
	{ /*ARMOR_ARMYARMOR*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.name = "Army Body Armor", .shortname = "Army Armor",
		.armor = {.body = 8, .head = 6},
		.body_covering = {.head = 1, .body = 1},
		.professionalism = 0, .conceal_weapon_size = 0,
	},


};

bool armor_decrease_quality(struct t_armor* armor, int decrease)
{
   armor->quality+=decrease;
   if(armor->quality<1) armor->quality=1;
   return armor->quality<=armor->type->quality_levels;
}
