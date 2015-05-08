#include "random.h"

int randval(int max) {
	int n = rand();	
	return n % max;
}

int xdx(int dice, int sides) {

	int n = 0;

	for (int i=0; i < dice; i++)
		n += randval(sides) + 1;

	return n;
}
