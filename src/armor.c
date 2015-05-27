#include "armor.h"

bool armor_decrease_quality(struct t_armor* armor, int decrease)
{
   armor->quality+=decrease;
   if(armor->quality<1) armor->quality=1;
   return armor->quality<=armor->type->quality_levels;
}
