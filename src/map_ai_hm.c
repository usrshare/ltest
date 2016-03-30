// vim: cin:sts=4:sw=4 
#include "map_ai_hm.h"

#include "mapmode.h"
#include "pqueue.h"

#include <string.h>

// -- these functions use heatmaps to determine where the player may hide.
// the idea has been shamelessly and badly stolen from 
// http://www.roguebasin.com/index.php?title=Smart_searching_and_Modeling_the_player_with_a_%22heatmap%22

int heatmap_exists(uint8_t* hm, uint8_t x, uint8_t y) {

    uint16_t yx = y * MAP_WIDTH + x;

    return hm[yx];
}

int heatmap_add(uint8_t* hm, uint8_t x, uint8_t y, uint8_t v) {

    hm[ y * MAP_WIDTH + x] = v;
    return 0;
}
int heatmap_reset(uint8_t* hm) {

    memset(hm,2,sizeof(uint8_t) * HEATMAP_SIZE);
    return 0;
}

int heatmap_clear_solid(struct t_map* map, uint8_t* hm) {

    for (int i = 0; i < HEATMAP_SIZE; i++)
	if (tflags[map->sq[i].type] & TF_SOLID) hm[i] = 0;
}

int heatmap_clear(uint8_t* hm) {

    memset(hm,0,sizeof(uint8_t) * HEATMAP_SIZE);
    return 0;
}

int _find_iterate_line (uint8_t sx, uint8_t y, uint8_t* heatmap) {

    for (int dx=0; dx < MAP_WIDTH; dx++) {

	if (vtile(sx-dx,y) && heatmap[y * MAP_WIDTH + (sx-dx)] ) return (sx-dx);
	if (vtile(sx+dx,y) && heatmap[y * MAP_WIDTH + (sx+dx)] ) return (sx+dx);
    }

    return -1;
}

int find_closest_on_hm_with_path(struct t_map* map, uint8_t sx, uint8_t sy, uint8_t* heatmap, uint8_t* viewarr, uint8_t* o_x, uint8_t* o_y) {

    uint16_t temp_patharr [ HEATMAP_SIZE ];
    memset(temp_patharr, 255, sizeof temp_patharr);

    temp_patharr [sy * MAP_WIDTH + sx] = 0;

    struct pqueue_t* pq = pq_create();

    pq_push(pq, (void*)(sy * MAP_WIDTH + sx + 1),0); // create a queue, set the source area to zero.

    //assumes the rest is already filled with 0xFFFF

    while (pq_size(pq)) { // while not empty

	int yx = (int)(pq_pop(pq,NULL)) - 1;
	int x = (yx % MAP_WIDTH);
	int y = (yx / MAP_WIDTH);

	if (heatmap[yx]) { pq_destroy(pq); *o_y = y; *o_x = x; return 0; }

	int dir=0; //x and y now contain element with lowest priority

	for (dir=0; dir < MD_COUNT; dir++) { // for all neighbors

	    int nx = x + movediff[dir][0];
	    int ny = y + movediff[dir][1];

	    if (!vtile(nx,ny)) continue; //this should be a valid tile!

	    int cost = getcost(map,NULL,nx,ny,viewarr);
	    if (cost == -1) continue;

	    if (dir % 2) {
		cost = (cost * 3) / 2;
	    }

	    int alt = temp_patharr[y * MAP_WIDTH + x] + cost;
	    if (alt < temp_patharr[ny * MAP_WIDTH + nx]) {

		temp_patharr[ny * MAP_WIDTH + nx] = alt;

		pq_push (pq, (void*) (ny * MAP_WIDTH + nx + 1), alt);			
	    }

	}
    }

    pq_destroy(pq);

    *o_x = 255;
    *o_y = 255;

    return 0;	
}

int find_closest_on_heatmap(uint8_t sx, uint8_t sy, uint8_t* heatmap, uint8_t* o_x, uint8_t* o_y) {

    for ( int dy = 0; dy < MAP_HEIGHT; dy++) {

	if (vtile(0,sy-dy)) {
	    int r = _find_iterate_line(sx,(sy-dy),heatmap);
	    if (r != -1) {*o_x = r; *o_y = (sy-dy); return 0;}
	}
	if (vtile(0,sy+dy)) {
	    int r = _find_iterate_line(sx,(sy+dy),heatmap);
	    if (r != -1) {*o_x = r; *o_y = (sy+dy); return 0;}
	}
    }

    *o_x = 255;
    *o_y = 255;

    return 0;	
}

bool iterate_heatmap(uint8_t* hm, uint8_t* x, uint8_t* y) {

    int yx = (*y) * MAP_WIDTH + (*x);

    for (int i = yx; i < HEATMAP_SIZE; i++) {

	if (hm[i]) {*y = (i / MAP_WIDTH); *x = (i % MAP_WIDTH); return true; }
    }
    return false;
}

int spread_heatmap(struct t_map* map, uint8_t x, uint8_t y, uint8_t* heatmap) {

    enum movedirections md = 0;

    uint8_t dx = x, dy = y;

    int r = 0;

    for (md = 0; md < MD_COUNT; md++) {
	dx = x + movediff[md][0];
	dy = y + movediff[md][1];
	if ( (vtile(dx,dy)) && ((tflags[map->sq[dy * MAP_WIDTH + dx].type] & TF_SOLID) == 0) && (heatmap_exists(heatmap,dx,dy) == 0) ) heatmap_add(heatmap,dx,dy,2); //new tile.
    }

    heatmap_add(heatmap,x,y,1);

    return 0;
}
int update_heatmap(struct t_map* map, uint8_t* hm, uint8_t x, uint8_t y) {

    int r = 0;

    //if (can_see_entity(map,me,target)) heatmap[(target->y) * MAP_WIDTH + (target->x)] = 2; //new tile

    uint8_t h_add [ HEATMAP_SIZE]; memset(h_add,0,sizeof(uint8_t) * HEATMAP_SIZE);

    if ( vtile(x,y)&& ((tflags[map->sq[y * MAP_WIDTH + x].type] & TF_SOLID) == 0) ) { r = heatmap_add(hm,x,y,2); if (r != 0) return 1; }

    for (int i=0; i < HEATMAP_SIZE; i++) {

	if (hm[i] == 2) {

	    uint8_t y = (i / MAP_WIDTH);
	    uint8_t x = (i % MAP_WIDTH);

	    hm[i] = 1;

	    r = spread_heatmap(map,x,y,hm); if (r != 0) return 1;

	}
    }

    for (int i=0; i < HEATMAP_SIZE; i++) {
	if ( hm[i] && (map->aidata.e_viewarr[i] >= 3) ) hm[i] = 0;
    }	

    return 0;
}

// -- end of heatmap functions
