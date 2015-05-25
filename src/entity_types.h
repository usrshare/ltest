#ifndef ENTITY_TYPES_H
#define ENTITY_TYPES_H
#include "entity.h"

// Based on Liberal Crime Squad's creature/creature.h file.

enum entity_types {
   ET_BOUNCER,
   ET_SECURITYGUARD,
   ET_SCIENTIST_LABTECH,
   ET_SCIENTIST_EMINENT,
   ET_CORPORATE_MANAGER,
   ET_CORPORATE_CEO,
   ET_WORKER_SERVANT,
   ET_WORKER_JANITOR,
   ET_WORKER_SWEATSHOP,
   ET_WORKER_FACTORY_NONUNION,
   ET_WORKER_FACTORY_CHILD,
   ET_WORKER_SECRETARY,
   ET_WORKER_FACTORY_UNION,
   ET_LANDLORD,
   ET_TEENAGER,
   ET_COP,
   ET_SWAT,
   ET_DEATHSQUAD,
   ET_FIREFIGHTER,
   ET_EDUCATOR,
   ET_GANGUNIT,
   ET_JUDGE_LIBERAL,
   ET_JUDGE_CONSERVATIVE,
   ET_AGENT,
   ET_RADIOPERSONALITY,
   ET_NEWSANCHOR,
   ET_GENETIC,
   ET_GUARDDOG,
   ET_PRISONER,
   ET_JUROR,
   ET_LAWYER,
   ET_SEWERWORKER,
   ET_COLLEGESTUDENT,
   ET_MUSICIAN,
   ET_MATHEMATICIAN,
   ET_TEACHER,
   ET_HSDROPOUT,
   ET_BUM,
   ET_GANGMEMBER,
   ET_CRACKHEAD,
   ET_PRIEST,
   ET_ENGINEER,
   ET_FASTFOODWORKER,
   ET_BAKER,
   ET_BARISTA,
   ET_BARTENDER,
   ET_TELEMARKETER,
   ET_CARSALESMAN,
   ET_OFFICEWORKER,
   ET_FOOTBALLCOACH,
   ET_PROSTITUTE,
   ET_MAILMAN,
   ET_GARBAGEMAN,
   ET_PLUMBER,
   ET_CHEF,
   ET_CONSTRUCTIONWORKER,
   ET_AMATEURMAGICIAN,
   ET_TANK,
   ET_MERC,
   ET_HICK,
   ET_SOLDIER,
   ET_VETERAN,
   ET_HARDENED_VETERAN,
   ET_PRISONGUARD,
   ET_HIPPIE,
   ET_CRITIC_ART,
   ET_CRITIC_MUSIC,
   ET_SOCIALITE,
   ET_PROGRAMMER,
   ET_RETIREE,
   ET_PAINTER,
   ET_SCULPTOR,
   ET_AUTHOR,
   ET_JOURNALIST,
   ET_DANCER,
   ET_PHOTOGRAPHER,
   ET_CAMERAMAN,
   ET_HAIRSTYLIST,
   ET_FASHIONDESIGNER,
   ET_CLERK,
   ET_THIEF,
   ET_ACTOR,
   ET_YOGAINSTRUCTOR,
   ET_MARTIALARTIST,
   ET_ATHLETE,
   ET_BIKER,
   ET_TRUCKER,
   ET_TAXIDRIVER,
   ET_NUN,
   ET_MUTANT,
   ET_DOCTOR,
   ET_NURSE,
   ET_CCS_VIGILANTE,
   ET_CCS_ARCHCONSERVATIVE,
   ET_POLITICALACTIVIST,
   ET_CCS_MOLOTOV,
   ET_CCS_SNIPER,
   ET_PSYCHOLOGIST,
   ET_LOCKSMITH,
   ET_MILITARYPOLICE,
   ET_SEAL,
   ET_MILITARYOFFICER,
   ET_BANK_TELLER,
   ET_BANK_MANAGER,
   ET_SECRET_SERVICE,
   ET_POLITICIAN,
   ENTT_COUNT
};

const char* type_description (struct t_entity* e);

extern struct t_entity_generate_rules type_rules[ENTT_COUNT];

#endif
