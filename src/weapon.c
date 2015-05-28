#include "weapon.h"

#include <stdlib.h>

struct t_attackst* get_attack(struct t_weapon* w, bool force_ranged, bool force_melee, bool force_no_reload) {

	struct t_attackst** attacks = get_attacks(w->type);

	for(int i=0;i<MAX_ATTACKS;i++)
	{
		if(force_ranged&&!attacks[i]->ranged) continue;
		if(force_melee&&attacks[i]->ranged) continue;
		if(force_no_reload&&attacks[i]->uses_ammo&&!w->ammo) continue;
		if(attacks[i]->uses_ammo&&attacks[i]->ammotype!=w->loaded_cliptype&&w->ammo) continue;
		if (attacks[i] == NULL) continue;
		return attacks[i];
	}
	return NULL;
}

const char* wt_get_name(struct t_weapon_type* wt) {
	return wt->name;
}

const char* wt_get_name_sub(struct t_weapon_type* wt, unsigned subtype) {

	switch(subtype) {
		case 1:
			return (wt->name_sub_1 ? wt->name_sub_1 : wt_get_name(wt));
		case 2:
			return (wt->name_sub_2 ? wt->name_sub_2 : wt_get_name(wt));
		case 0:
		default:
			return wt_get_name(wt);
	}
}
