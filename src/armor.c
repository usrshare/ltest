#include "armor.h"
#include "weapon.h"

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
		.make_difficulty = 14, .make_price = 2000,
		.armor = {.body = 8, .head = 6},
		.body_covering = {.head = 1, .body = 1},
		.professionalism = 2, .quality_levels=6,
		.fencevalue = 250, .interrogation_basepower = 6,
		.interrogation_assaultbonus = 4,
		.appropriate_weapon = {WT_SMG_MP5, WT_CARBINE_M4, WT_AUTORIFLE_M16,WT_SEMIRIFLE_AR15},
	},
	{ /*ARMOR_MILITARY*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.name = "Army Uniform", .shortname = "Army Uniform",
		.make_difficulty = 8, .make_price = 40,
		.body_covering = {.head = 1, .body = 1},
		.professionalism = 2, .stealth_value = 1,
		.quality_levels=6,
		.fencevalue = 250, .interrogation_basepower = 6,
		.interrogation_assaultbonus = 4,
		.appropriate_weapon = {WT_SMG_MP5, WT_CARBINE_M4, WT_AUTORIFLE_M16,WT_SEMIRIFLE_AR15},
	},
	{ /*ARMOR_CATSUIT*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.make_difficulty = 7, .make_price = 150, .fencevalue = 50,
		.body_covering = {.body = 1,},
		.name = "Black Catsuit",
		.shortname = "Black Catsuit",
		.interrogation_basepower = 4,
		.interrogation_assaultbonus = 2,
		.stealth_value = 3,
		.quality_levels = 3,
	},
	{ /*ARMOR_BLACKDRESS*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.make_difficulty = 12, .make_price = 60, .fencevalue = 110,
		.name = "Black Dress", .shortname = "Black Dress",
		.body_covering = {.body = 1,},
		.interrogation_basepower = 6, .interrogation_assaultbonus = 4,
		.professionalism = 3, .stealth_value = 2, .quality_levels = 2,
	},
	{ /*ARMOR_LEATHER*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.make_difficulty = 7, .make_price = 300, .fencevalue = 100,
		.name = "Dark Leather Armor", .shortname = "Black Leather",
		.body_covering = {.body = 1, .arms = 0, .legs = 0},
		.armor = {.body = 3, .arms = 3, .legs = 3},
		.professionalism = 1, .stealth_value = 3, .quality_levels = 3, .durability = 20,
	},
	{ /*ARMOR_BLACKROBE*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.make_difficulty = 5, .make_price = 20, .fencevalue = 40,
		.name = "Black Robe", .shortname = "Black Robe",
		.interrogation_basepower = 2,
		.professionalism = 2, .stealth_value = 2, .quality_levels = 2,
		.appropriate_weapon = {WT_GAVEL},
	},
	{ /*ARMOR_BLACKSUIT*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.make_difficulty = 12, .make_price = 60, .fencevalue = 110,
		.name = "Black Suit", .shortname = "Black Suit",
		.body_covering = {.body = 1,},
		.interrogation_basepower = 6, .interrogation_assaultbonus = 4,
		.professionalism = 3, .stealth_value = 2, .quality_levels = 2,
	},	
	{ /*ARMOR_BLACKCLOTHES*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.make_difficulty = 4, .make_price = 15, .fencevalue = 20,
		.name = "Black Turtleneck", .shortname = "Bl.Turtleneck",
		.body_covering = {.body = 1,},
		.professionalism = 1, .stealth_value = 2, .quality_levels = 2,
	},
	{ /*ARMOR_CIVILIANARMOR*/ .can_get_bloody = true, .can_get_damaged = true,
		.professionalism = 2, .conceal_weapon_size = 5, .quality_levels = 4, .durability = 10,
		.name = "Body Armor", .shortname = "Body Armor",
		.make_difficulty = 8, .make_price = 500,
		.armor = {.body = 5},
		.body_covering = {.body = 1, .arms = 0, .legs = 0},
		.professionalism = 2,
		.fencevalue = 75, .interrogation_basepower = 2, .stealth_value = 1,
	},

};

const struct t_armor_type* a_type(struct t_item* armor) {
	if (!armor) return &armortypes[ARMOR_NONE];
	if (armor->type != IT_ARMOR) return &armortypes[ARMOR_NONE];
	return &armortypes[armor->itemtypeid];
}

enum armortype a_type_id(struct t_item* armor) {
	if (!armor) return ARMOR_NONE;
	if (armor->type != IT_ARMOR) return ARMOR_NONE;
	return armor->itemtypeid;
}

int armor_get_quality(struct t_item* armor) {
	return (armor ? armor->a_flags & AD_QUALITY : 0);
}

void armor_set_quality(struct t_item* armor, int quality) {

	armor->a_flags = (armor->a_flags & ~AD_QUALITY) | ( quality < AD_QUALITY ? quality : AD_QUALITY);
	return;
}

bool armor_decrease_quality(struct t_item* armor, int decrease)
{
   armor_set_quality(armor,armor_get_quality(armor)+decrease);
   if (armor_get_quality(armor) < 1) armor_set_quality(armor,1);

   return armor_get_quality(armor)<=(a_type(armor)->quality_levels);
}

bool armor_type_covers(const struct t_armor_type* t, int bodypart) {
   switch(bodypart)
   {
      case EB_HEAD: return t->cover_head;
      case EB_BODY: return t->cover_body;
      case EB_ARM_RIGHT:
      case EB_ARM_LEFT: return t->cover_arms;
      case EB_LEG_RIGHT:
      case EB_LEG_LEFT: return t->cover_legs;
   }

   return false;
}
int get_armor_type_value(const struct t_armor_type* t, int bodypart) {
   if(armor_type_covers(t,bodypart))
   {
      switch(bodypart)
      {
         case EB_HEAD:
            return t->armor_head;

         case EB_BODY:
            return t->armor_body;

         case EB_ARM_RIGHT:
         case EB_ARM_LEFT:
            return t->armor_limbs;

         case EB_LEG_RIGHT:
         case EB_LEG_LEFT:
            return t->armor_limbs;
      }
   }
   return 0;
}

int armor_get_value(struct t_item* armor, int bodypart) {
	return get_armor_type_value(a_type(armor),bodypart);

}

int new_armor(const struct t_armor_type* type,struct t_item* o_item) {

	struct t_item newarmor = {.type = IT_ARMOR, .itemtypeid = type->type, .a_flags = 0};

	*o_item = newarmor;
	return 0;
}

int give_armor(struct t_creature* cr, struct t_item armor) {

	struct t_item* invspace = inv_find_empty(cr->inventory);
	if (!invspace) return 1;

	*invspace = armor; //copy physically!
	cr->armor = invspace;
	
	return 0;
}
