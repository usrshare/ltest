#ifndef FIGHT_H
#define FIGHT_H

#include "entity.h"
#include "mapdefs.h"

void attack(struct t_map* map, struct t_creature* a,struct t_creature* t,char mistake,char* actual,bool force_melee);

#endif
