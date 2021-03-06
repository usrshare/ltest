// vim: cin:sts=4:sw=4 
#include "mapmode.h"
#include "map_path.h"
#include "pqueue.h"
#include <stdint.h>
#include <string.h> //memset

uint8_t costs[TT_ELEMENT_COUNT] = {

    4, //outside
    6, //grass
    4, //corrdior
    4, //space
    4, //restricted space
    4, //special space
    255, //wall
    20, //window
    12, //closed door
    4, //open door
    255, //bars
    255, //table
    4, //custom
    4, //stairs
    255, //unknown
};

const struct plotflags plot_default = {
    .limited = true,
    .persp = NULL,
    .eightdir = true
};

enum movedirections plot_follow(uint8_t x, uint8_t y, enum movedirections* pathprev) {

    if (pathprev[y * MAP_WIDTH + x] < MD_COUNT) return (4 + pathprev[y * MAP_WIDTH + x]) % 8; else return MD_COUNT;
}

int getcost(struct t_map* map, struct t_map_entity* who, uint8_t x, uint8_t y, uint8_t* viewarr) {

    bool tilevis = (viewarr ? viewarr[y * MAP_WIDTH + x] : 1);
    enum terraintypes tt = map->sq[y * MAP_WIDTH + x].type; 

    uint8_t tilecost;

    if (tilevis == 0) tilecost = 4; else tilecost = costs[tt];
    if (tilecost == 255) return -1; else return tilecost;		
}

enum movedirections roll_downhill(uint16_t* patharr, uint8_t sx, uint8_t sy) {

    enum movedirections leastdir = MD_COUNT; uint16_t leastval = UINT16_MAX;

    uint8_t dx, dy;

    for (int i=0; i < MD_COUNT; i++) {

	dx = sx + movediff[i][0]; dy = sy + movediff[i][1];

	if (!vtile(dx,dy)) continue;

	if (patharr[ dy * MAP_WIDTH + dx ] < leastval) {
	    leastval = patharr[ dy * MAP_WIDTH + dx] ;
	    leastdir = i;
	}
    }

    return leastdir;
}

int plot_dijkstra_map(struct t_map* map, uint8_t* viewarr, uint16_t* o_patharr, bool invert, const struct plotflags* _pf) {

    //this function is more generic than plot_path.
    //it accepts more than one target square (o_patharr == 0).
    //it allows to invert the search (plot paths away from targets). 

    const struct plotflags* pf = (_pf ? _pf : &plot_default);

    struct pqueue_t* pq = pq_create();

    for (int i=0; i < (MAP_WIDTH * MAP_HEIGHT); i++) {
	if (o_patharr[i] == 0) pq_push(pq, (void*) (i+1), 0); }

    while (pq_size(pq)) {

	int yx = (int)(pq_pop(pq,NULL)) - 1;
	int x = (yx % MAP_WIDTH);
	int y = (yx / MAP_WIDTH);

	int dir=0;

	for (dir=0; dir < MD_COUNT; dir++) {

	    int nx = x + movediff[dir][0];
	    int ny = y + movediff[dir][1];

	    if ((pf->w && pf->h) && ((nx < pf->mx) || (nx >= (pf->mx + pf->w)) || (ny < pf->my) || (ny >= (pf->my + pf->w)))) continue;

	    if (!vtile(nx,ny)) continue; //this should be a valid tile!

	    int cost = getcost(map,pf->persp,nx,ny,viewarr);
	    if (cost == -1) continue;

	    if (dir % 2) {
		if (pf->eightdir) continue;
		cost = (cost * 3) / 2;
	    }

	    int alt = o_patharr[y * MAP_WIDTH + x] + cost;
	    if (alt < o_patharr[ny * MAP_WIDTH + nx]) {

		o_patharr[ny * MAP_WIDTH + nx] = alt;

		pq_push (pq, (void*) (ny * MAP_WIDTH + nx + 1), alt);			
	    }

	}
    }

    pq_destroy(pq);
    return 0;
}

int plot_path(struct t_map* map, uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy, uint8_t* viewarr, uint16_t* o_patharr, bool invert, const struct plotflags* _pf) {

    const struct plotflags* pf = (_pf ? _pf : &plot_default);

    o_patharr[dy * MAP_WIDTH + dx] = 0;

    struct pqueue_t* pq = pq_create();

    pq_push(pq, (void*)(dy * MAP_WIDTH + dx + 1),0); // create a queue, set the source area to zero.

    //assumes the rest is already filled with 0xFFFF

    while (pq_size(pq)) { // while not empty

	int yx = (int)(pq_pop(pq,NULL)) - 1;
	int x = (yx % MAP_WIDTH);
	int y = (yx / MAP_WIDTH);

	if ((x == dx) && (y == dy)) { pq_destroy(pq); return 0; }

	int dir=0; //x and y now contain element with lowest priority

	for (dir=0; dir < MD_COUNT; dir++) { // for all neighbors

	    int nx = x + movediff[dir][0];
	    int ny = y + movediff[dir][1];

	    if (!vtile(nx,ny)) continue; //this should be a valid tile!
	    if ((pf->w && pf->h) && ((nx < pf->mx) || (nx >= (pf->mx + pf->w)) || (ny < pf->my) || (ny >= (pf->my + pf->w)))) continue;

	    int cost = getcost(map,pf->persp,nx,ny,viewarr);
	    if (cost == -1) continue;

	    if (dir % 2) {
		if (pf->eightdir) continue;
		cost = (cost * 3) / 2;
	    }

	    int alt = o_patharr[y * MAP_WIDTH + x] + cost;
	    if (alt < o_patharr[ny * MAP_WIDTH + nx]) {

		o_patharr[ny * MAP_WIDTH + nx] = alt;

		pq_push (pq, (void*) (ny * MAP_WIDTH + nx + 1), alt);			
	    }

	}
    }

    pq_destroy(pq);
    return 0;
}
