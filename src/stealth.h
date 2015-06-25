#ifndef STEALTH_H
#define STEALTH_H

#include "entity.h"

void noticecheck(struct t_creature* p, struct t_creature* e, int exclude,int difficulty);
int disguisecheck(struct t_creature* p, struct t_creature* e, int timer, bool restricted_space);
char weaponcheck(struct t_creature* cr, bool metaldetect, bool restricted_space);
char disguisesite(long type);

#endif
