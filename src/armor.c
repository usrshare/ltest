#include "armor.h"

const struct t_armor_type armortypes[ARMOR_COUNT] = {
	{ //ARMOR_NONE
		.name = "None (naked)",
	},
};

const struct t_armor_type* a_type(struct t_item* armor) {
	if (armor->type != IT_ARMOR) return &armortypes[ARMOR_NONE];
	return &armortypes[armor->itemtypeid];
}

int armor_get_quality(struct t_item* armor) {
	return armor->a_flags & AD_QUALITY;
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

int give_armor(struct t_creature* cr, struct t_item* armor) {

	struct t_item* invspace = inv_find_empty(cr->inventory);
	if (!invspace) return 1;

	*invspace = *armor; //copy physically!
	cr->armor = invspace;
	
	return 0;
}
