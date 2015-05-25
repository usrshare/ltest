// vim: cin:sts=4:sw=4 
#ifndef MAPGEN_H
#define MAPGEN_H
#include "mapdefs.h"

enum directions {
	D_NONE = -1,
	D_NORTH = 0,
	D_EAST,
	D_SOUTH,
	D_WEST};

enum dirflags {
	DF_NONE = 0,
	DF_NORTH = 1,
	DF_EAST = 2,
	DF_SOUTH = 4,
	DF_WEST = 8,
	DF_ALL = 15,
};

enum generate_modes {
	GM_EMPTY,
	GM_LIBERAL, //"Liberal Crime Squad" algorithm	
	GM_RANDOM,
	GM_SINGLE,
	GM_LSHAPED,
	GM_TSHAPED,
	GM_MULTIPLE,
	GM_COMPLEX,
	GM_ELEMENT_COUNT
};

enum recurse_behavior {
	RB_RANDOM,
	RB_ALWAYS,
	RB_RANDOMLY,
	RB_RANDOMLY_SAME,
	RB_ELEMENT_COUNT,
};

struct rect {
	int x;
	int y;
	int w;
	int h;
};

int generate_buildings(struct t_map* map, enum generate_modes gm);

#endif
