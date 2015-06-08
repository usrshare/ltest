#include "weapon.h"

#include "entity.h"
#include <stdlib.h>

const struct t_weapon_type weapontypes [WT_COUNT] = {
	{ .name = "Unarmed", .shortname = "None",
	.is_suspicious = 0, .can_threaten_hostages = 0,
	.protects_against_kidnapping = 0, .size = 0,
	.attacks = {{.skill = ES_HANDTOHAND, .accuracy_bonus = 0, .strength_min = 5, .strength_max = 10, .bruises = true, .priority = 1}},
	},

};

const struct t_clip_type cliptypes [CT_COUNT] = {
	{ .type = CT_38,
		.name = ".38 Speedloader",
		.ammo = 6,
		.fencevalue = 2,
	},
	{ .type = CT_44,
		.name = ".44 Speedloader",
		.ammo = 6,
		.fencevalue = 3,
	},
	{ .type = CT_45,
		.name = ".45 Pistol Mag",
		.ammo = 15,
		.fencevalue = 4,
	},
	{ .type = CT_50AE,
		.name = ".50AE Pistol Mag",
		.ammo = 7,
		.fencevalue = 3,
	},
	{ .type = CT_9,
		.name = "9mm Pistol Mag",
		.ammo = 7,
		.fencevalue = 3,
	},
	{ .type = CT_SMG,
		.name = "9mm SMG Mag",
		.ammo = 15,
		.fencevalue = 5,
	},
	{ .type = CT_ASSAULT,
		.name = "Assault Rifle Mag",
		.ammo = 30,
		.fencevalue = 8,
	},
	{ .type = CT_GASOLINE,
		.name = "Gas Tank",
		.ammo = 2,
		.fencevalue = 2,
	},
	{ .type = CT_DRUM,
		.name = "Machine Gun Drum",
		.ammo = 100,
		.fencevalue = 25,
	},
	{ .type = CT_BUCKSHOT,
		.name = "Shotgun Shells",
		.ammo = 8,
		.fencevalue = 3,
	},
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


const char* hit_punctuation(const struct t_attackst* attack) {
	return (attack->hit_punctuation ? attack->hit_punctuation : ".");
}

const struct t_weapon_type* w_type( struct t_item* weapon) {

	if (!weapon) return &weapontypes[WT_NONE];
	if (weapon->type != IT_WEAPON) return &weapontypes[WT_NONE];
	return &weapontypes[weapon->itemtypeid];

	return NULL;
}
