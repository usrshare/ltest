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

enum movedirections plot_follow(uint8_t x, uint8_t y, enum movedirections* pathprev) {
	
	if (pathprev[y * MAP_WIDTH + x] < MD_COUNT) return (4 + pathprev[y * MAP_WIDTH + x]) % 8; else return MD_COUNT;
}

int getcost(struct t_map* map, struct t_map_entity* who, uint8_t x, uint8_t y) {
		
	bool tilevis = (who->aidata->viewarr[y * MAP_WIDTH + x]);
	enum terraintypes tt = map->sq[y * MAP_WIDTH + x].type; 

	uint8_t tilecost;

	if (tilevis == 0) tilecost = 4; else tilecost = costs[tt];
	if (tilecost == 255) return -1; else return tilecost;		
}

int plot_path(struct t_map* map, struct t_map_entity* who, uint8_t dx, uint8_t dy, uint16_t* patharr, enum movedirections* pathprev) {
	
	memset(patharr,-1,sizeof(uint16_t) * MAP_WIDTH * MAP_HEIGHT);
	memset(pathprev,-1,sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
	
	int dydx = (dy * MAP_WIDTH + dx);	
	
	patharr[dydx] = 0;

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

		if (!vtile(nx,ny)) continue; //this should be a valid tile!
		
		int cost = getcost(map,who,nx,ny);
		if (cost == -1) continue;

		if (dir % 2) cost = (cost * 3) / 2;

		int alt = patharr[y * MAP_WIDTH + x] + cost;
		if (alt < patharr[ny * MAP_WIDTH + nx]) {

			patharr[ny * MAP_WIDTH + nx] = alt;
			pathprev[ny * MAP_WIDTH + nx] = dir;

			int r = pq_decrease_or_add(pathqueue,(MAP_WIDTH * MAP_HEIGHT),(void*)(intptr_t)(ny * MAP_WIDTH + nx), alt); if (r == 2) pqels++;

		}

	}
	}
	return 0;
}
