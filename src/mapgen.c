#include "mapgen.h"
#include "mapmode.h"
#include "random.h"

int iterate_rooms(struct t_map* map, int x, int y, int w, int h) {

	


}

int goDir(int* x, int* y, enum directions d) {

	switch (d) {
		case D_NORTH: *y--; return 0;
		case D_EAST: *x++; return 0;
		case D_SOUTH: *y++; return 0;
		case D_WEST: *x--; return 0;
	}
}

enum directions cwDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_EAST;
		case D_EAST: return D_SOUTH;
		case D_SOUTH: return D_WEST;
		case D_WEST: return D_NORTH;
	}
}

enum directions ccwDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_WEST;
		case D_EAST: return D_NORTH;
		case D_SOUTH: return D_EAST;
		case D_WEST: return D_SOUTH;
	}
}

enum directions opDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_SOUTH;
		case D_EAST: return D_WEST;
		case D_SOUTH: return D_NORTH;
		case D_WEST: return D_EAST;
	}
}

int surround_with_walls(struct t_map* map, int x, int y) {

	//surround current terraintype with walls using a flood-fill like algorithm.

	enum terraintypes tt = map->sq[y*MAP_WIDTH+x].type;
	
	if ((map->sq[(y-1)*MAP_WIDTH+x].type) != tt) map->sq[(y-1)*MAP_WIDTH+x].type = TT_WALL; else surround_with_walls(map,x,y-1);
	if ((map->sq[(y+1)*MAP_WIDTH+x].type) != tt) map->sq[(y+1)*MAP_WIDTH+x].type = TT_WALL; else surround_with_walls(map,x,y+1);

	if ((map->sq[y*MAP_WIDTH+(x-1)].type) != tt) map->sq[y*MAP_WIDTH+(x-1)].type = TT_WALL; else surround_with_walls(map,x-1,y);
	if ((map->sq[y*MAP_WIDTH+(x+1)].type) != tt) map->sq[y*MAP_WIDTH+(x+1)].type = TT_WALL; else surround_with_walls(map,x+1,y);
	
	if ((map->sq[(y-1)*MAP_WIDTH+(x-1)].type) != tt) map->sq[(y-1)*MAP_WIDTH+(x-1)].type = TT_WALL;
        if ((map->sq[(y-1)*MAP_WIDTH+(x+1)].type) != tt) map->sq[(y+1)*MAP_WIDTH+(x+1)].type = TT_WALL;
	if ((map->sq[(y+1)*MAP_WIDTH+(x-1)].type) != tt) map->sq[(y+1)*MAP_WIDTH+(x-1)].type = TT_WALL;
        if ((map->sq[(y+1)*MAP_WIDTH+(x+1)].type) != tt) map->sq[(y+1)*MAP_WIDTH+(x+1)].type = TT_WALL;
                                                                                               
}

int make_corridor(struct t_map* map, int x, int y, int w, int h) {



}

int fill_rect(struct t_map* map, int mw, int x, int y, int w, int h, enum terraintypes tt) {

	for (int iy=y; iy < y+h; iy++)
		for (int ix=x; iy < x+w; ix++)
			map->sq[iy*mw+ix].type = tt;
}	

int generate_buildings(struct t_map* map, enum generate_modes gm) {

	if (gm == GM_RANDOM) gm = GM_SINGLE + randval(GM_ELEMENT_COUNT-1);

		

}
