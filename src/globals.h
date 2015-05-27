// vim: cin:sts=4:sw=4 
#ifndef GLOBALS_H
#define GLOBALS_H

#define gamelog NULL //TODO actual gamelog

extern int dbgmode;

enum uimodes {

    UI_MAPMODE,
};

extern enum uimodes curui;

enum gamemodes
{
   GM_TITLE,
   GM_BASE,
   GM_SITE,
   GM_CHASECAR,
   GM_CHASEFOOT
};

extern enum gamemodes mode;

extern short sitealarm;
extern short sitealarmtimer;

#define SQUAD_MAX 6

#endif
