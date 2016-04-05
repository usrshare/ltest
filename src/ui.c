// vim: cin:sts=4:sw=4 
#include "ui.h"
#include "globals.h"

#include "map_ui.h"

#include <stdarg.h>
#include <time.h>

int g_addstr (const char* str, void* log) {

	switch(curui) {
		case UI_MAPMODE:
			return msgaddstr(str);
	}
	return 1;
}

int g_printw (const char* fmt, ...) {

	int r = 0;

	va_list vargs;
	va_start (vargs, fmt);

	switch(curui) {
		case UI_MAPMODE:
			return msgvprintw(fmt,vargs);
	}

	va_end(vargs);
	return r;
} 

int g_addint (int val, void* log) {
	return g_printw("%d",val);
}

int g_getkey() {
	switch(curui) {
		case UI_MAPMODE:
			return mapgetch();
	}
	return 0;
}

int g_attrset(int attrs) {
	switch(curui) {
		case UI_MAPMODE:
			return msgattrset(attrs);
	}
	return 1;
}


char teststr[12];

int ms_sleep(int ms) {

	struct timespec ts;
	ts.tv_sec = 0; ts.tv_nsec = (ms * 100000);

	return nanosleep(&ts,NULL);
}

const char* tostring(int num) {
	snprintf(teststr,12,"%d",num);
	return teststr;
}

