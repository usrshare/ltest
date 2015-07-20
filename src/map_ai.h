// vim: cin:sts=4:sw=4 
#ifndef MAP_AI_H
#define MAP_AI_H

#include "mapdefs.h"

#define HEATMAP_SIZE 256

enum aitasks {

    AIT_WORKING, // for non-hostile NPCs doing "their regular jobs"
    AIT_PATROLLING, // for security/police NPCs
    AIT_PLEASE_LEAVE, // asking the target to leave the area
    AIT_CHECKING_OUT,
    AIT_PURSUING,   // when the AI sees the target directly
    AIT_ATTACKING,
    AIT_LOOKING_FOR,// and when it doesn't.
    AIT_FLEEING,
};

struct t_map_ai_data {

    // this one is for the entire map.
    
    // VIEWARR FORMAT:
    // 0 = never seen this square.
    // 1 = seen it on a map, but not directly.
    // 2 = seen it before, but not right now.
    // 3 = seeing it right now.
    // 4 = seeing it right now, and there's an entity there.

    // player / allies
    uint8_t p_viewarr [ MAP_WIDTH * MAP_HEIGHT ];
    uint16_t p_patharr [ MAP_WIDTH * MAP_HEIGHT ];
    enum movedirections p_pathprev[ MAP_WIDTH * MAP_HEIGHT ];


    // enemies / neutrals
    uint8_t e_viewarr [ MAP_WIDTH * MAP_HEIGHT ];
    uint16_t e_patharr [ MAP_WIDTH * MAP_HEIGHT ];
    enum movedirections e_pathprev[ MAP_WIDTH * MAP_HEIGHT ];
    
    uint16_t e_heatmap_old [ HEATMAP_SIZE ]; //storing coords as (y * MAP_WIDTH + x).
    uint16_t e_heatmap_new [ HEATMAP_SIZE ]; //65535 stands for "empty".

    struct t_map_entity* targets [SQUAD_MAX]; //the known squad members 

};

struct t_ent_ai_data {

    // this one is for specific entities.

    // this structure is deprecated. sooner or later, most arrays from
    // this structure will be replaced by ones from t_map_ai_data.

    struct t_map_entity* usedby;
    uint8_t /*enum movedirections*/ viewdir;
    bool wideview;
    bool show_fov;

    bool viewarr_updated;

    enum aitasks task;
    int path_plotted;

    uint8_t dx, dy;
    uint8_t lx, ly; //last x and y

    uint8_t alert_state;
    uint8_t alertmsg_state;
    struct t_map_entity* target; //target, if not alerted yet.

    uint16_t crimes;

    uint16_t timer;

    // array for pathfinding

};

extern bool needs_ai [ET_COUNT];

extern struct t_ent_ai_data aient[MAX_AI_ENTITIES];

uint16_t enemy_seeFunc(struct t_map* map, struct t_map_entity* me, uint8_t y, uint8_t x, struct t_map_entity* whom);

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me);
uint16_t enemy_actFunc(struct t_map* map, struct t_map_entity* me);

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me);
uint16_t player_actFunc(struct t_map* map, struct t_map_entity* me);

int heatmap_clear(uint16_t* hm);

#endif
