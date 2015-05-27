#include "ui.h"
#include "globals.h"

#include "map_ui.h"

#include <stdarg.h>

int g_addstr (const char* str, void* log) {

	switch(curui) {
		case UI_MAPMODE:
			return stat_addstr(str);
	}
}

int g_printw (const char* fmt, ...) {

	int r = 0;

	va_list vargs;
	va_start (vargs, fmt);

	switch(curui) {
		case UI_MAPMODE:
			return statvprintw(str);
	}

	va_end(vargs);
	return r;
}

int g_addint (int val, void* log) {
	return g_printw("%d",str);
}

int g_getkey() {
	switch(curui) {
		case UI_MAPMODE:
			return mapgetch();
	}

}

int g_attrset(int attrs) {
	switch(curui) {
		case UI_MAPMODE:
			return statattrset(statwindow,attrs);
	}

}

char teststr[12];

int tostring(int num) {
	sprintf(teststr,12,"%d",num);
	return teststr;
}

