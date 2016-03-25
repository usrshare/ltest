// vim: cin:sts=4:sw=4 
#ifndef MAP_UI_H
#define MAP_UI_H
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include "mapdefs.h"

int update_player_map(struct t_map* map, struct t_map_entity* player, int hl_player);

int draw_map(struct t_map* map, struct t_map_entity* persp, bool show_fov, bool show_targets, bool show_heatmaps, bool hl_persp);
int mapgetch();
int askpos(uint8_t* y, uint8_t* x);
int nc_beep(void);
int updheader();

int msgaddstr(const char *string);
int msgprintw(const char *fmt, ...);
int msgvprintw(const char *fmt, va_list ap);
int msgattrset(int attrs);
int msgsay(struct t_map_entity* me,const char *fmt, ...);

int mapaddch(const char *c, uint8_t y, uint8_t x);
int maprefresh(void);

enum movedirections askdir();

int update_status (struct t_map* map);
int update_ui (struct t_map* map);
int map_ui_init(struct t_map* map);
int map_ui_free(struct t_map* map);

#endif
