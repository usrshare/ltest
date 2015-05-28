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

const char* pickrandom(const char* array[]) {
    return array[randval(sizeof(array) / sizeof(*array))];
}

int roll_check(int skill)
{
   // This die rolling system (and the associated difficulty
   // ratings) is adapted from EABA, which uses a system of
   // rolling a number of six-sided dice equal to the ability
   // score divided by three. The top three dice are used,
   // the rest discarded. Finally, any additional points that
   // didn't divide evenly into creating a die contribute to
   // odd-shaped dice that don't exist in the real world. This
   // system gives diminishing returns for high skill levels.
   // EABA actually just adds the remainder to the die total,
   // but there are some statistical problems with that system.

   // It is not possible to roll above an 18 using this system.

   // It is possible to roll below a 3, if you don't have at
   // least 9 skill.

   int dice    = skill/3;
   int total   = 0;
   int roll[3] = {0,0,0};

   for(int i=0;i<dice+1;i++)
   {
      int newroll = 0;

      // Roll d6 for every three skill
      if(i<dice)
         newroll = randval(6)+1;
      // Partial dice for the remainder -- d3 for +1, d5 for +2
      else if(skill%3)
         newroll = randval((skill%3)*2+1)+1;

      // Keep the top three dice
      if(i<3)
         roll[i]=newroll;
      else
         for(int j=0;j<3;j++)
            if(newroll>roll[j])
            {
               int temp=roll[j];
               roll[j]=newroll;
               newroll=temp;
            }
   }

   for(int i=0;i<3;i++)
      total += roll[i];

   return total;
}

int xdx(int dice, int sides) {

	int n = 0;

	for (int i=0; i < dice; i++)
		n += randval(sides) + 1;

	return n;
}
