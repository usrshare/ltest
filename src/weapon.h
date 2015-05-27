#ifndef WEAPON_H
#define WEAPON_H

enum SpecialAttacks
{
   ATTACK_CANNON,
   ATTACK_FLAME,
   ATTACK_SUCK,
   ATTACKNUM
};

struct t_clip_type {
	const char* name;
	const char* shortname;
};

struct t_clip {

	int ammoamount;
};

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
	int get_legality;
	float get_bashstrengthmod;
	bool is_suspicious;
	int get_size;
	bool can_graffiti;
	bool uses_ammo;
	//bool acceptable_ammo(const Item& c);
	//bool acceptable_ammo(const Clip& c);
	//bool acceptable_ammo(const ClipType& c);
	bool is_ranged;
	bool is_throwable;
	bool auto_breaks_locks;
	bool is_legal;
};

struct t_attackst {
   //attackst(MCD_STR xmlchar*);
   int priority;
   bool ranged;
   bool thrown;
   char* ammotype;
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
      bool random_damage_defined;
      int fixed_damage;
      bool fixed_damage_defined;
      int severtype;
      bool severtype_defined;
   } critical;
   
   struct firest
   {
      //firest();   
      int chance;
      int chance_causes_debris;
   } fire;

};

struct t_weapon {

	struct t_weapon_type* type;

	char* loaded_cliptype;
	int ammo;
};

const char* get_loaded_cliptypename(struct t_weapon* weapon);
struct t_attackst* get_attack(struct t_weapon* w, bool force_ranged, bool force_melee, bool force_no_reload);

const char* get_weapon_name(struct t_weapon* weapon, int what);

#endif
