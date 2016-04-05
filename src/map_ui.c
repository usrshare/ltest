// vim: cin:sts=4:sw=4 
#include "map_ui.h"

#include <curses.h>
#include <string.h>

#include "entity.h"
#include "weapon.h"
#include "map_ai.h"
#include "cpairs.h"
#include "mapmode.h"
#include "location.h"

WINDOW* topwindow;
WINDOW* msgwindow;
WINDOW* statwindow;
WINDOW* mapwindow;

int morecount = 0;

chtype mapchar[TT_ELEMENT_COUNT] = {
	'.',',','.','.',
	':','~','#','~',
	'+','-','/','t',
	'\\','!','>','?'
};

//this array can't contain ACS characters, because they're not defined as constant.
//the ACS characters are instead assigned in map_ui_init(); 

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
	case ET_LOOT: return CP_YELLOW | '$';
	case ET_STATIC: return '!';
	default: return 'X';
    }
}

int update_player_map(struct t_map* map, struct t_map_entity* player, int hl_player) {
    return draw_map(map,player,1,!dbgmode,dbgmode,dbgmode,hl_player);
}

int draw_map(struct t_map* map, struct t_map_entity* persp, bool show_vis, bool show_fov, bool show_targets, bool show_heatmaps, bool hl_persp) {

    int cs = curs_set(0);
    int x,y;
    getsyx(y,x);

    wmove(mapwindow,0,0);

    for (int iy=0; iy< MAP_HEIGHT; iy++) {
	for (int ix=0; ix < MAP_WIDTH; ix++) {

	    chtype tilech = mapchar[map->sq[iy*(MAP_WIDTH)+ix].type]; 

	    int tilevis = 1;

	    tilevis = map->aidata.p_viewarr[iy * MAP_WIDTH + ix];
	    if (!show_vis && (tilevis<1)) tilevis = 1;

	    chtype tileflags = 0;

	    switch (tilevis) {
		case 1: tileflags = CP_BLUE; break;
		case 2: tileflags = CP_LIGHTGRAY; break;
		case 3:
		case 4:	tileflags = CP_WHITE; break;
		default: break;
	    }

	    if (tilevis) mvwaddch(mapwindow,iy,ix, tileflags | tilech ); else if (dbgmode) mvwaddch (mapwindow,iy,ix,' ');

	}
    }
    
    if (show_heatmaps) {
	for (int yx=0; yx < HEATMAP_SIZE; yx++) {
	    uint8_t y = yx / MAP_WIDTH; uint8_t x = yx % MAP_WIDTH;
	    uint8_t m = map->aidata.e_hm[yx];
	    if (m) mvwaddch(mapwindow,y,x,'&' | ( (m > 2) ? CP_RED : ((m == 2) ? CP_GREEN : CP_YELLOW) ) ) ;
	}
    }

    for (int i=0; i < MAX_ENTITIES; i++) {


	if (map->ent[i].type == ET_NONE) continue;

	if ((map->ent[i].aidata) != NULL) {

	    if (show_targets) {
		if (vtile(map->ent[i].aidata->dx,map->ent[i].aidata->dy))
		    mvwaddch(mapwindow,map->ent[i].aidata->dy,map->ent[i].aidata->dx,'%' | CP_PURPLE);
	    }

	    if (show_fov && (map->aidata.p_viewarr[map->ent[i].y * MAP_WIDTH + map->ent[i].x] >= 3)) {

		// if we can see the entity, that means we can see what direction it is currently looking at.
		// the next tile in that direction will be redrawn with a cyan color.

		uint8_t vx = map->ent[i].x + movediff[map->ent[i].aidata->viewdir][0];
		uint8_t vy = map->ent[i].y + movediff[map->ent[i].aidata->viewdir][1];

		if (vtile(vx,vy)) {
		    chtype t = mvwinch(mapwindow,vy,vx);
		    // we strip the original character of all previous attributes,
		    // except the "alternate charset" one.
		    
		    chtype fcol = 0;

		    switch(map->ent[i].aidata->task) {
			case AIT_WORKING: fcol = CP_BLUE; break;
			case AIT_PATROLLING: fcol = CP_CYAN; break;
			case AIT_CHECKING_OUT:
			case AIT_LOOKING_FOR: fcol = CP_YELLOW; break;
			case AIT_PLEASE_LEAVE:
			case AIT_PURSUING:
			case AIT_ATTACKING: fcol = CP_RED; break;
			case AIT_FLEEING: fcol = CP_MAGENTA; break;
			default: fcol = CP_GREEN; break;


		    }	

		    mvwaddch(mapwindow,vy,vx,(t & (A_ALTCHARSET | A_CHARTEXT)) | fcol);
		}
	    }


	}
    }
    
    for (int i= (MAX_ENTITIES - 1); i >= 0; i--) {

	if (map->ent[i].type == ET_NONE) continue;
	int ex = map->ent[i].x; int ey = map->ent[i].y;

	if ( (persp == NULL) || (map->ent[i].flags & EF_ALWAYSVISIBLE) || ((map->aidata.p_viewarr[ey * (MAP_WIDTH) + ex] >= 3) ) ) {

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

    int cy,cx;

    getyx(msgwindow,cy,cx);
    int m = wmove(msgwindow,0,COLS-1);
    if (m == ERR) beep();

    int c = getch();

    wmove(msgwindow,cy,cx);
    setsyx(y,x);

    return c;
}

int askgetch(bool nocr) {

    morecount = 0;

    echo();
    int c = wgetch(msgwindow);
    noecho();

    if (!nocr) waddstr(msgwindow,"\n");

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

int alarm_status (struct t_map* map) {

    if(map->postalarmtimer>80)
    {
	switch(map->type)
	{
	    return CP_RED | A_REVERSE; 
	}
    }
    else if(map->postalarmtimer>60) { return CP_RED | A_BOLD; }
    else if(map->sitealienate==1) { return CP_MAGENTA; }
    else if(map->sitealienate==2) { return CP_MAGENTA | A_REVERSE; }
    else if(map->sitealarm) { return CP_RED; }
    else if(map->sitealarmtimer==0) { return CP_YELLOW; }

    return CP_GREEN;
}

int maprefresh(void) {

    return wrefresh(mapwindow);
}

int msgaddstr(const char *string) {

    char* dblstr = malloc(strlen(string)+1);
    strcpy(dblstr,string);
    char* thisline = dblstr;

    bool hascr = strchr(thisline,'\n');
    thisline = strtok(dblstr,"\n");    
    int lines=0;
    while (thisline != NULL) {

	if (morecount >= LINES-23) {
	    int cy,cx;
	    getyx(msgwindow,cy,cx);
	    wmove(msgwindow,0,COLS-4);
	    wattron(msgwindow,A_REVERSE);
	    waddstr(msgwindow,">>");
	    wattroff(msgwindow,A_REVERSE);
	    wmove(msgwindow,0,COLS-1);
	    wrefresh(msgwindow);
	    askgetch(false);
	    wmove(msgwindow,cy,cx);
	    morecount = 0;
	}

	//wclear(msgwindow);
	waddstr(msgwindow,thisline);

	thisline = strtok(NULL,"\n");
	morecount++;
    }
    if (hascr) {
	if (LINES > 25) waddstr(msgwindow,"\n"); else wmove(msgwindow,0,0); }
    free(dblstr);
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

int mapaddch(const char *c, uint8_t y, uint8_t x) {

    mvwaddstr(mapwindow,y,x,c);

    return 0;
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
	int c = askgetch(false);
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

int askpos(uint8_t* y, uint8_t* x) {

    int ny = *y, nx = *x;
    
    wrefresh(msgwindow);

    int go_on = 1;
    int res = 0;

    while (go_on) {
    
	msgprintw("(%2d,%2d) Choose a location. [yuhjklbn] Move [.] Confirm [,] Cancel",nx,ny);

	wmove(mapwindow,ny,nx);
	int c = wgetch(mapwindow);
	switch(c) {
	    case 'h':
	    case 'H':
		if (nx > 0) nx--; break;
	    case 'j':
	    case 'J':
		if (ny < MAP_HEIGHT) ny++; break;
	    case 'k':
	    case 'K':
		if (ny > 0) ny--; break;
	    case 'l':
	    case 'L':
		if (nx < MAP_WIDTH) nx++; break;
	    case 'y':
	    case 'Y':
		if (nx > 0) nx--;
		if (ny > 0) ny--; break;
	    case 'u':
	    case 'U':
		if (nx < MAP_WIDTH) nx++;
		if (ny > 0) ny--; break;
	    case 'b':
	    case 'B':
		if (nx > 0) nx--;
		if (ny < MAP_HEIGHT) ny++; break;
	    case 'n':
	    case 'N':
		if (nx > 0) nx--;
		if (ny < MAP_HEIGHT) ny++; break;
	    case '.':
		go_on = 0;
		res = 0;
		break;
	    case ',':
		go_on = 0;
		res = 1;
	    default:
		beep();
		wrefresh(msgwindow);
	}

    }

    morecount=0;
    waddstr(msgwindow,"\n");

    if (res == 0) *y = ny; *x = nx;
    return res;
}

int init_status (struct t_map* map) {

    int y = 0,x = 0;
    setsyx(y,x);

    getsyx(y,x);

    return 0;
}

int update_status (struct t_map* map) {

    int y=0,x=0;
    setsyx(y,x);

    int border_attr = alarm_status(map);

    wattron(statwindow,border_attr);

    wmove(statwindow,0,0);
    for (int i=0; i < COLS; i++)
	if ((i > 79) || (i % 13)) waddch(statwindow,ACS_HLINE); else waddch(statwindow,ACS_TTEE);
    wmove(statwindow,1,0);
    for (int i=0; i < 79; i+= 13) {
	mvwaddch(statwindow,1,i,ACS_VLINE); }
    wmove(statwindow,2,0);
    for (int i=0; i < COLS; i++)
	if ((i > 79) || (i % 13)) waddch(statwindow,ACS_HLINE); else waddch(statwindow,ACS_BTEE);
    
    wattroff(statwindow,border_attr);

    int pi = 0;
    for (int i=0; i < MAX_ENTITIES; i++) {

	if ((map->ent[i].type == ET_PLAYER) && (pi < 6)) {

	    struct t_map_entity* e = &(map->ent[i]);

	    char* e_name = (strlen(e->ent->nickname) ? e->ent->nickname : e->ent->firstname);

	    int nl = strlen(e_name);

	    mvwprintw(statwindow,0, pi*13 + 1, " %.10s ",e_name);

	    const char* e_weapon = w_type(e->ent->weapon)->shortname;
	    nl = strlen(e_weapon);
	    mvwprintw(statwindow,1, pi*13 + 13 - nl, "%.10s",e_weapon);

	    int statattr;
	    char* e_stat = gethealthstat(e->ent, 1, &statattr);
	    nl = strlen(e_stat);

	    wattron(statwindow,statattr);
	    mvwprintw(statwindow,2, pi*13 + 13 - nl, "%.10s",e_stat);
	    wattroff(statwindow,statattr);

	    pi++;
	}
    }

    getsyx(y,x);
    return 0;
}

int update_ui (struct t_map* map) {
    //wmove(msgwindow,0,COLS-1);
    wrefresh(statwindow);
    wrefresh(msgwindow);
    wrefresh(msgwindow);

    return 0;
}

int map_ui_init(struct t_map* map) {

    mapchar[TT_WALL] = ACS_CKBOARD; // rewriting with a proper character on the fly

    mapwindow = newwin(20,COLS,LINES-20,0);

    topwindow = newwin(LINES-20,COLS,0,0);

    msgwindow = subwin(topwindow,LINES-23,COLS,0,0);
    if (msgwindow == 0) return 1;
    //scrollok(msgwindow,1);

    statwindow = subwin(topwindow,3,COLS,LINES-23,0);
    if (statwindow == 0) return 1;

    init_status(map);

    keypad(msgwindow,1);
    scrollok(msgwindow,TRUE);

    wmove(msgwindow,0,0);

    return 0;
}

int map_ui_free(struct t_map* map) {

    delwin(mapwindow);
    delwin(msgwindow);
    delwin(statwindow);
    delwin(topwindow);

    return 0;
}
