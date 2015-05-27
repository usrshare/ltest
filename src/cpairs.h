// vim: cin:sts=4:sw=4 
#ifndef CPAIRS_H
#define CPAIRS_H

#include <curses.h>

#define CP_GREEN COLOR_PAIR(1)
#define CP_CYAN COLOR_PAIR(2)
#define CP_BLUE COLOR_PAIR(3)
#define CP_YELLOW COLOR_PAIR(4)
#define CP_PURPLE COLOR_PAIR(5)
#define CP_RED COLOR_PAIR(6)
#define CP_WHITE COLOR_PAIR(7)
#define CP_DARKGRAY COLOR_PAIR(8)
#define CP_LIGHTGRAY COLOR_PAIR(9)

int init_pairs();

#endif
