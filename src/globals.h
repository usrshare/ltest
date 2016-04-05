// vim: cin:sts=4:sw=4 
#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdbool.h>

#define gamelog NULL //TODO actual gamelog

extern bool dbgmode;
extern bool unichar;

/// temp vars

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

extern short cursite;

/// stats

extern int stat_recruits;
extern int stat_kidnappings;
extern int stat_dead;
extern int stat_kills;
extern int stat_buys;
extern int stat_burns;

extern int ustat_recruits;
extern int ustat_kidnappings;
extern int ustat_dead;
extern int ustat_kills;
extern int ustat_funds;
extern int ustat_spent;
extern int ustat_buys;
extern int ustat_burns;

extern char ccs_kills;
extern int ccs_siege_kills;
extern int ccs_boss_kills;

/// end stats

enum Views
{
   VIEW_STALIN=-2, // this one is -2 and is actually calculated based on views >=0 and <VIEWNUM-3
   VIEW_MOOD, // this one is -1 and is likewise calculated based on views >=0 and <VIEWNUM-3
   VIEW_GAY, // view #0, the first one that is actually in the attitude[] array
   VIEW_DEATHPENALTY,
   VIEW_TAXES,
   VIEW_NUCLEARPOWER,
   VIEW_ANIMALRESEARCH,
   VIEW_POLICEBEHAVIOR,
   VIEW_TORTURE,
   VIEW_INTELLIGENCE,
   VIEW_FREESPEECH,
   VIEW_GENETICS,
   VIEW_JUSTICES,
   VIEW_GUNCONTROL,
   VIEW_SWEATSHOPS,
   VIEW_POLLUTION,
   VIEW_CORPORATECULTURE,
   VIEW_CEOSALARY,
   VIEW_WOMEN,//XXX: VIEW_ABORTION DOES NOT EXIST
   VIEW_CIVILRIGHTS,
   VIEW_DRUGS,
   VIEW_IMMIGRATION,
   VIEW_MILITARY,
   VIEW_PRISONS,
   //*JDS* I'm using VIEWNUM-5 in a random generator that rolls a
   //random issue, not including the media/politicalviolence ones, and this will
   //break if these stop being the last 4 issues; do a search
   //for VIEWNUM-5 to change it if it needs to be changed.
   VIEW_AMRADIO,
   VIEW_CABLENEWS,
   //VIEW_POLITICALVIOLENCE,
   //THESE THREE MUST BE LAST FOR VIEWNUM-3 TO WORK IN PLACES
   VIEW_LIBERALCRIMESQUAD,
   VIEW_LIBERALCRIMESQUADPOS,
   //THIS ONE MUST BE LAST. randomissue RELIES ON IT BEING LAST TO IGNORE IT IF
   //CCS IS DEAD.
   VIEW_CONSERVATIVECRIMESQUAD,
   VIEWNUM
};

enum Laws
{
   LAW_STALIN=-2, // not a real law: this is -2 and is actually calculated based on views >=0 and <VIEWNUM-3
   LAW_MOOD, // not a real law: this is -1 and is likewise calculated based on views >=0 and <VIEWNUM-3
   LAW_ABORTION, // law #0, the first one that is actually in the law[] array
   LAW_ANIMALRESEARCH,
   LAW_POLICEBEHAVIOR,
   LAW_PRIVACY,
   LAW_DEATHPENALTY,
   LAW_NUCLEARPOWER,
   LAW_POLLUTION,
   LAW_LABOR,
   LAW_GAY,
   LAW_CORPORATE,
   LAW_FREESPEECH,
   LAW_FLAGBURNING,
   LAW_GUNCONTROL,
   LAW_TAX,
   LAW_WOMEN,
   LAW_CIVILRIGHTS,
   LAW_DRUGS,
   LAW_IMMIGRATION,
   LAW_ELECTIONS,
   LAW_MILITARY,
   LAW_PRISONS,
   LAW_TORTURE,
   LAWNUM
};

enum FieldSkillRates
{
	FIELDSKILLRATE_FAST,
	FIELDSKILLRATE_CLASSIC,
	FIELDSKILLRATE_HARD
};

enum Lawflags
{
   LAWFLAG_TREASON,      // "Treason"
   LAWFLAG_TERRORISM,    // "Terrorism"
   LAWFLAG_MURDER,       // "Murder"
   LAWFLAG_KIDNAPPING,   // "Kidnapping"
   LAWFLAG_BANKROBBERY,  // "Bank robbery"
   LAWFLAG_ARSON,        // "Arson"
   LAWFLAG_BURNFLAG,     // "Flag burning" / "Flag Murder"
   LAWFLAG_SPEECH,       // "Harmful speech"
   LAWFLAG_BROWNIES,     // "Drug dealing"
   LAWFLAG_ESCAPED,      // "Escaping prison"
   LAWFLAG_HELPESCAPE,   // "Releasing prisoners"
   LAWFLAG_JURY,         // "Jury tampering"
   LAWFLAG_RACKETEERING, // "Racketeering"
   LAWFLAG_EXTORTION,    // "Extortion"
   LAWFLAG_ARMEDASSAULT, // "Armed assault"
   LAWFLAG_ASSAULT,      // "Assault"
   LAWFLAG_CARTHEFT,     // "Grand theft auto"
   LAWFLAG_CCFRAUD,      // "Credit card fraud"
   LAWFLAG_THEFT,        // "Theft"
   LAWFLAG_PROSTITUTION, // "Prostitution"
   LAWFLAG_HIREILLEGAL,  // "Hiring illegal aliens" / "Hiring undocumented workers"
   //LAWFLAG_GUNUSE,     // "Firing illegal weapons"
   //LAWFLAG_GUNCARRY,   // "Carrying illegal weapons"
   LAWFLAG_COMMERCE,     // "Electronic sabotage"
   LAWFLAG_INFORMATION,  // "Hacking"
   LAWFLAG_BURIAL,       // "Unlawful burial"
   LAWFLAG_BREAKING,     // "Breaking and entering"
   LAWFLAG_VANDALISM,    // "Vandalism"
   LAWFLAG_RESIST,       // "Resisting arrest"
   LAWFLAG_DISTURBANCE,  // "Disturbing the peace"
   LAWFLAG_PUBLICNUDITY, // "Public nudity"
   LAWFLAG_LOITERING,    // "Loitering"
   LAWFLAGNUM
};

/// game variables

extern short law[LAWNUM];

extern char slogan[80];

extern short fieldskillrate;

/// end game variables

#define RNG_SIZE 4
#define SQUAD_MAX 6
#define ENCMAX 18

#define MIN(a,b) ((a) < (b) ? (a) : (b))

// functions

int min(int a, int b);
int max(int a, int b);


#endif
