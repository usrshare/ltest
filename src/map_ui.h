// vim: cin:sts=4:sw=4 
#ifndef MAP_UI_H
#define MAP_UI_H
#include <stdbool.h>
#include <unistd.h>
#include "mapdefs.h"

int draw_map(struct t_map* map, struct t_map_entity* persp, bool show_fov, bool show_targets, bool show_heatmaps, bool hl_persp);
int mapgetch();
int nc_beep(void);
int updheader();
int msgprintw(const char *fmt, ...);
int msgsay(struct t_map_entity* me,const char *fmt, ...);
enum movedirections askdir();

int update_status (struct t_map* map);
int update_ui (struct t_map* map);
int map_ui_init(struct t_map* map);
int map_ui_free(struct t_map* map);

#endif
