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

int xdx(int dice, int sides) {

	int n = 0;

	for (int i=0; i < dice; i++)
		n += randval(sides) + 1;

	return n;
}
