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
    .persp = NULL,
    .eightdir = true
};

enum movedirections plot_follow(uint8_t x, uint8_t y, enum movedirections* pathprev) {
	
	if (pathprev[y * MAP_WIDTH + x] < MD_COUNT) return (4 + pathprev[y * MAP_WIDTH + x]) % 8; else return MD_COUNT;
}

int getcost(struct t_map* map, struct t_map_entity* who, uint8_t x, uint8_t y, uint8_t* viewarr) {
		
	bool tilevis = (who ? viewarr[y * MAP_WIDTH + x] : 1);
	enum terraintypes tt = map->sq[y * MAP_WIDTH + x].type; 

	uint8_t tilecost;

	if (tilevis == 0) tilecost = 4; else tilecost = costs[tt];
	if (tilecost == 255) return -1; else return tilecost;		
}

int plot_path(struct t_map* map, uint8_t dx, uint8_t dy, uint8_t* viewarr, uint16_t* o_patharr, enum movedirections* o_pathprev, const struct plotflags* _pf) {

	const struct plotflags* pf = (_pf ? _pf : &plot_default);
	
	int dydx = (dy * MAP_WIDTH + dx);	

	if (pf->no_clear_patharr == false) {	
	memset(o_patharr,-1,sizeof(uint16_t) * MAP_WIDTH * MAP_HEIGHT);
	o_patharr[dydx] = 0;
	}
	memset(o_pathprev,-1,sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
	

	struct pqel pathqueue[MAP_WIDTH * MAP_HEIGHT]; 
	memset(pathqueue,0,sizeof(struct pqel) * MAP_WIDTH * MAP_HEIGHT);

	pq_add_element(pathqueue,(MAP_WIDTH * MAP_HEIGHT), (void*)(intptr_t)(dydx), 0);
	int pqels = 1;

	while (pqels) {

		intptr_t xy = (intptr_t) pq_get_lowest(pathqueue, (MAP_WIDTH * MAP_HEIGHT)); pqels--;
		int x = (xy % MAP_WIDTH);
		int y = (xy / MAP_WIDTH);

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
			o_pathprev[ny * MAP_WIDTH + nx] = dir;

			int r = pq_decrease_or_add(pathqueue,(MAP_WIDTH * MAP_HEIGHT),(void*)(intptr_t)(ny * MAP_WIDTH + nx), alt); if (r == 2) pqels++;

		}

	}
	}
	return 0;
}
