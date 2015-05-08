#include "mapgen.h"
#include "mapmode.h"
#include "random.h"

#include <string.h> //memset

// TODO name and TODO description
// Copyright (C) 2015 usr_share
// Parts Copyright (c) 2002,2003,2004 by Tarn Adams  
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// ---------------------------------------------------------------
//
// The code of this particular function was inspired by the generateroom()
// function from sitemode/sitemap.cpp of "Liberal Crime Squad"
// (http://sourceforge.net/p/lcsgame)

int wall_border(struct t_map* map, int x, int y, int w, int h) {

	for (int ix=x; ix < (x+w); ix++) {
		map->sq[y*MAP_WIDTH+ix].type = TT_WALL;
		map->sq[(y+h-1)*MAP_WIDTH+ix].type = TT_WALL;
	}
	
	for (int iy=y; iy < (y+h); iy++) {
		map->sq[iy*MAP_WIDTH+x].type = TT_WALL;
		map->sq[iy*MAP_WIDTH+(x+w-1)].type = TT_WALL;
	}
}

int largest_room(struct t_map* map, int mx, int my, int vertical_lookup, int* w, int* h) {


}

int create_room(struct t_map* map, int x, int y, int w, int h) {

/*
	wall_border(map,x,y,w,h);

	int dir = randval(4); //random direction

	int mx = x + (w/2);
	int my = y + (h/2);

	int dist = randbetween(3,6);

	int nx = mx, ny = my;

	switch(dir) {
		case D_NORTH: ny -= dist;
		case D_SOUTH: ny += dist;
		case D_WEST: nx -= dist;
		case D_EAST: nx += dist;
	}

	if (nx < 0) { int d = -nx; nx += d; nw -= d; }
	if (ny < 0) { int d = -ny; ny += d; nh -= d; }
		
	if ((nx + nw) > MAP_WIDTH) { int d = (nx+nw) - MAP_WIDTH; nw -= d; }
	if ((ny + nh) > MAP_HEIGHT) { int d = (ny+nh) - MAP_HEIGHT; nh -= d; }

	if ((nw <= 2) || (nh <= 2)) return 0;

	if ( map->sq[ny*MAP_WIDTH+nx].type != TT_OUTSIDE) return 0;

	create_room(map,nx,ny,nw,nh);
*/
}

int iterate_rooms_2(struct t_map* map, int x, int y, int w, int h) {

	if ((w <= 2) || (h <= 2)) return 0;	
	int width = randbetween(1,5);
	
	if ( (randval(3) == 0) && ((width < w) && (width < h)) ) {
		
		// do a corridor

		if (randval(w + h) > w) {

			int cor_y = randbetween(y, y + h - width);

			for (int ix=x; ix < (x+w); ix++) {
				map->sq[(cor_y - 1)*MAP_WIDTH+ix].type = TT_WALL;
				for (int iy = cor_y; iy < (cor_y + width); iy++) map->sq[iy*MAP_WIDTH+ix].type = TT_CORRIDOR;

				map->sq[(cor_y + width)*MAP_WIDTH+ix].type = TT_WALL;

			}

			iterate_rooms_2(map,x,y,w,cor_y - 1- y);
			iterate_rooms_2(map,x,cor_y+width+1,w,y + h - cor_y - width - 1);

			//horizontal

		} else {
			
			int cor_x = randbetween(x, x + w - width);

			for (int iy=y; iy < (y+h); iy++) {
				map->sq[iy*MAP_WIDTH+(cor_x-1)].type = TT_WALL;
				for (int ix = cor_x; ix < (cor_x + width); ix++) map->sq[iy*MAP_WIDTH+ix].type = TT_CORRIDOR;
				map->sq[iy*MAP_WIDTH+(cor_x + width)].type = TT_WALL;

			}
		
			iterate_rooms_2(map,x,y,cor_x - 1 - x,h);
			iterate_rooms_2(map,cor_x+width+1,y,x + w - cor_x - width - 1,h);

			// vertical
		}
		
		
	} else {
	
	if (((w * h) <= 12) && (randval(2) == 0)) return 0;
	if ((w <= 2) || (h <= 2)) return 0;

	if ( ( (randval(w + h) < w) || h <= 3) && w > 2) {

		int div_x = x + randval(w - 2) + 1;
		for (int iy=y; iy < (y+h); iy++) {
			map->sq[iy*MAP_WIDTH+div_x].type = TT_WALL;
		}
		if (w > 1) {
			int door_y = y + randval(h);
			map->sq[door_y*MAP_WIDTH+div_x].type = TT_DOOR; }
		iterate_rooms_2(map,x,y,div_x - x,h);
		iterate_rooms_2(map,div_x+1,y,x + w - div_x - 1,h);
	} else {
		int div_y = y + randval(h - 2) + 1;
		for (int ix=x; ix < (x+w); ix++) {
			map->sq[div_y*MAP_WIDTH+ix].type = TT_WALL;
		}
		if (h > 1) {
			int door_x = x + randval(w);
			map->sq[div_y*MAP_WIDTH+door_x].type = TT_DOOR; }
		iterate_rooms_2(map,x,y,w,div_y - y);
		iterate_rooms_2(map,x,div_y+1,w,y + h - div_y - 1);
	}
	return 0;

	}


}

int iterate_rooms(struct t_map* map, int x, int y, int w, int h) {


	if (((w * h) <= 12) && (randval(2) == 0)) return 0;
	if ((w <= 3) && (h <= 3)) return 0;

	for (int iy=y; iy< (y+h); iy++)
		for (int ix=x; ix< (x+w); ix++)
			map->sq[iy*MAP_WIDTH+ix].type = TT_SPACE;


	if (( (randval(w + h) < w) || h <= 3) && w > 3) {

		int div_x = x + randval(w - 2) + 1;
		for (int iy=y; iy < (y+h); iy++) {
			map->sq[iy*MAP_WIDTH+div_x].type = TT_WALL;
		}
			int door_y = y + randval(h);
			map->sq[door_y*MAP_WIDTH+div_x].type = TT_DOOR;
		iterate_rooms(map,x,y,div_x - x,h);
		iterate_rooms(map,div_x+1,y,x + w - div_x - 1,h);
	} else {
		int div_y = y + randval(h - 2) + 1;
		for (int ix=x; ix < (x+w); ix++) {
			map->sq[div_y*MAP_WIDTH+ix].type = TT_WALL;
		}
			int door_x = x + randval(w);
			map->sq[div_y*MAP_WIDTH+door_x].type = TT_DOOR; 
		iterate_rooms(map,x,y,w,div_y - y);
		iterate_rooms(map,x,div_y+1,w,y + h - div_y - 1);
	}
	return 0;
}

// ---------------------------------------------------------------

int goDir(int* x, int* y, enum directions d) {

	switch (d) {
		case D_NORTH: (*y)--; return 0;
		case D_EAST: (*x)++; return 0;
		case D_SOUTH: (*y)++; return 0;
		case D_WEST: (*x)--; return 0;
		default: return 1;
	}
}

enum directions cwDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_EAST;
		case D_EAST: return D_SOUTH;
		case D_SOUTH: return D_WEST;
		case D_WEST: return D_NORTH;
		default: return D_NORTH;
	}
}

enum directions ccwDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_WEST;
		case D_EAST: return D_NORTH;
		case D_SOUTH: return D_EAST;
		case D_WEST: return D_SOUTH;
		default: return D_NORTH;
	}
}

enum directions opDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_SOUTH;
		case D_EAST: return D_WEST;
		case D_SOUTH: return D_NORTH;
		case D_WEST: return D_EAST;
		default: return D_NORTH;
	}
}



int surround_iter(struct t_map* map, int x, int y, int* done_array) {

	//surround current terraintype with walls using a flood-fill like algorithm.

	if (done_array[y*MAP_WIDTH+x] == 1) return 0;
	done_array[y*MAP_WIDTH+x]=1;

	enum terraintypes tt = map->sq[y*MAP_WIDTH+x].type;

	if ((map->sq[(y-1)*MAP_WIDTH+x].type) != tt) (map->sq[(y-1)*MAP_WIDTH+x].type) = TT_WALL; else surround_iter(map,x,y-1,done_array);
	if ((map->sq[(y+1)*MAP_WIDTH+x].type) != tt) (map->sq[(y+1)*MAP_WIDTH+x].type) = TT_WALL; else surround_iter(map,x,y+1,done_array);

	if ((map->sq[y*MAP_WIDTH+(x-1)].type) != tt) (map->sq[y*MAP_WIDTH+(x-1)].type) = TT_WALL; else surround_iter(map,x-1,y,done_array);
	if ((map->sq[y*MAP_WIDTH+(x+1)].type) != tt) (map->sq[y*MAP_WIDTH+(x+1)].type) = TT_WALL; else surround_iter(map,x+1,y,done_array);

	if ((map->sq[(y-1)*MAP_WIDTH+(x-1)].type) != tt) map->sq[(y-1)*MAP_WIDTH+(x-1)].type = TT_WALL;
	if ((map->sq[(y-1)*MAP_WIDTH+(x+1)].type) != tt) map->sq[(y-1)*MAP_WIDTH+(x+1)].type = TT_WALL;
	if ((map->sq[(y+1)*MAP_WIDTH+(x-1)].type) != tt) map->sq[(y+1)*MAP_WIDTH+(x-1)].type = TT_WALL;
	if ((map->sq[(y+1)*MAP_WIDTH+(x+1)].type) != tt) map->sq[(y+1)*MAP_WIDTH+(x+1)].type = TT_WALL;


	return 0;                                                                                               
}

int surround_with_walls(struct t_map* map, int x, int y) {

	int done_array[MAP_HEIGHT*MAP_WIDTH];
	memset(done_array, 0, sizeof(int) * MAP_HEIGHT * MAP_WIDTH);
	surround_iter(map,x,y,done_array);

	return 0;
}



int make_corridor(struct t_map* map, int x, int y, int w, int h) {

	for (int iy=y; iy<(y+h); iy++)
		for (int ix=x; ix<(x+w); ix++)
			map->sq[iy*MAP_WIDTH+ix].type=TT_CORRIDOR;

	return 0;
}

int fill_rect(struct t_map* map, int x, int y, int w, int h, enum terraintypes tt) {

	for (int iy=y; iy < y+h; iy++)
		for (int ix=x; ix < x+w; ix++)
			map->sq[iy*MAP_WIDTH+ix].type = tt;

	return 0;
}	

int generate_buildings(struct t_map* map, enum generate_modes gm) {

	if (gm == GM_RANDOM) gm = randbetween (GM_SINGLE, GM_ELEMENT_COUNT-1);

	switch(gm) {

		case GM_LIBERAL: {

					 fill_rect(map,1,1,MAP_WIDTH-2,MAP_HEIGHT-2,TT_SPACE);
					 surround_with_walls(map,MAP_WIDTH/2,MAP_HEIGHT/2);
					 iterate_rooms(map,1,1,MAP_WIDTH-2,MAP_HEIGHT-2);
					 break; }
		case GM_SINGLE: {
					 //fill_rect(map,0,0,MAP_WIDTH-1,MAP_HEIGHT-1,TT_OUTSIDE);
					 //create_room(map,35,5,10,10);	
					 fill_rect(map,1,1,MAP_WIDTH-2,MAP_HEIGHT-2,TT_SPACE);
					 surround_with_walls(map,MAP_WIDTH/2,MAP_HEIGHT/2);
					 iterate_rooms_2(map,1,1,MAP_WIDTH-2,MAP_HEIGHT-2);
					
			
					break; }
		default: {
				 break; }

	}
	return 0;
}
