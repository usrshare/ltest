#ifndef MAPGEN_H
#define MAPGEN_H

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

	GM_LIBERAL, //"Liberal Crime Squad" algorithm	
	GM_RANDOM,
	GM_SINGLE,
	GM_LSHAPED,
	GM_TSHAPED,
	GM_MULTIPLE,
	GM_COMPLEX,
	GM_ELEMENT_COUNT
};

#endif
