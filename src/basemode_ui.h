// vim: cin:sts=4:sw=4 
#ifndef BASEMODE_UI_H
#define BASEMODE_UI_H

#include "squad.h"

int menugetch();

int update_squad_stats(struct t_squad* squad);
int draw_basemode_menu(); 
int update_basemode(struct t_squad* squad);

int basemode_ui_init();
int basemode_ui_free();

#endif
