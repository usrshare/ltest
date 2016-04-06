// vim: cin:sts=4:sw=4 
#include "item_ui.h"
#include "armor.h"
#include "weapon.h"

const char* describe_item (struct t_item* i) {

	switch (i->type) {

		case IT_WEAPON:
		return get_weapon_name(i);
	       	case IT_ARMOR:
		return get_armor_name(i);
		case IT_CLIP:
		return cliptypes[i->itemtypeid].name;
		case IT_MONEY:
		return "$";
		default:
		return "something";
	}
}
