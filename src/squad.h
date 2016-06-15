// vim: cin:sts=4:sw=4 
#ifndef SQUAD_H
#define SQUAD_H

#include "entity.h"

#include <stdbool.h>

#define SQUAD_NAMELEN 40
#define SQUAD_MAXITEMS 60

#define SQUAD_MAXSIZE 6

struct t_squad {
    char name [SQUAD_NAMELEN];
    struct t_creature *member[SQUAD_MAXSIZE];
    //struct t_activity activity;
    int id;
    //vector <Item*> loot;

    char stance;
};


bool squad_alive(struct t_squad* sq);

int squad_add(struct t_squad* sq, struct t_creature* cr);
int squad_del(struct t_squad* sq, struct t_creature* cr);
int squad_remove_dead( struct t_squad* sq);

#endif
