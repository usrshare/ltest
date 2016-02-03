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
	case ET_LOOT: return CP_YELLOW | '$';
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

	    if ((persp != NULL) && (persp->aidata)) tilevis = map->aidata.p_viewarr[iy * MAP_WIDTH + ix];

	    chtype tileflags = 0;

	    switch (tilevis) {
		case 1: tileflags = CP_BLUE; break;
		case 2: tileflags = CP_WHITE; break;
		case 3:
		case 4:	tileflags = CP_WHITE | A_BOLD; break;
		default: break;
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

	    if (show_fov && (map->aidata.p_viewarr[map->ent[i].y * MAP_WIDTH + map->ent[i].x] >= 3)) {

		// if we can see the entity, that means we can see what direction it is currently looking at.
		// the next tile in that direction will be redrawn with a cyan color.

		uint8_t vx = map->ent[i].x + movediff[map->ent[i].aidata->viewdir][0];
		uint8_t vy = map->ent[i].y + movediff[map->ent[i].aidata->viewdir][1];

		if (vtile(vx,vy)) {
		    chtype t = mvwinch(mapwindow,vy,vx);
		    // we strip the original character of all previous attributes,
		    // except the "alternate charset" one.
		    mvwaddch(mapwindow,vy,vx,(t & (A_ALTCHARSET | A_CHARTEXT)) | CP_CYAN);
		}
	    }

	    if (show_heatmaps) {
		for (int m=0; m < HEATMAP_SIZE; m++) {
		    uint16_t yx = map->aidata.e_heatmap_old[m];
		    if (yx == 65535) continue;
		    uint8_t y = yx / MAP_WIDTH; uint8_t x = yx % MAP_WIDTH;
		    mvwaddch(mapwindow,y,x,'&' | CP_YELLOW);
		}
		for (int m=0; m < HEATMAP_SIZE; m++) {
		    uint16_t yx = map->aidata.e_heatmap_new[m];
		    if (yx == 65535) continue;
		    uint8_t y = yx / MAP_WIDTH; uint8_t x = yx % MAP_WIDTH;
		    mvwaddch(mapwindow,y,x,'&' | CP_GREEN);
		}
	    }

	}
    }

    for (int i= (MAX_ENTITIES - 1); i >= 0; i--) {

	if (map->ent[i].type == ET_NONE) continue;
	int ex = map->ent[i].x; int ey = map->ent[i].y;

	if ( (persp == NULL) || (map->ent[i].flags & EF_ALWAYSVISIBLE) || ( (persp->aidata) && (map->aidata.p_viewarr[ey * (MAP_WIDTH) + ex] >= 3) ) ) {

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


int askgetch() {

    morecount = 0;
	
    echo();
    int c = wgetch(msgwindow);
    noecho();
    
    waddstr(msgwindow,"\n");
     
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

const char* alarm_status (struct t_map* map) {

    if(map->postalarmtimer>80)
    {
	switch(map->type)
	{
	    case SITE_GOVERNMENT_ARMYBASE:
		return("SOLDIERS AND TANKS RESPONDING");
		break;
	    case SITE_GOVERNMENT_WHITE_HOUSE:
		return("SECRET SERVICE RESPONDING");
		break;
	    case SITE_GOVERNMENT_INTELLIGENCEHQ:
		return("AGENTS RESPONDING");
		break;
	    case SITE_CORPORATE_HEADQUARTERS:
	    case SITE_CORPORATE_HOUSE:
		return("MERCENARIES RESPONDING");
		break;
	    case SITE_MEDIA_AMRADIO:
	    case SITE_MEDIA_CABLENEWS:
		return("ANGRY MOB RESPONDING");
		break;
	    case SITE_BUSINESS_CRACKHOUSE:
		return("GANG MEMBERS RESPONDING");
		break;
	    case SITE_GOVERNMENT_POLICESTATION:
	    default:
		/*if(location[cursite]->renting==RENTING_CCS)
		  {
		  return("CCS VIGILANTIES RESPONDING");
		  }
		  else */ if(law[LAW_DEATHPENALTY]==-2&&
			  law[LAW_POLICEBEHAVIOR]==-2)return("DEATH SQUADS RESPONDING");
		  else return("POLICE RESPONDING");
		  break;
	}
    }
    else if(map->postalarmtimer>60) { return("CONSERVATIVE REINFORCEMENTS INCOMING"); }
    else if(map->sitealienate==1) { return("ALIENATED MASSES"); }
    else if(map->sitealienate==2) { return("ALIENATED EVERYONE"); }
    else if(map->sitealarm) { return("CONSERVATIVES ALARMED");  }
    else if(map->sitealarmtimer==0) { return("CONSERVATIVES SUSPICIOUS"); }

    return "EVERYTHING QUIET";
}

int updheader(struct t_map* map) {
    wmove(headerwindow,0,0);
    whline(headerwindow,ACS_HLINE,COLS);
    wattron(headerwindow, A_BOLD);
    mvwprintw(headerwindow,0,1," insert title here. - T%5d - %s ",map->time,alarm_status(map));
    wattroff(headerwindow, A_BOLD);
    wrefresh(headerwindow);
    return 0;
}

int msgaddstr(const char *string) {

    char* dblstr = strdup(string);
    char* thisline = dblstr;

    bool hascr = strchr(thisline,'\n');
    thisline = strtok(dblstr,"\n");    
    int lines=0;
    while (thisline != NULL) {

    if (morecount >= LINES-24) {
	int cy,cx;
	getyx(msgwindow,cy,cx);
	wmove(msgwindow,0,COLS-4);
	wattron(msgwindow,A_REVERSE);
	waddstr(msgwindow,">>");
	wattroff(msgwindow,A_REVERSE);
	wmove(msgwindow,0,COLS-1);
	wrefresh(msgwindow);
	askgetch();
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
	int c = askgetch();
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

    int y = 0,x = 0;
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

    int y=0,x=0;
    setsyx(y,x);

    int pi = 0;
    for (int i=0; i < MAX_ENTITIES; i++) {

	if ((map->ent[i].type == ET_PLAYER) && (pi < 6)) {

	    struct t_map_entity* e = &(map->ent[i]);

	    char* e_name = (strlen(e->ent->nickname) ? e->ent->nickname : e->ent->firstname);

	    int nl = strlen(e_name);

	    mvwprintw(statwindow,0, pi*13 + 1, " %s ",e_name);

	    const char* e_weapon = w_type(e->ent->weapon)->shortname;
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
    //wmove(msgwindow,0,COLS-1);
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

    msgwindow = subwin(topwindow,LINES-24,COLS,0,0);
    if (msgwindow == 0) return 1;
    //scrollok(msgwindow,1);

    statwindow = subwin(topwindow,3,COLS,LINES-24,0);
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
    delwin(headerwindow);
    delwin(topwindow);

    return 0;
}
