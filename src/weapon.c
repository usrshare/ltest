#include "weapon.h"

struct attackst* get_attack(struct t_weapon* w, bool force_ranged, bool force_melee, bool force_no_reload) {

	const struct attackst* attacks = get_attacks(w->type);

	for(int i=0;i<MAX_ATTACKS;i++)
	{
		if(force_ranged&&!attacks[i]->ranged) continue;
		if(force_melee&&attacks[i]->ranged) continue;
		if(force_no_reload&&attacks[i]->uses_ammo&&!ammo_) continue;
		if(attacks[i]->uses_ammo&&attacks[i]->ammotype!=loaded_cliptype_&&ammo_) continue;
		if (!attacks[i]) continue;
		return attacks[i];
	}
	return NULL;
}
