// vim: cin:sts=4:sw=4 
// vim: cin:sts=4:sw=4 
#include "log.h"

FILE* logfile = NULL;

void log_atexit(void) {

	fprintf(logfile, "Terminating.\n");
	if (logfile) fclose(logfile);
}

int log_init(void) {

	logfile = fopen("ltest.log","a");
	
	if (!logfile) return 1;

	fprintf(logfile, "--------\nltest started.\n");

	atexit(log_atexit);
	return 0;
}



int vlprintf(const char *fmt, va_list ap) {

	if (!logfile) return -1;
    int r = vfprintf(logfile,fmt,ap);
    return r;
}

int lprintf (const char* fmt, ...) {

	int r = 0;

	va_list vargs;
	va_start (vargs, fmt);

	return vlprintf(fmt,vargs);

	va_end(vargs);
	return r;
} 
