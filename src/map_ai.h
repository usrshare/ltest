// vim: cin:sts=4:sw=4 
#ifndef MAP_AI_H
#define MAP_AI_H

#include "mapdefs.h"
#include "map_ai_hm.h"

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

typedef uint16_t (*customFunc)(struct t_map* map, struct t_map_entity* me);

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

    // enemies / neutrals
    uint8_t e_viewarr [ MAP_WIDTH * MAP_HEIGHT ];
    // enemies are assumed to have perfect knowledge of the map
    // and/or have radios to communicate.

    uint8_t e_hm [ MAP_WIDTH * MAP_HEIGHT ]; //0 = none, 1 = old, 2 = new.

    struct t_map_entity* targets [SQUAD_MAX]; //the known squad members
    uint8_t targets_x [SQUAD_MAX]; //known x positions of targets
    uint8_t targets_y [SQUAD_MAX]; //known y positions of targets
    
    bool targets_moved; //whether the targets moved / plots need refreshing.

    uint16_t e_targets_arr [ MAP_WIDTH * MAP_HEIGHT ]; //pursuing targets.

    uint16_t e_flee_arr [ MAP_WIDTH * MAP_HEIGHT ]; //fleeing from the targets.

    uint16_t e_explore_timer [ MAP_WIDTH * MAP_HEIGHT];
    //have these areas been neglected for too long?
    
    uint16_t e_explore_arr [ MAP_WIDTH * MAP_HEIGHT];

    customFunc turnFunc;
    customFunc actFunc;

};

int find_empty_target (struct t_map_ai_data* data); //returns an index from 0 to 5 or -1.

struct t_ent_ai_data {

    // this one is for specific entities.

    // this structure is deprecated. sooner or later, most arrays from
    // this structure will be replaced by ones from t_map_ai_data.

    struct t_map_entity* usedby;
    enum movedirections viewdir;
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
    
    uint16_t ent_target_arr [ MAP_WIDTH * MAP_HEIGHT ]; //single array for single target.

    // array for pathfinding

};

extern bool needs_ai [ET_COUNT];

extern struct t_ent_ai_data aient[MAX_AI_ENTITIES];

void map_ai_init (struct t_map* map);

uint16_t enemy_seeFunc(struct t_map* map, struct t_map_entity* me, uint8_t y, uint8_t x, struct t_map_entity* whom);

void map_attack (struct t_map* map, struct t_map_entity* a, uint8_t dx, uint8_t dy);

uint16_t enemy_turnFunc(struct t_map* map, struct t_map_entity* me);
uint16_t enemy_actFunc(struct t_map* map, struct t_map_entity* me);

uint16_t player_turnFunc(struct t_map* map, struct t_map_entity* me);
uint16_t player_actFunc(struct t_map* map, struct t_map_entity* me);

int static_plot(struct t_map* map);

#endif
