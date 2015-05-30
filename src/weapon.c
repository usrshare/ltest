#include "weapon.h"

#include <stdlib.h>

const struct t_weapon_type weapontypes [WT_COUNT] = {

};

const struct t_clip_type cliptypes [CT_COUNT] = {

};


const struct t_clip_type* c_type(struct t_item* clip) {

	if (clip == NULL) return NULL;	
	if (clip->type != IT_CLIP) return NULL;
	return &cliptypes[clip->itemtypeid];

}

const struct t_attackst* get_attack(struct t_item* w, bool force_ranged, bool force_melee, bool force_no_reload) {

	const struct t_attackst* attacks = w_type(w)->attacks;

	for(int i=0;i<MAX_ATTACKS;i++)
	{
		if(force_ranged&&!attacks[i].ranged) continue;
		if(force_melee&&attacks[i].ranged) continue;
		if(force_no_reload&&attacks[i].uses_ammo&&!w->ammo) continue;
		if(attacks[i].uses_ammo&&attacks[i].ammotype!=(c_type(w->itemdetails))&&w->ammo) continue;
		if (attacks[i].priority == 0) continue;
		return &attacks[i];
	}
	return NULL;
}

const char* wt_get_name(const struct t_weapon_type* wt) {
	return wt->name;
}

const char* wt_get_name_sub(const struct t_weapon_type* wt, unsigned subtype) {

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

const char* get_weapon_name (struct t_item* w) {
	return wt_get_name(w_type(w));
}

const char* get_weapon_name_sub (struct t_item* w, unsigned subtype) {
	return wt_get_name_sub(w_type(w),subtype);
}


const struct t_weapon_type* w_type( struct t_item* weapon) {

	if (weapon->type != IT_WEAPON) return &weapontypes[WT_NONE];
	return &weapontypes[weapon->itemtypeid];

	return NULL;
}
