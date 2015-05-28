#include "armor.h"

const struct t_armor_type armortypes[ARMOR_COUNT] = {
	{ //ARMOR_NONE
		.name = "None (naked)",
	},
};

bool armor_decrease_quality(struct t_armor* armor, int decrease)
{
   armor->quality+=decrease;
   if(armor->quality<1) armor->quality=1;
   return armor->quality<=armor->type->quality_levels;
}
