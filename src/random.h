// vim: cin:sts=4:sw=4 
#ifndef RANDOM_H
#define RANDOM_H

struct t_range {
	// this is a structure that represents a range of values.
	// if both values are 0, the entity generator shall ignore this range.
	int min;
	int max;
};

int randval(int max);
int randrange(struct t_range* range);
int randbetween(int min, int max);

int xdx(int dice, int sides);

#endif
