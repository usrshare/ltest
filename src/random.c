// vim: cin:sts=4:sw=4 
#include "random.h"
#include <stdlib.h>

int randval(int max) {

	int n = rand();	
	return n % max;
}

int randbetween(int min, int max) {

	int n = rand();
	return min + (n % (max - min + 1));
}

int randrange(struct t_range* range) {
    return randbetween(range->min,range->max);
}

int xdx(int dice, int sides) {

	int n = 0;

	for (int i=0; i < dice; i++)
		n += randval(sides) + 1;

	return n;
}
