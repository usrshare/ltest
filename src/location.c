#include "location.h"

const struct siegest siegest_def = {
	.siege = 0,
	.siegetype = -1,
	.underattack = 0,
	.escalationstate = 0,
	.timeuntillocated = -1,
	.timeuntilcorps = -1,
	.timeuntilcia = -1,
	.timeuntilccs = -1,
	.timeuntilfiremen = -1,
};

struct t_location* location[LOC_COUNT];
