#include "entity.h"

#include <malloc.h>
#include "globals.h"
#include "random.h"

uint32_t curcreatureid = 0;

/*
   Copyright (c) 2002,2003,2004 by Tarn Adams                                         //
   Copyright (c) 2015 usr_share
//
This file is part of Liberal Crime Squad.                                           //
//
Liberal Crime Squad is free software; you can redistribute it and/or modify     //
it under the terms of the GNU General Public License as published by            //
the Free Software Foundation; either version 2 of the License, or               //
(at your option) any later version.                                             //
//
Liberal Crime Squad is distributed in the hope that it will be useful,          //
but WITHOUT ANY WARRANTY; without even the implied warranty of                  //
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the                  //
GNU General Public License for more details.                                    //
//
You should have received a copy of the GNU General Public License               //
along with Liberal Crime Squad; if not, write to the Free Software              //
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   02111-1307   USA     //
 */

/*
   This file was created by Chris Johnson (grundee@users.sourceforge.net)
   by copying code from game.cpp.
   To see descriptions of files and functions, see the list at
   the bottom of includes.h in the top src folder.
 */

// this file based on creature/creature.cpp from Liberal Crime Squad

struct t_entity* creature_init() {

	struct t_entity* newent = malloc(sizeof(struct t_entity));

	newent->age = 18 + randval(40);
	newent->gender_bio = randbetween(EG_MALE,EG_FEMALE);
	if (randval(100) < 1) 
		newent->gender_id = randbetween(EG_NEUTRAL,EG_FEMALE); else newent->gender_id = newent->gender_bio;

	newent->birthday_month = randval(12) + 1;

	switch(newent->birthday_month)
	{
		case 4:
		case 6:
		case 9:
		case 11:
			newent->birthday_day=randval(30)+1;
			break;
		case 2:
			newent->birthday_day=randval(28)+1;
			break;
		default:
			newent->birthday_day=randval(31)+1;
			break;
	}

   	newent->id=curcreatureid++;

	for(int w=0;w<EB_COUNT;w++)newent->wound[w]=0;

   for(int a=0;a<EA_COUNT;a++)
   {
      newent->attrvalue[a]=1;
   }
   int attnum=32;
   while(attnum>0)
   {
      int a=randval(EA_COUNT);
      if(newent->attrvalue[a]<10)
      {
         newent->attrvalue[a]++;
         attnum--;
      }
   }

   newent->special[ESW_TEETH]=TOOTHNUM;
   newent->special[ESW_RIGHTEYE]=1;
   newent->special[ESW_LEFTEYE]=1;
   newent->special[ESW_NOSE]=1;
   newent->special[ESW_TONGUE]=1;
   newent->special[ESW_RIGHTLUNG]=1;
   newent->special[ESW_LEFTLUNG]=1;
   newent->special[ESW_HEART]=1;
   newent->special[ESW_LIVER]=1;
   newent->special[ESW_STOMACH]=1;
   newent->special[ESW_RIGHTKIDNEY]=1;
   newent->special[ESW_LEFTKIDNEY]=1;
   newent->special[ESW_SPLEEN]=1;
   newent->special[ESW_RIBS]=RIBNUM;
   newent->special[ESW_NECK]=1;
   newent->special[ESW_UPPERSPINE]=1;
   newent->special[ESW_LOWERSPINE]=1;

}
