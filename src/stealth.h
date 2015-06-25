#ifndef STEALTH_H
#define STEALTH_H

#include "entity.h"

void noticecheck(struct t_creature* p, struct t_creature* e, int exclude,int difficulty);
char weaponcheck(struct t_creature* cr, bool metaldetect);
char disguisesite(long type);

#endif
