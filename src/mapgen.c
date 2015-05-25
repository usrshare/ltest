// vim: cin:sts=4:sw=4 
#include "mapgen.h"
#include "mapmode.h"
#include "pqueue.h"
#include "random.h"

#include "globals.h"

#include <malloc.h>
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

// --- directions and dirflags manipulation

#define ROOMQUEUE_SZ 256

enum roomstyles {

	RS_RANDOM,
	RS_OFFICE,
	RS_MEETING,
	RS_CUBICLES,
	RS_PERSONAL,
	RS_LOCKER,
	RS_WC,
	RS_COUNT
};

struct roomparams {
	uint8_t x;
	uint8_t y;
	uint8_t w;
	uint8_t h;
	enum roomstyles style;
};

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
		default: return D_NONE;
	}
}

enum directions ccwDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_WEST;
		case D_EAST: return D_NORTH;
		case D_SOUTH: return D_EAST;
		case D_WEST: return D_SOUTH;
		default: return D_NONE;
	}
}

enum directions opDir(enum directions d) {
	switch (d) {
		case D_NORTH: return D_SOUTH;
		case D_EAST: return D_WEST;
		case D_SOUTH: return D_NORTH;
		case D_WEST: return D_EAST;
		default: return D_NONE;
	}
}

enum dirflags dirFlag(enum directions d) {
	switch (d) {
		case D_NORTH: return DF_NORTH;
		case D_EAST: return DF_EAST;
		case D_SOUTH: return DF_SOUTH;
		case D_WEST: return DF_WEST;
		default: return DF_NONE;
	}
}

// ---

int no_door(struct t_map* map, int x, int y) {

	if (map->sq[(y+1)*MAP_WIDTH+x].type == TT_DOOR_CLOSED) return 1;
	if (map->sq[(y-1)*MAP_WIDTH+x].type == TT_DOOR_CLOSED) return 1;
	if (map->sq[y*MAP_WIDTH+(x-1)].type == TT_DOOR_CLOSED) return 1;
	if (map->sq[y*MAP_WIDTH+(x+1)].type == TT_DOOR_CLOSED) return 1;
	return 0;
}

int decorate_room(struct t_map* map, int x, int y, int w, int h, enum roomstyles style) {

	//TODO

	if (style == RS_RANDOM) style = randbetween(RS_RANDOM+1,RS_COUNT-1);

	switch (style) {
		case RS_OFFICE: {

			break; }

		case RS_MEETING: {
			bool vertical = (w < h);
			if ((w < 4) || (h < 4)) return 1;
			
			for (int iy=1; iy < (h-1); iy++)
			for (int ix=1; ix < (w-1); ix++)
				map->sq[(y +iy)*MAP_WIDTH+(x+ix)].type = TT_TABLE;

			break; }
		case RS_CUBICLES: {

			break; }
		case RS_PERSONAL: {

			break; }
		case RS_LOCKER: {

			int randdirs = randval(16);
			
			if ((w < 4) || (h < 4)) return 1;
			
			if (randdirs & DF_NORTH) {
				for (int l=0; l<w; l++) {
					if (no_door(map,x + l, y) == 0) map->sq[y * MAP_WIDTH + (x+l)].type = TT_LOCKER; }
			}
			if (randdirs & DF_SOUTH) {
				for (int l=0; l<w; l++) {
					if (no_door(map,x + l, y+h-1) == 0) map->sq[(y+h-1) * MAP_WIDTH + (x+l)].type = TT_LOCKER; }
			}
			if (randdirs & DF_WEST) {
				for (int l=0; l<h; l++) {
					if (no_door(map,x, y+l) == 0) map->sq[(y+l) * MAP_WIDTH + x].type = TT_LOCKER; }
			}
			if (randdirs & DF_EAST) {
				for (int l=0; l<h; l++) {
					if (no_door(map,x+w-1, y+l) == 0) map->sq[(y+l) * MAP_WIDTH + (x+w-1)].type = TT_LOCKER; }
			}


			break; }
		case RS_WC: {

			break; }
		default:
			break;
	}
	return 0;
}

int wall_border(struct t_map* map, int x, int y, int w, int h) {

	for (int ix=x; ix < (x+w); ix++) {
		map->sq[y*MAP_WIDTH+ix].type = TT_WALL;
		map->sq[(y+h-1)*MAP_WIDTH+ix].type = TT_WALL;
	}

	for (int iy=y; iy < (y+h); iy++) {
		map->sq[iy*MAP_WIDTH+x].type = TT_WALL;
		map->sq[iy*MAP_WIDTH+(x+w-1)].type = TT_WALL;
	}
	return 0;
}

int wall_border_2(struct t_map* map, int x, int y, int w, int h) { //doesn't replace doors!

	int doors = 0;

	for (int ix=x; ix < (x+w); ix++) {
		if (map->sq[y*MAP_WIDTH+ix].type != TT_DOOR_CLOSED) map->sq[y*MAP_WIDTH+ix].type = TT_WALL; else doors++;
		if (map->sq[(y+h-1)*MAP_WIDTH+ix].type != TT_DOOR_CLOSED) map->sq[(y+h-1)*MAP_WIDTH+ix].type = TT_WALL; else doors++;
	}

	for (int iy=y; iy < (y+h); iy++) {
		if (map->sq[iy*MAP_WIDTH+x].type != TT_DOOR_CLOSED) map->sq[iy*MAP_WIDTH+x].type = TT_WALL; else doors++;
		if (map->sq[iy*MAP_WIDTH+(x+w-1)].type != TT_DOOR_CLOSED) map->sq[iy*MAP_WIDTH+(x+w-1)].type = TT_WALL; else doors++;
	}

	return doors;
}
int fill_rect(struct t_map* map, int x, int y, int w, int h, enum terraintypes tt) {

	for (int iy=y; iy < y+h; iy++)
		for (int ix=x; ix < x+w; ix++)
			map->sq[iy*MAP_WIDTH+ix].type = tt;

	return 0;
}	

int okay_door (struct t_map* map, int x, int y, int l, enum directions dir, enum terraintypes tt1, enum terraintypes tt2, int* out_posarray) {

	int okaydoors = 0;

	enum terraintypes t1, t2;

	switch (dir) {

		case D_NORTH:
		case D_SOUTH: 

			for (int ix=x; ix < (x+l); ix++) {

				t1 = map->sq[ (y-1) * MAP_WIDTH + ix].type;
				t2 = map->sq[ (y+1) * MAP_WIDTH + ix].type;

				if (((t1 == tt1) && (t2 == tt2)) || ((t1 == tt2) && (t2 == tt1)) || ((tflags[t1] & TF_OKAY_DOOR) && (tflags[t2] & TF_OKAY_DOOR))  ) {

					if (out_posarray) out_posarray[okaydoors] = ix;
					okaydoors++;
				}

			}

			break;

		case D_EAST:
		case D_WEST:

			for (int iy=y; iy < (y+l); iy++) {

				t1 = map->sq[ iy * MAP_WIDTH + (x-1) ].type;
				t2 = map->sq[ iy * MAP_WIDTH + (x+1) ].type;

				if (((t1 == tt1) && (t2 == tt2)) || ((t1 == tt2) && (t2 == tt1)) || ((tflags[t1] & TF_OKAY_DOOR) && (tflags[t2] & TF_OKAY_DOOR))  ) {

					if (out_posarray) out_posarray[okaydoors] = iy;
					okaydoors++;
				}

			}

			break;
		case D_NONE:
		return 0;

	}

	return okaydoors;
}
int build_doors(struct t_map* map, int x, int y, int w, int h, enum dirflags df, enum terraintypes tt1, enum terraintypes tt2, int min) {

	int doorcount = 0;

	int od_n[w], od_s[w], od_w[h], od_e[h];
	int odc_n, odc_s, odc_w, odc_e;

	enum dirflags adf = df;

	if ( (odc_n = okay_door(map, x, y, w, D_NORTH, tt1, tt2, od_n))     == 0 ) adf &= ~DF_NORTH;
	if ( (odc_s = okay_door(map, x, y+h-1, w, D_SOUTH, tt1, tt2, od_s)) == 0 ) adf &= ~DF_SOUTH;
	if ( (odc_w = okay_door(map, x, y, h, D_WEST, tt1, tt2, od_w))      == 0 ) adf &= ~DF_WEST;
	if ( (odc_e = okay_door(map, x+w-1, y, h, D_EAST, tt1, tt2, od_e )) == 0 ) adf &= ~DF_EAST;

	if (adf == 0) return 1;

	do {

		int direction = -1;

		do {

			direction =randval(4);

			if ( (direction == D_NORTH) && ((adf & DF_NORTH) == 0) ) direction = -1;
			if ( (direction == D_SOUTH) && ((adf & DF_SOUTH) == 0) ) direction = -1;
			if ( (direction == D_EAST) && ((adf & DF_EAST) == 0) ) direction = -1;
			if ( (direction == D_WEST) && ((adf & DF_WEST) == 0) ) direction = -1;

		} while (direction == -1);

		int dy = y, dx = 0;

		switch (direction) {

			case D_NORTH: {
					      dy = y;

					      int dx = od_n[randval(odc_n)];
					      map->sq[dy * MAP_WIDTH + dx].type = TT_DOOR_CLOSED;
					      doorcount++;
					      adf &= ~DF_NORTH;

					      break; }
			case D_SOUTH: {

					      dy = y + h - 1;

					      int dx = od_s[randval(odc_s)];
					      map->sq[dy * MAP_WIDTH + dx].type = TT_DOOR_CLOSED;
					      doorcount++;
					      adf &= ~DF_SOUTH;

					      break; }
			case D_EAST: {
					     dx = x + w -1;

					     int dy = od_e[randval(odc_e)];
					     map->sq[dy * MAP_WIDTH + dx].type = TT_DOOR_CLOSED;
					     doorcount++;
					     adf &= ~DF_EAST;

					     break; }
			case D_WEST: {
					     dx = x;

					     int dy = od_w[randval(odc_w)];
					     map->sq[dy * MAP_WIDTH + dx].type = TT_DOOR_CLOSED;
					     doorcount++;
					     adf &= ~DF_WEST;

					     break; }

		}

	} while ((doorcount < min) && (adf != 0));
	return 0;
}

enum terraintypes check_collision(struct t_map* map, int x, int y, int w, int h, enum terraintypes normaltype) {

	for (int iy = y; iy < (y+h); iy++)
		for (int ix = x; ix < (x+w); ix++)
			if (map->sq[iy*MAP_WIDTH+ix].type != normaltype) return map->sq[iy*MAP_WIDTH+ix].type;

	return normaltype;
}
int count_terrain_left(struct t_map* map, int x, int y, int w, int h, enum terraintypes tt) {

	int n = 0;

	for (int iy = y; iy < (y+h); iy++)
		for (int ix = x; ix < (x+w); ix++)
			if (map->sq[iy*MAP_WIDTH+ix].type == tt) n++;

	return n;
}

int pop(unsigned x){
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0F0F0F0F;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return x & 0x0000003F;
}

int find_next_area(int* x, int* y, int w, int h, enum directions growdir) {

	switch (growdir) {

		case D_NORTH: {
			*x = (*x) + randval(w); *y = (*y) - 2;
		break; }
		case D_EAST: {
			*x = (*x) + w + 1; *y = (*y) + randval(h);
		break; }
		case D_SOUTH: {
			*x = (*x) + randval(w); *y = (*y) + h + 1;
		break; }
		case D_WEST: {
			*x = (*x) - 2; *y = (*y) + randval(h);
		break; }
		default:
		return -1;
	}
	return 0;
}

enum growflags {
	GF_RESTRICTED = 1,
};

int grow_room(struct t_map* map, int x, int y, enum directions growdir, enum dirflags join_to, int recurse,enum growflags flags, struct rect* outrect, struct pqel* roomqueue);

int recurse_grow(struct t_map* map, int x, int y, int w, int h, enum directions growdir, enum recurse_behavior recbeh, enum growflags flags, struct pqel* roomqueue) {

	if ((w <= 0) || (h <= 0)) return 1;
	if ((x <= 0) || (y <= 0)) return 1;
	if (((x + w) >= (MAP_WIDTH-1)) || ((y + h) >= (MAP_HEIGHT-1))) return 1;

	int x_ccw = x, x_cw = x, x_f = x;
	int y_ccw = y, y_cw = y, y_f = y;

	find_next_area(&x_f,&y_f,w,h,growdir);
	find_next_area(&x_ccw,&y_ccw,w,h,ccwDir(growdir));
	find_next_area(&x_cw,&y_cw,w,h,cwDir(growdir));

	struct rect o_f, o_ccw, o_cw;

	int grow_f, grow_ccw, grow_cw;

	if (recbeh == RB_RANDOM) recbeh = randbetween(RB_RANDOM+1, RB_ELEMENT_COUNT-1);

	switch (recbeh) {

		case RB_ALWAYS:
		grow_f = 1; grow_ccw = 1; grow_cw = 1; break;
		case RB_RANDOMLY:
		grow_ccw = randval(3); grow_cw = randval(3); grow_f = (grow_ccw + grow_cw) ? randval(3) : 1; break; // always build at least one!
		case RB_RANDOMLY_SAME:
		grow_f = 1; grow_cw = randval(3); grow_ccw = grow_cw; break;
		default:
		return 1;
	}


	if (grow_f) grow_room( map, x_f, y_f, growdir, dirFlag(opDir(growdir)), 0, flags, &o_f, roomqueue);
	if (grow_ccw) grow_room( map, x_ccw, y_ccw, ccwDir(growdir), dirFlag(cwDir(growdir)),0, flags, &o_ccw, roomqueue);
	if (grow_cw) grow_room( map, x_cw, y_cw, cwDir(growdir), dirFlag(ccwDir(growdir)), 0, flags,&o_cw, roomqueue);

	if (grow_f) recurse_grow(map,o_f.x,o_f.y,o_f.w,o_f.h,growdir,recbeh, flags,roomqueue);
	if (grow_ccw) recurse_grow(map,o_ccw.x,o_ccw.y,o_ccw.w,o_ccw.h,ccwDir(growdir),recbeh, flags,roomqueue);
	if (grow_cw) recurse_grow(map,o_cw.x,o_cw.y,o_cw.w,o_cw.h,cwDir(growdir),recbeh, flags,roomqueue);

	return 0;
}

int grow_room(struct t_map* map, int x, int y, enum directions growdir, enum dirflags join_to, int recurse, enum growflags flags, struct rect* outrect, struct pqel* roomqueue) {

	int nx=x, ny=y, nw=1, nh=1;

	if ( check_collision(map, nx, ny, nw, nh, TT_UNKNOWN) != TT_UNKNOWN) return 1;

	enum dirflags doors = join_to;

	int growing = 3;
	int mindoors = 1;

	int outsides = 0;

	while (growing) {

		int randdir = D_NONE;
		
		do { randdir =randval(4); } while (randdir == opDir(growdir));

		enum terraintypes ct;

		switch (randdir) {
			case D_NORTH: {

				ct = check_collision(map, nx, ny-1, nw, nh+1, TT_UNKNOWN);
				if (ct == TT_UNKNOWN) { ny--; nh++; doors |= DF_NORTH; }
				else if (ct == TT_WALL) growing--;
				else if (ct == TT_OUTSIDE) { outsides++; }
				break; }	
				
			case D_SOUTH: {

				ct = check_collision(map, nx, ny, nw, nh+1, TT_UNKNOWN);
				if (ct == TT_UNKNOWN) { nh++; doors |= DF_SOUTH; }
				else if (ct == TT_WALL) growing--;
				else if (ct == TT_OUTSIDE) { outsides++; }
				break; }	
			case D_EAST: {

				ct = check_collision(map, nx, ny, nw+1, nh, TT_UNKNOWN);
				if (ct == TT_UNKNOWN) { nw++; doors |= DF_EAST; }
				else if (ct == TT_WALL) growing--;
				else if (ct == TT_OUTSIDE) { outsides++; }
				break; }	
				
			case D_WEST: {

				ct = check_collision(map, nx-1, ny, nw+1, nh, TT_UNKNOWN);
				if (ct == TT_UNKNOWN) { nx--; nw++; doors |= DF_WEST; }
				else growing--;
					
				if (ct == TT_OUTSIDE) { outsides++; }
				break; }	
		}

	if (randval(256) < (nw * nh)) { growing=0;}
	}

	mindoors = 1;

	if (randval(2) == 0) flags |= GF_RESTRICTED;

	fill_rect(map,nx,ny,nw,nh,(flags & GF_RESTRICTED) ? TT_RESTRICTED_SPACE : TT_SPACE);
	wall_border_2(map,nx-1,ny-1,nw+2,nh+2);
	if (mindoors < 0) mindoors = 0;
	build_doors(map,nx-1,ny-1,nw+2,nh+2,doors,TT_SPACE,TT_SPACE,mindoors);	

	//if (randval(nw * nh) > 4) iterate_rooms(map,nx,ny,nw,nh);
					
	if (outrect) {
		outrect->x = nx;
		outrect->y = ny;
		outrect->w = nw;
		outrect->h = nh;
	}
	
	struct roomparams* newroom = malloc(sizeof(struct roomparams));
	newroom->x = nx; newroom->y = ny; newroom->w = nw; newroom->h = nh;
	newroom->style = RS_RANDOM;
	pq_add_element(roomqueue,ROOMQUEUE_SZ,newroom,1);

	if (recurse) recurse_grow(map,nx,ny,nw,nh, growdir, RB_RANDOM, flags, roomqueue);
	
	//decorate_room(map,nx,ny,nw,nh,RS_RANDOM);
	return 0;
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
				map->sq[door_y*MAP_WIDTH+div_x].type = TT_DOOR_CLOSED; }
			iterate_rooms_2(map,x,y,div_x - x,h);
			iterate_rooms_2(map,div_x+1,y,x + w - div_x - 1,h);
		} else {
			int div_y = y + randval(h - 2) + 1;
			for (int ix=x; ix < (x+w); ix++) {
				map->sq[div_y*MAP_WIDTH+ix].type = TT_WALL;
			}
			if (h > 1) {
				int door_x = x + randval(w);
				map->sq[div_y*MAP_WIDTH+door_x].type = TT_DOOR_CLOSED; }
			iterate_rooms_2(map,x,y,w,div_y - y);
			iterate_rooms_2(map,x,div_y+1,w,y + h - div_y - 1);
		}
		return 0;

	}

	return 0;
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
		if (w > 1) {
			int door_y = y + randval(h);
			map->sq[door_y*MAP_WIDTH+div_x].type = TT_DOOR_CLOSED; }
		iterate_rooms(map,x,y,div_x - x,h);
		iterate_rooms(map,div_x+1,y,x + w - div_x - 1,h);
	} else {
		int div_y = y + randval(h - 2) + 1;
		for (int ix=x; ix < (x+w); ix++) {
			map->sq[div_y*MAP_WIDTH+ix].type = TT_WALL;
		}
		if (h > 1) {
			int door_x = x + randval(w);
			map->sq[div_y*MAP_WIDTH+door_x].type = TT_DOOR_CLOSED; }
		iterate_rooms(map,x,y,w,div_y - y);
		iterate_rooms(map,x,div_y+1,w,y + h - div_y - 1);
	}
	return 0;
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

int generate_buildings(struct t_map* map, enum generate_modes gm) {

	struct pqel roomqueue[ROOMQUEUE_SZ];
	memset(roomqueue,0,sizeof(struct pqel) * ROOMQUEUE_SZ);

	if (gm == GM_RANDOM) gm = randbetween (GM_SINGLE, GM_ELEMENT_COUNT-1);

	switch(gm) {
		
		case GM_EMPTY: {
					 fill_rect(map,1,1,MAP_WIDTH-2,MAP_HEIGHT-2,TT_SPACE);
					 surround_with_walls(map,MAP_WIDTH/2,MAP_HEIGHT/2);
					 break; }

		case GM_LIBERAL: {

					 fill_rect(map,1,1,MAP_WIDTH-2,MAP_HEIGHT-2,TT_SPACE);
					 surround_with_walls(map,MAP_WIDTH/2,MAP_HEIGHT/2);
					 iterate_rooms(map,1,1,MAP_WIDTH-2,MAP_HEIGHT-2);
					 break; }
		case GM_SINGLE: {

					fill_rect(map,2,2,MAP_WIDTH-4,MAP_HEIGHT-4,TT_UNKNOWN);
					int outw = 5 + 2 * randval(5);
					int outh = 3 + 2 * randval(2);
					fill_rect(map,(MAP_WIDTH-outw)/2,MAP_HEIGHT-1-outh,outw,outh,TT_OUTSIDE);
					int lobbyw = outw + (2 * randval(2));
					int lobbyh = 2 + randval(2);

					fill_rect(map,(MAP_WIDTH-lobbyw)/2,MAP_HEIGHT-1-outh-lobbyh,lobbyw,lobbyh,TT_SPACE);
					surround_with_walls(map,MAP_WIDTH/2,MAP_HEIGHT-1-outh-(lobbyh/2));

					map->sq[ (MAP_HEIGHT - 1 - outh) * MAP_WIDTH + (MAP_WIDTH/2) - 1 ].type = TT_DOOR_CLOSED;

						recurse_grow(map,(MAP_WIDTH-lobbyw)/2,MAP_HEIGHT-1-outh-lobbyh,lobbyw,lobbyh,D_NORTH,RB_RANDOM, 0, roomqueue);

					for (int i=0; i < SQUAD_MAX; i++)
						map->spawn_points[i] = (struct coords){.x = 38 + (i%3), .y = MAP_HEIGHT - 3 + (i/3)};

					break; }


		case GM_COMPLEX: {
					 fill_rect(map,2,2,MAP_WIDTH-4,MAP_HEIGHT-4,TT_UNKNOWN);
					 while (count_terrain_left(map,2,2,MAP_WIDTH-4,MAP_HEIGHT-4,TT_UNKNOWN)) {
						 grow_room(map,2,2,-1,15,1,0, NULL, roomqueue);
					 }

					 break; }
		default: {
				 break; }

	}

	struct roomparams* nextroom = NULL;

	while ((nextroom = pq_get_lowest(roomqueue,ROOMQUEUE_SZ)) != NULL) {
		decorate_room(map,nextroom->x,nextroom->y,nextroom->w,nextroom->h,nextroom->style);
		free(nextroom);
	}
	return 0;
}
