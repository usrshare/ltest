// vim: cin:sts=4:sw=4 
#include "map_ui.h"

#include <curses.h>
#include <string.h>

#include "entity.h"
#include "weapon.h"
#include "map_ai.h"
#include "cpairs.h"
#include "mapmode.h"

WINDOW* topwindow;
WINDOW* headerwindow;
WINDOW* msgwindow;
WINDOW* statwindow;
WINDOW* mapwindow;

int morecount = 0;

chtype mapchar(struct t_square* sq) {

    switch (sq->type) {
	case TT_OUTSIDE: return ',';
	case TT_SPACE: return '.';
	case TT_RESTRICTED_SPACE: return ':';
	case TT_SPECIAL_SPACE: return '.';
	case TT_WALL: return ACS_CKBOARD;
	case TT_WINDOW: return '~';
	case TT_DOOR_CLOSED: return '+';
	case TT_DOOR_OPEN: return '-';
	case TT_BARS: return '#';
	case TT_TABLE: return '-';
	case TT_LOCKER: return '\\';
	case TT_CUSTOM: return '!';
	case TT_STAIRS: return '>';
	default: return 'X';
    }
}
chtype entchar(struct t_map_entity* e) {

    switch (e->type) {
	case ET_NONE: return 0;
	case ET_PLAYER: return (('1' + (e->e_id)) | A_BOLD);
	case ET_CPU: {

			 int char_color = 0;

			 if (e->ent) {
			     switch (e->ent->align) {
				 case ALIGN_ARCHCONSERVATIVE:
				 case ALIGN_CONSERVATIVE:
				     char_color = CP_RED; break;
				 case ALIGN_MODERATE:
				     char_color = 0; break;
				 case ALIGN_LIBERAL:
				 case ALIGN_ELITELIBERAL:
				     char_color = CP_GREEN; break;
			     }}




			 return char_color | '@'; }
	case ET_STATIC: return '!';
	default: return 'X';
    }
}
int draw_map(struct t_map* map, struct t_map_entity* persp, bool show_fov, bool show_targets, bool show_heatmaps, bool hl_persp) {

    int cs = curs_set(0);
    int x,y;
    getsyx(y,x);

    wmove(mapwindow,0,0);

    for (int iy=0; iy< MAP_HEIGHT; iy++) {
	for (int ix=0; ix < MAP_WIDTH; ix++) {

	    int tilech = mapchar(&map->sq[iy*(MAP_WIDTH)+ix]); 

	    int tilevis = 1;

	    if ((persp != NULL) && (persp->aidata)) tilevis = persp->aidata->viewarr[iy * MAP_WIDTH + ix];

	    chtype tileflags = 0;

	    switch (tilevis) {
		case 1: tileflags = CP_BLUE; break;
		case 2: tileflags = CP_DARKGRAY; break;
		case 3: tileflags = CP_WHITE; break;
		case 4:	tileflags = CP_WHITE; break;
		default: break;
	    }

	    if ((show_fov) && (tilevis >= 3)) {
		chtype fovcolor = 0;
		if ((tflags[map->sq[iy*(MAP_WIDTH)+ix].type] & TF_BLOCKS_VISION) == 0) {

		    for (int i=0; i < MAX_ENTITIES; i++) {
			if (map->ent[i].type == ET_NONE) continue;

			int ex = map->ent[i].x; int ey = map->ent[i].y;
			if ((map->ent[i].type != ET_PLAYER) && (map->ent[i].aidata) && (persp->aidata->viewarr[ey * (MAP_WIDTH) + ex] >= 3) && (map->ent[i].aidata->viewarr[iy * MAP_WIDTH + ix] >= 3) ) {

			    switch (map->ent[i].aidata->task) {
				case AIT_WORKING: fovcolor = CP_GREEN; break;
				case AIT_PATROLLING: fovcolor = CP_CYAN; break;
				case AIT_CHECKING_OUT:
				case AIT_PLEASE_LEAVE: fovcolor = CP_YELLOW; break;
				case AIT_PURSUING:
				case AIT_ATTACKING: fovcolor = CP_RED; break;
				case AIT_FLEEING: fovcolor = CP_PURPLE; break;
				case AIT_LOOKING_FOR: fovcolor = CP_YELLOW; break;
			    }
			}
		    } }
		if (fovcolor) tileflags = fovcolor;
	    }

	    if (tilevis) mvwaddch(mapwindow,iy,ix, tileflags | tilech );

	}
    }

    for (int i=0; i < MAX_ENTITIES; i++) {


	if (map->ent[i].type == ET_NONE) continue;

	if ((map->ent[i].aidata) != NULL) {

	    if (show_targets) {
		if (vtile(map->ent[i].aidata->dx,map->ent[i].aidata->dy))
		    mvwaddch(mapwindow,map->ent[i].aidata->dy,map->ent[i].aidata->dx,'%' | CP_PURPLE);
	    }

	    if (show_heatmaps) {
		for (int m=0; m < HEATMAP_SIZE; m++) {
		    uint16_t yx = map->ent[i].aidata->heatmap_old[m];
		    if (yx == 65535) continue;
		    uint8_t y = yx / MAP_WIDTH; uint8_t x = yx % MAP_WIDTH;
		    mvwaddch(mapwindow,y,x,'&' | CP_YELLOW);
		}
		for (int m=0; m < HEATMAP_SIZE; m++) {
		    uint16_t yx = map->ent[i].aidata->heatmap_new[m];
		    if (yx == 65535) continue;
		    uint8_t y = yx / MAP_WIDTH; uint8_t x = yx % MAP_WIDTH;
		    mvwaddch(mapwindow,y,x,'&' | CP_GREEN);
		}
	    }

	}
    }

    for (int i=0; i < MAX_ENTITIES; i++) {

	if (map->ent[i].type == ET_NONE) continue;
	int ex = map->ent[i].x; int ey = map->ent[i].y;

	if ( (persp == NULL) || (map->ent[i].flags & EF_ALWAYSVISIBLE) || ( (persp->aidata) && (persp->aidata->viewarr[ey * (MAP_WIDTH) + ex] >= 3) ) ) {

	    int highlight = (hl_persp) && (&map->ent[i] == persp);
	    mvwaddch(mapwindow,ey,ex,entchar(&map->ent[i]) | (highlight ? A_REVERSE : 0) );
	}
    }

    wnoutrefresh(mapwindow);

    setsyx(y,x);
    curs_set(cs);
    doupdate();
    return 0;
}
int mapgetch() {

    int x,y;
    morecount = 0;

    getsyx(y,x);
    int m = wmove(msgwindow,0,COLS-1);
    if (m == ERR) beep();

    int c = getch();

    setsyx(y,x);

    return c;
}
int nc_beep(void) {

    return beep();
}

char* alertdescriptions[AL_COUNT] = {
    "",
    "Conservatives Suspicious",
    "Conservatives Alerted",
    "Backup Called"
};

int updheader(struct t_map* map) {
    wmove(headerwindow,0,0);
    whline(headerwindow,ACS_HLINE,COLS);
    wattron(headerwindow, A_BOLD);
    mvwprintw(headerwindow,0,1," insert title here. - T%5d - %s ",map->time,alertdescriptions[map->alert_state]);
    wattroff(headerwindow, A_BOLD);
    wrefresh(headerwindow);
    return 0;
}

int msgaddstr(const char *string) {

    int y,x;

    /*
    char* thisline = strtok(string,"\n");    

    int lines=0;

    while (thisline != NULL) {
    */

	if (morecount >= 1) {
	    getsyx(y,x);
	    wmove(msgwindow,0,COLS-6);
	    wattron(msgwindow,A_REVERSE);
	    wprintw(msgwindow,"(more)");
	    wattroff(msgwindow,A_REVERSE);
	    wmove(msgwindow,0,COLS-1);
	    wrefresh(msgwindow);
	    mapgetch();
	    setsyx(y,x);
	    morecount = 0;
	}

	wclear(msgwindow);
	mvwaddstr(msgwindow,0,0,string);
	wmove(msgwindow,0,0);

	//thisline = strtok(NULL,"\n");
	morecount++;
    /*}*/
	return 0;
}

int msgvprintw(const char *fmt, va_list ap) {

    char outtext[1024];

    int r = vsnprintf(outtext,1024,fmt,ap);

    msgaddstr(outtext);

    return r;
}

int msgprintw(const char *fmt, ...) {

    va_list varglist;
    va_start(varglist,fmt);
    int r = msgvprintw(fmt,varglist);
    va_end(varglist);
    return r;
}

int msgattrset(int attrs) {
    return wattrset(msgwindow,attrs);
}

int describe_map_entity(struct t_map_entity* me, char* const restrict o_name, size_t strsize) {
    return describe_entity(me->ent,o_name,strsize);
}


int msgsay(struct t_map_entity* me, const char* fmt, ...) {

    char my_description[66];
    char outstr[1024];

    describe_map_entity(me,my_description,66);

    va_list varglist;
    va_start(varglist,fmt);
    int r = vsnprintf(outstr,1024,fmt,varglist);
    va_end(varglist);

    return msgprintw("%.66s: %.1024s",my_description,outstr);
}

enum movedirections askdir() {

    msgprintw("Please specify a direction: [yuhjklbn]>");
    wrefresh(msgwindow);

    enum movedirections r = MD_COUNT;

    int go_on = 1;

    while (go_on) {
	echo();
	int c = mapgetch();
	noecho();
	switch(c) {
	    case 'h':
	    case 'H':
		r = MD_WEST; go_on = 0;break;
	    case 'j':
	    case 'J':
		r = MD_SOUTH; go_on = 0;break;
	    case 'k':
	    case 'K':
		r = MD_NORTH; go_on = 0;break;
	    case 'l':
	    case 'L':
		r = MD_EAST; go_on = 0;break;
	    case 'y':
	    case 'Y':
		r = MD_NORTHWEST; go_on = 0;break;
	    case 'u':
	    case 'U':
		r = MD_NORTHEAST; go_on = 0;break;
	    case 'b':
	    case 'B':
		r = MD_SOUTHWEST; go_on = 0;break;
	    case 'n':
	    case 'N':
		r = MD_SOUTHEAST; go_on = 0;break;
	    default:
		beep();
		msgprintw("Invalid direction. Please choose [yuhjklbn]>");
		wrefresh(msgwindow);
	}

    }

    return r;
}

int init_status (struct t_map* map) {

    int y,x;
    setsyx(y,x);

    wmove(statwindow,0,0);
    for (int i=0; i < 79; i++)
	if (i % 13) waddch(statwindow,ACS_HLINE); else waddch(statwindow,ACS_TTEE);
    wmove(statwindow,1,0);
    for (int i=0; i < 79; i+= 13) {
	mvwaddch(statwindow,1,i,ACS_VLINE);
	mvwaddch(statwindow,2,i,ACS_VLINE); }
    
    getsyx(y,x);

    return 0;
}

int update_status (struct t_map* map) {

    int y,x;
    setsyx(y,x);

    int pi = 0;
    for (int i=0; i < MAX_ENTITIES; i++) {

	if ((map->ent[i].type == ET_PLAYER) && (pi < 6)) {

	    struct t_map_entity* e = &(map->ent[i]);

	    char* e_name = (strlen(e->ent->nickname) ? e->ent->nickname : e->ent->firstname);

	    int nl = strlen(e_name);

	    mvwprintw(statwindow,0, pi*13 + 1, " %s ",e_name);

	    char* e_weapon = w_type(e->ent->weapon)->shortname;
	    nl = strlen(e_weapon);
	    mvwprintw(statwindow,1, pi*13 + 13 - nl, "%s",e_weapon);

	    int statattr;
	    char* e_stat = gethealthstat(e->ent, 1, &statattr);
	    nl = strlen(e_stat);

	    wattron(statwindow,statattr);
	    mvwprintw(statwindow,2, pi*13 + 13 - nl, "%s",e_stat);
	    wattroff(statwindow,statattr);

	    pi++;
	}
    }

    getsyx(y,x);
    return 0;
}

int update_ui (struct t_map* map) {
    wmove(msgwindow,0,COLS-1);
    wrefresh(headerwindow);
    wrefresh(statwindow);
    wrefresh(msgwindow);
    wrefresh(msgwindow);

    return 0;
}

int map_ui_init(struct t_map* map) {

    mapwindow = newwin(20,COLS,LINES-20,0);

    topwindow = newwin(LINES-20,COLS,0,0);
    headerwindow = subwin(topwindow,1,COLS,LINES-21,0);

    updheader(map);

    msgwindow = subwin(topwindow,1,COLS,0,0);
    if (msgwindow == 0) return 1;
    //scrollok(msgwindow,1);

    statwindow = subwin(topwindow,LINES-22,COLS,1,0);
    if (statwindow == 0) return 1;

    init_status(map);

    keypad(msgwindow,1);

    wmove(msgwindow,0,0);

    return 0;
}

int map_ui_free(struct t_map* map) {

    delwin(mapwindow);
    delwin(msgwindow);
    delwin(statwindow);
    delwin(headerwindow);
    delwin(topwindow);

    return 0;
}
