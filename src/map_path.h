// vim: cin:sts=4:sw=4 
#ifndef MAP_PATH_H
#define MAP_PATH_H

#include "map_ai.h"
enum movedirections plot_follow(uint8_t x, uint8_t y, enum movedirections* pathprev);

struct plotflags {
    struct t_map_entity* persp; //for line of sight and TODO locked doors
    bool eightdir; //plot in all 8 directions instead of 4.
    bool no_clear_patharr; // don't clear patharr. useful for setting multiple targets.
    uint8_t mx; //minimum x
    uint8_t my;
    uint8_t w;
    uint8_t h;
    uint16_t* extra_targets; //extra target points.
};

extern const struct plotflags plot_default;

int plot_dijkstra_map(struct t_map* map, uint8_t* viewarr, uint16_t* o_patharr, bool invert, const struct plotflags* _pf);
enum movedirections roll_downhill(uint16_t* patharr, uint8_t sx, uint8_t sy);

//int plot_path(struct t_map* map, uint8_t dx, uint8_t dy, uint8_t* viewarr, uint16_t* o_patharr, enum movedirections* o_pathprev, const struct plotflags* _pf);

#endif
