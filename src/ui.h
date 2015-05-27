#ifndef UI_H
#define UI_H

#include "cpairs.h"

//generic UI functions, not bound to any mode

int g_addstr(const char *str, void* log);
int g_printw(const char *fmt, ...);
int g_getkey();
int g_attrset(int attrs);

const char* tostring(int num); //uses a static non-thread-safe string

#endif
