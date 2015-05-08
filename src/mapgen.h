#ifndef MAPGEN_H
#define MAPGEN_H

enum directions {
	D_NORTH,
	D_EAST,
	D_SOUTH,
	D_WEST};

enum generate_modes {
	
	GM_RANDOM,
	GM_SINGLE,
	GM_LSHAPED,
	GM_TSHAPED,
	GM_MULTIPLE,
	GM_COMPLEX,
	GM_ELEMENT_COUNT
};

#endif
