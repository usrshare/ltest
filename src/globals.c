// vim: cin:sts=4:sw=4 
#include "globals.h"

int dbgmode;

// temporary variables.

enum uimodes curui;
enum gamemodes mode;

short sitealarm;
short sitealarmtimer;

short cursite;

//stats 

int stat_recruits=0;
int stat_kidnappings=0;
int stat_dead=0;
int stat_kills=0;
int stat_buys=0;
int stat_burns=0;

int ustat_recruits=0;
int ustat_kidnappings=0;
int ustat_dead=0;
int ustat_kills=0;
int ustat_funds=0;
int ustat_spent=0;
int ustat_buys=0;
int ustat_burns=0;

char ccs_kills=0;
int ccs_siege_kills=0;
int ccs_boss_kills=0;

// game vars

char slogan[80];

extern short law[LAWNUM];
short law[LAWNUM];
