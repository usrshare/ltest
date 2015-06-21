#ifndef WEAPON_H
#define WEAPON_H
#include <stdbool.h>

#include "item.h"

enum SpecialAttacks
{
	ATTACK_CANNON,
	ATTACK_FLAME,
	ATTACK_SUCK,
	ATTACKNUM
};

enum weapon_types {
	WT_CIVILIAN = -1,
	WT_NONE = 0,
	WT_REVOLVER_38,
	WT_REVOLVER_44,
	WT_SEMIPISTOL_45,
	WT_SEMIPISTOL_9MM,
	WT_SHOTGUN_AA12,
	WT_AUTORIFLE_AK47,
	WT_SEMIRIFLE_AR15,
	WT_DESERT_EAGLE,
	WT_FLAMETHROWER,
	WT_CARBINE_M4,
	WT_AUTORIFLE_M16,
	WT_M249_MACHINEGUN,
	WT_MOLOTOV,
	WT_SMG_MP5,
	WT_SHOTGUN_PUMP,
	WT_AXE,
	WT_BASEBALLBAT,
	WT_CHAIN,
	WT_COMBATKNIFE, 
	WT_CROSS,
	WT_CROWBAR,
	WT_DAGGER,
	WT_DAISHO,
	WT_GAVEL,
	WT_GUITAR,
	WT_HAMMER,
	WT_MAUL,
	WT_NIGHTSTICK,
	WT_PITCHFORK,
	WT_SHANK,
	WT_SPRAYCAN,
	WT_STAFF,
	WT_SWORD,
	WT_SYRINGE,
	WT_TORCH,
	WT_COUNT
};

enum clip_types {
		CT_NONE,
		CT_38,
		CT_44,
		CT_45,
		CT_50AE,
		CT_9,
		CT_SMG,
		CT_ASSAULT,
		CT_GASOLINE,
		CT_DRUM,
		CT_BUCKSHOT,
		CT_COUNT,
};

struct t_clip_type {
	int type;
	const char* name;
	const char* shortname;
	int ammo;
	int fencevalue;
};

struct t_attackst {
	//attackst(MCD_STR xmlchar*);
	int priority;
	bool ranged;
	bool thrown;
	enum clip_types ammotype;
	bool uses_ammo;
	char* attack_description;
	char* hit_description;
	bool always_describe_hit;
	bool can_backstab;
	char* hit_punctuation;
	int skill;
	int accuracy_bonus;
	int number_attacks;
	int successive_attacks_difficulty;
	int strength_min;
	int strength_max;
	int random_damage;
	int fixed_damage;
	bool bruises;
	bool tears;
	bool cuts;
	bool burns;
	bool shoots;
	bool bleeding;
	int severtype;
	bool damages_armor;
	int armorpiercing;
	int no_damage_reduction_for_limbs_chance;

	struct criticalst
	{
		//criticalst();
		int chance;
		int hits_required;
		int random_damage;
		int fixed_damage;
		int severtype;
	} critical;

	struct firest
	{
		//firest();   
		int chance;
		int chance_causes_debris;
	} fire;

};

#define MAX_ATTACKS 3

struct t_weapon_type {

	const char* name;
	const char* shortname;
	long fencevalue;
	bool can_take_hostages;
	bool is_threatening;
	bool can_threaten_hostages;
	bool protects_against_kidnapping;
	bool has_musical_attack;
	bool is_instrument;
	int legality;
	float bashstrengthmod;
	bool is_suspicious;
	int size;
	bool can_graffiti;
	bool uses_ammo;
	//bool acceptable_ammo(const Item& c);
	//bool acceptable_ammo(const Clip& c);
	//bool acceptable_ammo(const ClipType& c);
	bool is_ranged;
	bool is_throwable;
	bool auto_breaks_locks;
	
	struct t_attackst attacks[MAX_ATTACKS];

	const char* name_sub_1;
	const char* name_sub_2;
	const char* shortname_sub_1;
	const char* shortname_sub_2;
};
extern const struct t_weapon_type weapontypes [WT_COUNT];
extern const struct t_clip_type cliptypes [CT_COUNT];

const struct t_weapon_type* w_type(struct t_item* weapon);
const struct t_clip_type* c_type(struct t_item* clip);

const char* get_loaded_cliptypename(struct t_item* weapon);
const struct t_attackst* get_attack(struct t_item* w, bool force_ranged, bool force_melee, bool force_no_reload);

const char* get_weapon_name(struct t_item* w);
const char* get_weapon_name_sub (struct t_item* w, unsigned subtype);

const char* wt_get_name(const struct t_weapon_type* wt);
const char* wt_get_name_sub(const struct t_weapon_type* wt, unsigned subtype);

const char* hit_punctuation(const struct t_attackst* attack);
#endif
