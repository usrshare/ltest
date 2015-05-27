// vim: cin:sts=4:sw=4 
#include "fight.h"

/*

   Copyright (c) 2002,2003,2004 by Tarn Adams                                            //
   Copyright (c) 2015 usr_share
//
This file is part of Liberal Crime Squad.                                             //
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

// based on src/combat/fight.cpp from Liberal Crime Squadi

#include "globals.h"
#include "ui.h"
#include "entity.h"
#include <stdbool.h>
#include <string.h>

bool goodguyattack = false;

/* attack handling for an individual creature and its target */
void attack(struct t_entity* a,struct t_entity* t,char mistake,char* actual,bool force_melee)
{
    *actual=0;

    char str[200];

    //clearmessagearea(true);  // erase the whole length and redraw map if applicable, since previous combat messages can be wider than 53 chars.
    //if(goodguyattack) g_attrset(CP_GREEN); else g_attrset(CP_RED);

    //INCAPACITATED
    char incaprint;
    a->forceinc=0;
    if(incapacitated(a,0,&incaprint))
    {
	/*
	if(incaprint)
	{
	    printparty();
	    if(mode==GM_CHASECAR||
		    mode==GM_CHASEFOOT) printchaseencounter();
	    else printencounter();

	    g_getkey();
	}

	a.forceinc=1;

	return;
	*/
    }

    //SPECIAL ATTACK!

    /*
    int encnum=0;
    for(int e=0;e<ENCMAX;e++) if(encounter[e].exists) encnum++;

    if(!force_melee &&
	    (((a.type==CREATURE_COP&&a.align==ALIGN_MODERATE&&a.enemy())||
	      a.type==CREATURE_SCIENTIST_EMINENT||
	      a.type==CREATURE_JUDGE_LIBERAL||
	      a.type==CREATURE_JUDGE_CONSERVATIVE||
	      (a.type==CREATURE_CORPORATE_CEO&&randval(2))||
	      a.type==CREATURE_POLITICIAN||
	      a.type==CREATURE_RADIOPERSONALITY||
	      a.type==CREATURE_NEWSANCHOR||
	      a.type==CREATURE_MILITARYOFFICER||
	      a->weapon.has_musical_attack()) &&
	     (a->weapon.has_musical_attack() || !a.is_armed() || a.align!=1)))
    {
	if(a.align==1||encnum<ENCMAX)
	{
	    specialattack(a, t, actual);
	    return;
	}
    }
    */

    //RELOAD
    /*if((a.will_reload(mode==GM_CHASECAR,force_melee)
		||(a.has_thrown_weapon && len(a.extra_throwing_weapons)))
	    &&!force_melee)
    {
	if(a.will_reload(mode==GM_CHASECAR,force_melee))
	{
	    a.reload(false);
	    strcpy(str,describe_entity_static(a));
	    strcat(str," reloads.");
	}
	else if(a.has_thrown_weapon && len(a.extra_throwing_weapons))
	{
	    a.ready_another_throwing_weapon();
	    strcpy(str,describe_entity_static(a));
	    strcat(str," readies another ");
	    strcat(str,get_weapon_name(a->weapon,));
	    strcat(str,".");
	}
	//move(16,1);
	g_addstr(str, gamelog);
	g_addstr("\n",NULL);

	printparty();
	if(mode==GM_CHASECAR
		||mode==GM_CHASEFOOT) printchaseencounter();
	else printencounter();

	g_getkey();

	return;
    }*/

    else if(a->has_thrown_weapon) a->has_thrown_weapon = false;

    struct t_attackst* attack_used = NULL;
    attack_used = get_attack(get_weapon(a),mode==GM_CHASECAR, force_melee, (force_melee || !can_reload(a))); 

    //Force ranged if in a car.
    //No reload if force melee or unable to reload.

    if(attack_used==NULL) //No attack possible.
	return;            //Then bail, they can't fight

    bool melee=true;
    if(attack_used->ranged) melee=false;
    bool sneak_attack=false;

    strcpy(str,describe_entity_static(a));
    strcat(str," ");
    if(mistake) strcat(str,"MISTAKENLY ");
    if(!a->weapon)
    {
	if(!a->animalgloss) //Move into WEAPON_NONE -XML
	{
	    if(!randval(entity_get_skill(a,ES_HANDTOHAND)+1))
		strcat(str,"punches");
	    else if(!randval(entity_get_skill(a,ES_HANDTOHAND)))
		strcat(str,"swings at");
	    else if(!randval(entity_get_skill(a,ES_HANDTOHAND)-1))
		strcat(str,"grapples with");
	    else if(!randval(entity_get_skill(a,ES_HANDTOHAND)-2))
		strcat(str,"kicks");
	    else if(!randval(entity_get_skill(a,ES_HANDTOHAND)-3))
		strcat(str,"strikes at");
	    else if(!randval(entity_get_skill(a,ES_HANDTOHAND)-4))
		strcat(str,"jump kicks");
	    else strcat(str,"gracefully strikes at");
	}
	else
	{
	    if(a->specialattack==ATTACK_CANNON)
	    {
		strcat(str,"fires a 120mm shell at");
		melee=false;
	    }
	    else if(a->specialattack==ATTACK_FLAME) strcat(str,"breathes fire at");
	    else if(a->specialattack==ATTACK_SUCK) strcat(str,"stabs");
	    else strcat(str,"bites");
	}
    }
    else
    {
	if(attack_used->can_backstab && a->align==ALIGN_LIBERAL && !mistake)
	{
	    if(t->cantbluff<1 && sitealarm<1)
	    {
		sneak_attack = true;
		strcat(str,"sneaks up on");
		if(sitealarmtimer>10 || sitealarmtimer<0) sitealarmtimer=10;
		t->cantbluff = 2;
	    }
	}

	if(!sneak_attack)
	{
	    strcat(str,attack_used->attack_description);
	    sitealarm=1;
	}
    }

    strcat(str," ");
    strcat(str,describe_entity_static(t));
    //move(16,1);
    g_addstr(str, gamelog);

    strcpy(str,"");

    if(a->weapon && !attack_used->thrown)
    {
	strcat(str," with a ");
	strcat(str,get_weapon_name(a->weapon,1));
    }
    strcat(str,"!");
    g_addstr(str, gamelog);
    g_addstr("\n",NULL);

    g_getkey();

    if(goodguyattack) g_attrset(CP_GREEN);
    else g_attrset(CP_RED);
    strcpy(str, entity_heshe(a,true)); // capitalize=true. Shorten the string so it doesn't spill over as much; we already said attacker's name on the previous line anyways.

    int bonus=0; // Accuracy bonus or penalty that does NOT affect damage or counterattack chance

    //SKILL EFFECTS
    int wsk=attack_used->skill;

    struct t_entity* driver = getChaseDriver(t);
    struct t_vehicle* vehicle = getChaseVehicle(t);
    struct t_entity* adriver = getChaseDriver(a);
    struct t_vehicle* avehicle = getChaseVehicle(a);

    // Basic roll
    int aroll=entity_skill_roll(a,wsk);
    // In a car chase, the driver provides the defence roll instead of the victim.
    int droll= 0;
    if (mode!=GM_CHASECAR)
    {
	droll = entity_skill_roll(t,ES_DODGE)/2;
    }
    else
    {
	if (driver!=NULL && vehicle!=NULL)
	{  // without a vehicle or driver, you get a zero roll.
	    droll = entity_skill_roll(driver,ES_P_DODGEDRIVE);
	}
	if (adriver!=NULL && avehicle!=NULL)
	{
	    //bonus += avehicle->attackbonus(adriver->id == a->id);  // Attack bonus depends on attacker's car and whether attacker is distracted by driving.
	}
	else // shouldn't happen
	{
	    bonus -= 10; // You're on the wrong side of a drive-by shooting?!
	}
    }

    if(sneak_attack)
    {
	droll=entity_attr_roll(t,EA_WIS)/2;
	aroll+=entity_skill_roll(a,ES_STEALTH);
	entity_train(a,wsk,10);
    }
    else
    {
	if (driver!=NULL)
	    entity_train(driver,ES_DRIVING, aroll/2);
	else
	    entity_train(t,ES_DODGE,aroll*2);
	entity_train(a,wsk,droll*2+5);
    }

    // Hostages interfere with attack
    if(t->prisoner!=NULL) bonus-=randval(10);
    if(a->prisoner!=NULL) aroll-=randval(10);

    //Injured people suck at attacking, are like fish in a barrel to attackers
    if(mode==GM_CHASEFOOT)
    {
	// If in a foot chase, double the debilitating effect of injuries
	healthmodroll(&aroll,a);
	healthmodroll(&droll,t);
	healthmodroll(&droll,t);
    }
    else if (mode==GM_CHASECAR)
    {
	// In a car chase, the driver is applying dodge rolls even for crippled people.
	healthmodroll(&aroll,a);
	if (driver != NULL)
	{// if there is no driver, we already rolled a zero, so don't worry about further penalties.
	    healthmodroll(&droll,driver);
	}
    }
    else
    {  
	// Any other case (site fight) normal penalties.
	healthmodroll(&aroll,a);
	healthmodroll(&droll,t);
    }

    // Prevent negative rolls
    if(aroll<0) aroll=0;
    if(droll<0) droll=0;

    // Weapon accuracy bonuses and penalties
    bonus+=attack_used->accuracy_bonus;

    //USE BULLETS
    int bursthits=0; // Tracks number of hits.

    int thrownweapons=0; // Used by thrown weapons to remove the weapons at the end of the turn if needed

    if(!a->weapon) //Move into WEAPON_NONE -XML
    {
	// Martial arts multi-strikes
	bursthits=1+randval(entity_get_skill(a,ES_HANDTOHAND)/3+1);
	if(bursthits>5) bursthits=5;
	if(a->animalgloss) bursthits=1; // Whoops, must be human to use martial arts fanciness
    }
    else
    {
	if(mode==GM_SITE && randval(100) < attack_used->fire.chance_causes_debris)
	{// TODO - In a car chase, debris should make driving harder for one round, or require a drive skill check to avoid damage
	    //sitechangest change(locx,locy,locz,SITEBLOCK_DEBRIS);
	    //location[cursite]->changes.push_back(change);
	}
	if(mode==GM_SITE && randval(100) < attack_used->fire.chance)
	{// TODO - In a car chase, apply vehicle damage, with drive skill check to partially mitigate
	    // Fire!

	    /*
	    if(!(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_END) ||
		    !(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_PEAK) ||
		    !(levelmap[locx][locy][locz].flag & SITEBLOCK_FIRE_START) ||
		    !(levelmap[locx][locy][locz].flag & SITEBLOCK_DEBRIS))
	    {
		levelmap[locx][locy][locz].flag|=SITEBLOCK_FIRE_START;
		sitecrime+=3;
		addjuice(a,5,500);
		criminalizeparty(LAWFLAG_ARSON);
		sitestory->crime.push_back(CRIME_ARSON);
	    }
	    */
	}

	for(int i=0;i<attack_used->number_attacks;i++)
	{
	    if(attack_used->uses_ammo)
	    {
		if(a->weapon->ammo > 0)
		    a->weapon->ammo -= 1;
		else break;
	    }
	    else if(attack_used->thrown)
	    {
		if(entity_count_weapons(a)-thrownweapons > 0)
		    thrownweapons++;
		else break;
	    }

	    if(sneak_attack)
	    {
		bursthits=1; // Backstab only hits once
		break;
	    }
	    // Each shot in a burst is increasingly less likely to hit
	    if(aroll+bonus-i*attack_used->successive_attacks_difficulty > droll)
		bursthits++;
	}
    }

    //HIT!
    if(aroll+bonus>droll)
    {
	if(sneak_attack) strcat(str, " stabs ");
	else strcat(str," hits ");
	strcat(str,describe_entity_static(t));
	strcat(str, "'s ");
	int w;
	bool canhit=false;

	for(w=0;w<EB_COUNT;w++)
	{
	    if(!(t->wound[w]&WOUND_CLEANOFF) && !(t->wound[w]&WOUND_NASTYOFF))
	    {
		canhit=true;
		break;
	    }
	}

	do
	{
	    int offset=0;
	    if(aroll>droll+5)
		offset=4;  // NICE SHOT; MORE LIKELY TO HIT BODY/HEAD
	    if(aroll>droll+10 &&
		    (!(t->wound[EB_HEAD]&(WOUND_CLEANOFF|WOUND_NASTYOFF)) ||
		     !(t->wound[EB_BODY]&(WOUND_CLEANOFF|WOUND_NASTYOFF))))
		offset=8;  // NO LIMB HITS HERE YOU AWESOME PERSON
	    if(sneak_attack)
		offset=10; // Backstab! 2/3 body, 1/3 head
	    if(aroll>droll+15 &&
		    !(t->wound[EB_HEAD]&(WOUND_CLEANOFF|WOUND_NASTYOFF)))
		offset=12; // BOOM AUTOMATIC HEADSHOT MOTHA******
	    //Weighted location roll:
	    //200% chance to hit body
	    //50% chance to hit head
	    w=offset+randval(13-offset);

	    switch(w)
	    {
		case 12:w=EB_HEAD; break;
		case 11:
		case 10:
		case 9:
		case 8:w=EB_BODY; break;
		case 7:
		case 3:w=EB_ARM_RIGHT; break;
		case 6:
		case 2:w=EB_ARM_LEFT; break;
		case 5:
		case 1:w=EB_LEG_RIGHT; break;
		case 4:
		case 0:w=EB_LEG_LEFT; break;
	    }
	} while(((t->wound[w]&WOUND_CLEANOFF) || (t->wound[w]&WOUND_NASTYOFF)) && canhit==true);

	if(t->animalgloss==ANIMALGLOSS_TANK)
	    switch(w)
	    {
		case EB_HEAD:strcat(str,"turret"); break;
		case EB_BODY:strcat(str,"front"); break;
		case EB_ARM_RIGHT:strcat(str,"right side"); break;
		case EB_ARM_LEFT:strcat(str,"left side"); break;
		case EB_LEG_RIGHT:strcat(str,"right tread"); break;
		case EB_LEG_LEFT:strcat(str,"left tread"); break;
	    }
	else if(t->animalgloss==ANIMALGLOSS_ANIMAL) // FIXME: What about Six-legged Pigs?
	    switch(w)
	    {
		case EB_HEAD:strcat(str,"head"); break;
		case EB_BODY:strcat(str,"body"); break;
		case EB_ARM_RIGHT:strcat(str,"right front leg"); break;
		case EB_ARM_LEFT:strcat(str,"left front leg"); break;
		case EB_LEG_RIGHT:strcat(str,"right rear leg"); break;
		case EB_LEG_LEFT:strcat(str,"left rear leg"); break;
	    }
	else
	    switch(w)
	    {
		case EB_HEAD:strcat(str,"head"); break;
		case EB_BODY:strcat(str,"body"); break;
		case EB_ARM_RIGHT:strcat(str,"right arm"); break;
		case EB_ARM_LEFT:strcat(str,"left arm"); break;
		case EB_LEG_RIGHT:strcat(str,"right leg"); break;
		case EB_LEG_LEFT:strcat(str,"left leg");break;
	    }

	// show multiple hits
	if(bursthits>1 && a->weapon) // Only show if not melee
	{
	    strcat(str,", ");
	    if(!a->weapon) //Move into WEAPON_NONE? -XML
		strcat(str,"striking");
	    else strcat(str,attack_used->hit_description);

	    switch(bursthits)
	    {
		case 1: break;
		case 2: strcat(str," twice"); break;
		case 3: strcat(str," three times"); break;
		case 4: strcat(str," four times"); break;
		case 5: strcat(str," five times"); break;
		default: strcat(str," ");
			 strcat(str,tostring(bursthits));
			 strcat(str," times"); break;
	    }
	}
	else if(attack_used->always_describe_hit)
	{
	    strcat(str,", ");
	    strcat(str,attack_used->hit_description);
	}

	char damtype=0;
	int damamount=0;
	char strengthmin=1;
	char strengthmax=1;
	int severtype=-1;

	char damagearmor=0;
	char armorpiercing=0;
	int extraarmor=0;

	if (!a->weapon)
	{
	    strengthmin=5;
	    strengthmax=10;
	    while(bursthits) //Put into WEAPON_NONE -XML
	    {
		damamount+=randval(5+entity_get_skill(a,ES_HANDTOHAND))+1+entity_get_skill(a,ES_HANDTOHAND);
		bursthits--;
	    }
	    if(!a->animalgloss) damtype|=WOUND_BRUISED;
	    else
	    {
		if(a->specialattack==ATTACK_CANNON)
		{
		    damamount=randval(5000)+5000;
		    armorpiercing=20;
		    damtype|=WOUND_BURNED;
		    damtype|=WOUND_TORN;
		    damtype|=WOUND_SHOT;
		    damtype|=WOUND_BLEEDING;
		    strengthmin=0;
		    strengthmax=0;
		}
		else if(a->specialattack==ATTACK_FLAME) damtype|=WOUND_BURNED;
		else if(a->specialattack==ATTACK_SUCK) damtype|=WOUND_CUT;
		else damtype|=WOUND_TORN;
		severtype=WOUND_NASTYOFF;
	    }
	}
	else
	{
	    if(attack_used->bruises) damtype|=WOUND_BRUISED;
	    if(attack_used->cuts) damtype|=WOUND_CUT;
	    if(attack_used->burns) damtype|=WOUND_BURNED;
	    if(attack_used->tears) damtype|=WOUND_TORN;
	    if(attack_used->shoots) damtype|=WOUND_SHOT;
	    if(attack_used->bleeding) damtype|=WOUND_BLEEDING;

	    strengthmin=attack_used->strength_min;
	    strengthmax=attack_used->strength_max;
	    severtype=attack_used->severtype;
	    int random=attack_used->random_damage;
	    int fixed=attack_used->fixed_damage;
	    if(sneak_attack) fixed+=100;
	    if(bursthits >= attack_used->critical.hits_required
		    && randval(100) < attack_used->critical.chance)
	    {
		if(attack_used->critical.random_damage_defined)
		    random=attack_used->critical.random_damage;
		if(attack_used->critical.fixed_damage_defined)
		    fixed=attack_used->critical.fixed_damage;
		if(attack_used->critical.severtype_defined)
		    severtype=attack_used->critical.severtype;
	    }
	    while(bursthits > 0)
	    {
		damamount+=randval(random)+fixed;
		bursthits--;
	    }
	    damagearmor=attack_used->damages_armor;
	    armorpiercing=attack_used->armorpiercing;
	}

	// Coarse combat lethality reduction.
	//damamount/=2;

	if(t->squadid!=-1&&t->hireid==-1) // Plot Armor: if the founder is hit, inflict
	    damamount/=2;                // 1/2 damage, because founders are cool

	int mod=0;

	if(strengthmax>strengthmin)
	{
	    // Melee attacks: Maximum strength bonus, minimum
	    // strength to deliver full damage
	    int strength=entity_attr_roll(a,EA_STR);
	    if(strength>strengthmax) strength=(strengthmax+strength)/2;
	    mod+=strength-strengthmin;
	    armorpiercing+=(strength-strengthmin)/4;
	}

	//SKILL BONUS FOR GOOD ROLL
	mod+=aroll-droll;

	//DO THE HEALTH MOD ON THE WOUND
	mod-=entity_attr_roll(t,EA_CON);

	//Health and poor accuracy will only avoid critical hits, not stop low-damage attacks
	if(mod<0) mod=0;

	// In a car chase, the vehicle itself provides bonus armor
	int vehicleHitLocation = 0;
	/*if (mode==GM_CHASECAR && vehicle != NULL)
	{
	    vehicleHitLocation = vehicle->gethitlocation(w);
	    extraarmor = vehicle->armorbonus(vehicleHitLocation);
	    // TODO damage vehicle itself
	}*/

	//int cardmg = damamount;
	//damagemod(t,damtype,damamount,w,armorpiercing,mod,extraarmor);

	// Report vehicle protection effect
	/*if (mode==GM_CHASECAR && vehicle != NULL && extraarmor > 0)
	{
	    strcat(str, " through ");
	    // Could the vehicle have bounced that round on its own?
	    if (damamount==0)
	    {
		struct t_entity testDummy; // Spawn nude test dummy to see if body armor was needed to prevent damage
		damagemod(testDummy,damtype,cardmg,w,armorpiercing,mod,extraarmor);

		if (cardmg < 2) //fudge factor of 1 armor level due to randomness
		{
		    strcpy(str, "The attack bounces off ");
		}
	    }

	    strcat(str,"the "+vehicle->shortname()+"'s ");
	    strcat(str, vehicle->getpartname(vehicleHitLocation));
	}*/

	// Temporary debug output for the damage roll
#ifdef SHOWMECHANICS
	{
	    char str[200];
	    clearmessagearea();
	    //move(16,1);
	    g_addstr_f("(ATK %d, DEF %d, DAMMOD %d, DAMAGE %d, AP %d)",aroll,droll,mod,damamount,armorpiercing);

	    g_getkey();
	}
#endif
	// Bullets caught by armor should bruise instead of poke holes.
	if(damamount<4 && damtype & WOUND_SHOT)
	{
	    damtype &= ~(WOUND_SHOT|WOUND_BLEEDING);
	    damtype |= WOUND_BRUISED;
	}
	if(damamount>0)
	{
	    struct t_entity *target=0;

	    if(t->squadid!=-1&&t->hireid==-1&& //if the founder is hit...
		    (damamount>t->blood||damamount>=10)&& //and lethal or potentially crippling damage is done...
		    (w==EB_HEAD||w==EB_BODY)) //to a critical bodypart...
	    {
		//Oh Noes!!!! Find a liberal to jump in front of the bullet!!!
		for(int i=0;i<6;i++)
		{
		    if(activesquad->squad[i]==NULL) break;
		    if(activesquad->squad[i]==&t) break;
		    if(activesquad->squad[i]->get_attribute(EA_HRT,true)>8&&
			    activesquad->squad[i]->get_attribute(EA_AGI,true)>4)
		    {
			target=activesquad->squad[i];

			clearmessagearea();
			g_attrset(CP_GREEN);

			//move(16,1);
			g_addstr(describe_entity_static(target), gamelog);
			if(!t->alive) g_addstr(" misguidedly", gamelog);
			else g_addstr(" heroically", gamelog);
			g_addstr(" shields ", gamelog);
			g_addstr(describe_entity_static(t), gamelog);
			if(!t->alive) g_addstr("'s corpse", gamelog);
			g_addstr("!", gamelog);
			g_addstr("\n",NULL);

			addjuice(*target,10,1000);//Instant juice!! Way to take the bullet!!

			g_getkey();

			break;
		    }
		}
	    }
	    if(!target) target=&t;//If nobody jumps in front of the attack,

	    target->wound[w]|=damtype;

	    int severamount=100;

	    switch(w)
	    {
		case EB_HEAD:severamount=100;break;
		case EB_BODY:severamount=1000;break;
		case EB_ARM_RIGHT:severamount=200;break;
		case EB_ARM_LEFT:severamount=200;break;
		case EB_LEG_RIGHT:severamount=400;break;
		case EB_LEG_LEFT:severamount=400;break;
	    }

	    if(severtype!=-1 && damamount>=severamount)
		target->wound[w]|=(char)severtype;

	    if(w!=EB_HEAD && w!=EB_BODY && target->blood-damamount<=0 &&
		    target->blood>0)
	    {
		do
		{
		    if(randval(100) < attack_used->no_damage_reduction_for_limbs_chance)
			break;
		    else damamount>>=1;
		} while(target->blood-damamount<=0);
	    }

	    if(damagearmor) armordamage(target->get_armor(),w,damamount);

	    target->blood-=damamount;

	    //levelmap[locx][locy][locz].flag|=SITEBLOCK_BLOODY;

	    if((target->wound[EB_HEAD] & WOUND_CLEANOFF)||
		    (target->wound[EB_BODY] & WOUND_CLEANOFF)||
		    (target->wound[EB_HEAD] & WOUND_NASTYOFF)||
		    (target->wound[EB_BODY] & WOUND_NASTYOFF)||
		    target->blood<=0)
	    {
		if((w==EB_HEAD && target->wound[EB_HEAD] & WOUND_NASTYOFF)||
			(w==EB_BODY && target->wound[EB_BODY] & WOUND_NASTYOFF))
		    bloodblast(&target->get_armor());

		char alreadydead=!target->alive;

		if(!alreadydead)
		{
		    target->die();

		    if(t.align==-a.align)
			addjuice(a,5+t->juice/20,1000); // Instant juice
		    else addjuice(a,-(5+t->juice/20),-50);

		    if(target->squadid!=-1)
		    {
			if(target->align==1) stat_dead++;
		    }
		    else if(target->enemy()&&(t->animalgloss!=ANIMALGLOSS_ANIMAL||law[LAW_ANIMALRESEARCH]==2))
		    {
			stat_kills++;
			if(location[cursite]->siege.siege) location[cursite]->siege.kills++;
			if(location[cursite]->siege.siege && t->animalgloss==ANIMALGLOSS_TANK) location[cursite]->siege.tanks--;
			if(location[cursite]->renting==RENTING_CCS)
			{
			    if(target->type==CREATURE_CCS_ARCHCONSERVATIVE) ccs_boss_kills++;
			    ccs_siege_kills++;
			}
		    }
		    if(target->squadid==-1 &&
			    (target->animalgloss!=ANIMALGLOSS_ANIMAL||law[LAW_ANIMALRESEARCH]==2) &&
			    !sneak_attack)
		    {
			sitecrime+=10;
			sitestory->crime.push_back(CRIME_KILLEDSOMEBODY);
			if(a.squadid!=-1) criminalizeparty(LAWFLAG_MURDER);
		    }
		}

		if(w==EB_HEAD && target->wound[EB_HEAD] & WOUND_CLEANOFF)
		    strcat(str," CUTTING IT OFF!");
		else if(w==EB_BODY && target->wound[EB_BODY] & WOUND_CLEANOFF)
		    strcat(str," CUTTING IT IN HALF!");
		else if(w==EB_HEAD && target->wound[EB_HEAD] & WOUND_NASTYOFF)
		    strcat(str," BLOWING IT APART!");
		else if(w==EB_BODY && target->wound[EB_BODY] & WOUND_NASTYOFF)
		    strcat(str," BLOWING IT IN HALF!");
		else strcat(str,attack_used->hit_punctuation);
		//move(17,1);
		//set_color(COLOR_WHITE,COLOR_BLACK,1);
		if(goodguyattack) g_attrset(CP_GREEN);
		else g_attrset(CP_RED);
		g_addstr(str, gamelog);
		g_addstr("\n",NULL);

		g_getkey();

		if(!alreadydead)
		{
		    severloot(t,groundloot);
		    clearmessagearea();
		    adddeathmessage(*target);

		    g_getkey();

		    if(target->prisoner!=NULL) freehostage(t,1);
		}
	    }
	    else
	    {
		if(target->wound[w] & WOUND_CLEANOFF) strcat(str," CUTTING IT OFF!");
		else if(target->wound[w] & WOUND_NASTYOFF) strcat(str," BLOWING IT OFF!");
		else strcat(str,attack_used->hit_punctuation);

		if(target->wound[w] & WOUND_NASTYOFF) bloodblast(&target->get_armor());

		if(goodguyattack) g_attrset(CP_GREEN);
		else g_attrset(CP_RED);
		//move(17,1);
		//set_color(COLOR_WHITE,COLOR_BLACK,1);
		g_addstr(str, gamelog);
		g_addstr("\n",NULL);

		printparty();
		if(mode==GM_CHASECAR||
			mode==GM_CHASEFOOT) printchaseencounter();
		else printencounter();

		g_getkey();

		//SPECIAL WOUNDS
		if(!(target->wound[w] & (WOUND_CLEANOFF|WOUND_NASTYOFF))&&
			!target->animalgloss)
		{
		    char heavydam=0,breakdam=0,pokedam=0;
		    if(damamount>=12) //JDS -- 2x damage needed
		    {
			if(damtype & WOUND_SHOT) heavydam=1;
			if(damtype & WOUND_BURNED) heavydam=1;
			if(damtype & WOUND_TORN) heavydam=1;
			if(damtype & WOUND_CUT) heavydam=1;
		    }

		    if(damamount>=10) //JDS -- 2x damage needed
		    {
			if(damtype & WOUND_SHOT) pokedam=1;
			if(damtype & WOUND_TORN) pokedam=1;
			if(damtype & WOUND_CUT) pokedam=1;
		    }

		    if((damtype & WOUND_BRUISED)&&damamount>=50) breakdam=1;
		    if((damtype & WOUND_SHOT)&&damamount>=50) breakdam=1;
		    if((damtype & WOUND_TORN)&&damamount>=50) breakdam=1;
		    if((damtype & WOUND_CUT)&&damamount>=50) breakdam=1;

		    if(w==EB_HEAD)
		    {
			clearmessagearea();
			if(goodguyattack) g_attrset(CP_GREEN);
			else g_attrset(CP_RED);

			switch(randval(7))
			{
			    case 0:
				if((target->special[ESW_RIGHTEYE]||
					    target->special[ESW_LEFTEYE]||
					    target->special[ESW_NOSE])&&heavydam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s face is blasted off!", gamelog);
				    else if(damtype & WOUND_BURNED)g_addstr("'s face is burned away!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s face is torn off!", gamelog);
				    else if(damtype & WOUND_CUT)g_addstr("'s face is cut away!", gamelog);
				    else g_addstr("'s face is removed!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_RIGHTEYE]=0;
				    target->special[ESW_LEFTEYE]=0;
				    target->special[ESW_NOSE]=0;
				    if(target->blood>20)target->blood=20;
				}
				break;
			    case 1:
				if(target->special[ESW_TEETH]>0)
				{
				    int teethminus=randval(TOOTHNUM)+1;
				    if(teethminus>target->special[ESW_TEETH])
					teethminus=target->special[ESW_TEETH];

				    //move(16,1);
				    if(teethminus>1)
				    {
					if(teethminus==target->special[ESW_TEETH])
					    g_addstr("All ", gamelog);
					g_addstr(teethminus, gamelog);
					g_addstr(" of ", gamelog);
					g_addstr(describe_entity_static(target), gamelog);
					g_addstr("'s teeth are ", gamelog);
				    }
				    else if(target->special[ESW_TEETH]>1)
				    {
					g_addstr("One of ", gamelog);
					g_addstr(describe_entity_static(target), gamelog);
					g_addstr("'s teeth is ", gamelog);
				    }
				    else
				    {
					g_addstr(describe_entity_static(target), gamelog);
					g_addstr("'s last tooth is ", gamelog);
				    }

				    if(damtype & WOUND_SHOT)g_addstr("shot out!", gamelog);
				    else if(damtype & WOUND_BURNED)g_addstr("burned away!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("gouged out!", gamelog);
				    else if(damtype & WOUND_CUT)g_addstr("cut out!", gamelog);
				    else g_addstr("knocked out!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_TEETH]-=teethminus;
				}
				break;
			    case 2:
				if(target->special[ESW_RIGHTEYE]&&heavydam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s right eye is blasted out!", gamelog);
				    else if(damtype & WOUND_BURNED)g_addstr("'s right eye is burned away!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s right eye is torn out!", gamelog);
				    else if(damtype & WOUND_CUT)g_addstr("'s right eye is poked out!", gamelog);
				    else g_addstr("'s right eye is removed!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_RIGHTEYE]=0;
				    if(target->blood>50)target->blood=50;
				}
				break;
			    case 3:
				if(target->special[ESW_LEFTEYE]&&heavydam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s left eye is blasted out!", gamelog);
				    else if(damtype & WOUND_BURNED)g_addstr("'s left eye is burned away!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s left eye is torn out!", gamelog);
				    else if(damtype & WOUND_CUT)g_addstr("'s left eye is poked out!", gamelog);
				    else g_addstr("'s left eye is removed!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_LEFTEYE]=0;
				    if(target->blood>50)target->blood=50;
				}
				break;
			    case 4:
				if(target->special[ESW_TONGUE]&&heavydam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s tongue is blasted off!", gamelog);
				    else if(damtype & WOUND_BURNED)g_addstr("'s tongue is burned away!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s tongue is torn out!", gamelog);
				    else if(damtype & WOUND_CUT)g_addstr("'s tongue is cut off!", gamelog);
				    else g_addstr("'s tongue is removed!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_TONGUE]=0;
				    if(target->blood>50)target->blood=50;
				}
				break;
			    case 5:
				if(target->special[ESW_NOSE]&&heavydam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s nose is blasted off!", gamelog);
				    else if(damtype & WOUND_BURNED)g_addstr("'s nose is burned away!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s nose is torn off!", gamelog);
				    else if(damtype & WOUND_CUT)g_addstr("'s nose is cut off!", gamelog);
				    else g_addstr("'s nose is removed!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_NOSE]=0;
				    if(target->blood>50)target->blood=50;
				}
				break;
			    case 6:
				if(target->special[ESW_NECK]&&breakdam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s neck bones are shattered!", gamelog);
				    else g_addstr("'s neck is broken!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_NECK]=0;
				    if(target->blood>20)target->blood=20;
				}
				break;
			}
		    }
		    if(w==EB_BODY)
		    {
			clearmessagearea();
			//set_color(COLOR_MAGENTA,COLOR_BLACK,1);
			if (goodguyattack) g_attrset(CP_GREEN);
			else g_attrset(CP_RED);

			switch(randval(11))
			{
			    case 0:
				if(target->special[ESW_UPPERSPINE]&&breakdam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT) g_addstr("'s upper spine is shattered!", gamelog);
				    else g_addstr("'s upper spine is broken!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_UPPERSPINE]=0;
				    if(target->blood>20) target->blood=20;
				}
				break;
			    case 1:
				if(target->special[ESW_LOWERSPINE]&&breakdam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s lower spine is shattered!", gamelog);
				    else g_addstr("'s lower spine is broken!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_LOWERSPINE]=0;
				    if(target->blood>20) target->blood=20;
				}
				break;
			    case 2:
				if(target->special[ESW_RIGHTLUNG]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s right lung is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s right lung is torn!", gamelog);
				    else g_addstr("'s right lung is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_RIGHTLUNG]=0;
				    if(target->blood>20) target->blood=20;
				}
				break;
			    case 3:
				if(target->special[ESW_LEFTLUNG]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s left lung is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s left lung is torn!", gamelog);
				    else g_addstr("'s left lung is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_LEFTLUNG]=0;
				    if(target->blood>20) target->blood=20;
				}
				break;
			    case 4:
				if(target->special[ESW_HEART]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s heart is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s heart is torn!", gamelog);
				    else g_addstr("'s heart is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_HEART]=0;
				    if(target->blood>3) target->blood=3;
				}
				break;
			    case 5:
				if(target->special[ESW_LIVER]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s liver is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s liver is torn!", gamelog);
				    else g_addstr("'s liver is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_LIVER]=0;
				    if(target->blood>50) target->blood=50;
				}
				break;
			    case 6:
				if(target->special[ESW_STOMACH]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s stomach is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s stomach is torn!", gamelog);
				    else g_addstr("'s stomach is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_STOMACH]=0;
				    if(target->blood>50) target->blood=50;
				}
				break;
			    case 7:
				if(target->special[ESW_RIGHTKIDNEY]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s right kidney is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s right kidney is torn!", gamelog);
				    else g_addstr("'s right kidney is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_RIGHTKIDNEY]=0;
				    if(target->blood>50) target->blood=50;
				}
				break;
			    case 8:
				if(target->special[ESW_LEFTKIDNEY]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s left kidney is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s left kidney is torn!", gamelog);
				    else g_addstr("'s left kidney is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_LEFTKIDNEY]=0;
				    if(target->blood>50) target->blood=50;
				}
				break;
			    case 9:
				if(target->special[ESW_SPLEEN]&&pokedam)
				{
				    //move(16,1);
				    g_addstr(describe_entity_static(target), gamelog);
				    if(damtype & WOUND_SHOT)g_addstr("'s spleen is blasted!", gamelog);
				    else if(damtype & WOUND_TORN)g_addstr("'s spleen is torn!", gamelog);
				    else g_addstr("'s spleen is punctured!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_SPLEEN]=0;
				    if(target->blood>50) target->blood=50;
				}
				break;
			    case 10:
				if(target->special[ESW_RIBS]>0&&breakdam)
				{
				    int ribminus=randval(RIBNUM)+1;
				    if(ribminus>target->special[ESW_RIBS]) ribminus=target->special[ESW_RIBS];

				    //move(16,1);
				    if(ribminus>1)
				    {
					if(ribminus==target->special[ESW_RIBS])
					    g_addstr("All ", gamelog);
					g_addstr(ribminus, gamelog);
					g_addstr(" of ", gamelog);
					g_addstr(describe_entity_static(target), gamelog);
					g_addstr("'s ribs are ", gamelog);
				    }
				    else if(target->special[ESW_RIBS]>1)
				    {
					g_addstr("One of ", gamelog);
					g_addstr(describe_entity_static(target), gamelog);
					g_addstr("'s rib is ", gamelog);
				    }
				    else
				    {
					g_addstr(describe_entity_static(target), gamelog);
					g_addstr("'s last unbroken rib is ", gamelog);
				    }

				    if(damtype & WOUND_SHOT)g_addstr("shot apart!", gamelog);
				    else g_addstr("broken!", gamelog);
				    g_addstr("\n",NULL);

				    g_getkey();

				    target->special[ESW_RIBS]-=ribminus;
				}
				break;
			}
		    }

		    severloot(*target,groundloot);
		}

		//set_color(COLOR_WHITE,COLOR_BLACK,1);
	    }
	}
	else
	{
	    g_attrset(CP_YELLOW);
	    strcat(str," to no effect.");
	    //move(17,1);
	    g_addstr(str, gamelog);
	    g_addstr("\n",NULL);

	    printparty();
	    if(mode==GM_CHASECAR||
		    mode==GM_CHASEFOOT) printchaseencounter();
	    else printencounter();

	    g_getkey();
	}
    }
    else
    {
	set_color(COLOR_WHITE,COLOR_BLACK,1);

	if(melee && aroll<droll-10 && t.blood>70 && t->animalgloss==ANIMALGLOSS_NONE
		&& t->weapon && t->weapon.get_attack(false,true,true) != NULL)
	{

	    strcpy(str,describe_entity_static(t));
	    strcat(str," knocks the blow aside and counters!");
	    //move(17,1);
	    g_addstr(str, gamelog);
	    g_addstr("\n",NULL);

	    g_getkey();

	    goodguyattack = !goodguyattack;
	    char actual_dummy;
	    attack(t,a,0,actual_dummy,true);
	    goodguyattack = !goodguyattack;
	}//TODO if missed person, but vehicle is large, it might damage the car. 
	else
	{
	    if(sneak_attack)
	    {
		strcpy(str, describe_entity_static(t));
		switch(randval(4))
		{
		    case 0: strcat(str," notices at the last moment!"); break;
		    case 1: strcat(str," wasn't born yesterday!"); break;
		    case 2: strcat(str," spins and blocks the attack!"); break;
		    default: strcat(str," jumps back and cries out in alarm!"); break;
		}
		sitealarm=1;
	    }
	    else
	    {
		if (mode==GM_CHASECAR)
		{
		    strcpy(str, driver->name);
		    switch(droll)
		    {
			case 1: strcpy(str, describe_entity_static(a)); strcat(str," missed!"); break;
			case 2: strcpy(str, describe_entity_static(a)); strcat(str," just barely missed!"); break;
			case 3: strcat(str," can't seem to keep the vehicle in either the lane or the line of fire!"); break;
			case 4: strcat(str," swerves randomly!"); break;
			case 5: strcat(str," cuts off another driver and the shot is blocked!"); break;
			case 6: strcat(str," drops behind a hill in the road!"); break;
			case 7: strcat(str," changes lanes at the last second!"); break;
			case 8: strcat(str," accelerates suddenly and the shot goes short!"); break;
			case 9: strcat(str," fakes a left, and goes right instead!"); break;
			case 10: strcat(str," fakes a right, and goes left instead!"); break;
			case 11: strcat(str," fakes with the brakes while powering ahead!"); break;
			case 12: strcat(str," swerves to the other side of a truck!"); break;
			case 13: strcat(str," weaves through a row of taxis!"); break;
			case 14: strcat(str," dodges behind a hot dog cart!"); break;
			case 15: strcat(str," squeezes between some bridge supports for cover!"); break;
			case 16: strcat(str," squeals around a corner and behind a building!"); break;
			case 17: strcat(str," power slides through a narrow gap in the traffic!"); break;
			case 18: strcat(str," rolls the car onto two wheels to dodge the shot!"); break;
			default: strcpy(str, describe_entity_static(a)); strcat(str," misses completely!"); break; // You failed to hit someone who probably rolled a zero.  You should feel bad.
		    }
		}else{
		    strcpy(str, describe_entity_static(t));
		    switch(droll)
		    {
			case 1: strcpy(str, describe_entity_static(a)); strcat(str," missed!"); break;
			case 2: strcpy(str, describe_entity_static(a)); strcat(str," just barely missed!"); break;
			case 3: strcat(str," tumbles out of the way!"); break;
			case 4: strcat(str," jumps aside at the last moment!"); break;
			case 5: strcat(str," leaps for cover!"); break;
			case 6: strcat(str," ducks back behind cover!"); break;
			case 7: strcat(str," wisely stays behind cover!"); break;
			case 8: strcat(str," rolls away from the attack!"); break;
			case 9: strcat(str," nimbly dodges away from the line of fire!"); break;
			case 10: strcat(str," leaps over the attack!"); break;
			case 11: strcat(str," gracefully dives to avoid the attack!"); break;
			case 12: strcat(str," twists to avoid the attack!"); break;
			case 13: strcat(str," spins to the side!"); break;
			case 14: strcat(str," does the Matrix-dodge!"); break; // You saying I can dodge bullets?
			case 15: strcat(str," avoids the attack with no difficulty at all!"); break;
			case 16: strcat(str," flexes slightly to avoid being hit!"); break;
			case 17: strcat(str," confidently allows the attack to miss!"); break;
			case 18: strcat(str," seems to avoid the attack with only an angry glare!"); break; //When you're ready, you won't have to.
			default: strcpy(str, describe_entity_static(a)); strcat(str," misses completely!"); break; // You failed to hit someone who probably rolled a zero.  You should feel bad.
		    }
		}
	    }
	    //move(17,1);
	    g_addstr(str, gamelog);
	    g_addstr("\n",NULL);
	    printparty();
	    if(mode==GM_CHASECAR||
		    mode==GM_CHASEFOOT) printchaseencounter();
	    else printencounter();

	    g_getkey();
	}
    }

    for(;thrownweapons>0;thrownweapons--)
    {
	if(a.has_thrown_weapon)
	    a.ready_another_throwing_weapon();
	a.drop_weapon(NULL);
    }

    *actual=1;
    return;
}



/* modifies a combat roll based on the creature's critical injuries */
void healthmodroll(int *aroll,struct t_entity* a)
{
    if(a->special[ESW_RIGHTEYE]!=1) *aroll-=randval(2);
    if(a->special[ESW_LEFTEYE]!=1) *aroll-=randval(2);
    if(a->special[ESW_RIGHTEYE]!=1&&
	    a->special[ESW_LEFTEYE]!=1) *aroll-=randval(20);
    if(a->special[ESW_RIGHTLUNG]!=1) *aroll-=randval(8);
    if(a->special[ESW_LEFTLUNG]!=1) *aroll-=randval(8);
    if(a->special[ESW_HEART]!=1) *aroll-=randval(10);
    if(a->special[ESW_LIVER]!=1) *aroll-=randval(5);
    if(a->special[ESW_STOMACH]!=1) *aroll-=randval(5);
    if(a->special[ESW_RIGHTKIDNEY]!=1) *aroll-=randval(5);
    if(a->special[ESW_LEFTKIDNEY]!=1) *aroll-=randval(5);
    if(a->special[ESW_SPLEEN]!=1) *aroll-=randval(4);
    if(a->special[ESW_LOWERSPINE]!=1) *aroll-=randval(100);
    if(a->special[ESW_UPPERSPINE]!=1) *aroll-=randval(200);
    if(a->special[ESW_NECK]!=1) *aroll-=randval(300);
    if(a->special[ESW_RIBS]<RIBNUM) *aroll-=randval(5);
    if(a->special[ESW_RIBS]<RIBNUM/2) *aroll-=randval(5);
    if(a->special[ESW_RIBS]==0) *aroll-=randval(5);
}



/* adjusts attack damage based on armor, other factors */
void damagemod(struct t_entity &t,char &damtype,int &damamount,
	char hitlocation,char armorpenetration,int mod,int extraarmor)
{
    int armor=t.get_armor().get_armor(hitlocation);

    if(t->animalgloss==ANIMALGLOSS_TANK)
    {
	if(damtype!=WOUND_BURNED) armor=15;
	else armor=10;
    }


    //if(t.get_armor().get_quality()>1)
    armor-=t.get_armor().get_quality()-1;
    if(t.get_armor().is_damaged())
	armor-=1;

    if(armor<0) armor=0; // Possible from second-rate clothes
    armor += extraarmor; // Add vehicle armor 

    int mod2=armor+randval(armor+1)-armorpenetration;
    if(mod2>0) mod-=mod2*2;

    if(mod>10) mod=10; // Cap damage multiplier (every 5 points adds 1x damage)

    if(mod<=-20) damamount>>=8;  //Cars plus heavy armor can be really tough.
    else if(mod<=-14) damamount>>=7;
    else if(mod<=-8) damamount>>=6;
    else if(mod<=-6) damamount>>=5;
    else if(mod<=-4) damamount>>=4;
    else if(mod<=-3) damamount>>=3;
    else if(mod<=-2) damamount>>=2;
    else if(mod<=-1) damamount>>=1;
    else if(mod>=0) damamount=(int)((float)damamount * (1.0f + 0.2f*mod));

    // Firefighter's bunker gear reduces fire damage by 3/4
    if((damtype & WOUND_BURNED) && t.get_armor().has_fireprotection())
    {
	// Damaged gear isn't as effective as undamaged gear
	if(t.get_armor().is_damaged())
	    damamount>>=1; // Only half as much damage reduction
	else
	    damamount>>=2; // Full damage reduction
    }

    if(damamount<0)damamount=0;
}


void specialattack(struct t_entity &a, struct t_entity &t, char &actual)
{
    std::string s = "";
    static const char *judge_debate[]   =
    {
	"debates the death penalty with",
	"debates gay rights with",
	"debates free speech with",
	"debates the Second Amendment with"
    };

    static const char *conservative_ceo_debate[] =
    {
	"explains the derivatives market to",
	"justifies voodoo economics to",
	"extols the Reagan presidency to",
	"argues about tax cuts with",
	"explains Conservative philosophy to",
	"extends a dinner invitation to",
	"offers a VP position to",
	"shows a $1000 bill to",
	"debates fiscal policy with",
	"offers stock options to"
    };

    static const char *other_ceo_debate[] =
    {
	"debates fiscal policy with",
	"derides voodoo economics to",
	"dismisses the Reagan presidency to",
	"argues about tax cuts with",
	"explains Liberal philosophy to"
    };

    static const char *conservative_politician_debate[] =
    {
	"debates the death penalty with",
	"debates gay rights with",
	"debates free speech with",
	"debates the Second Amendment with",
	"justifies voodoo economics to",
	"extols the Reagan presidency to",
	"argues about tax cuts with",
	"explains Conservative philosophy to",
	"extends a dinner invitation to",
	"debates fiscal policy with",
	"chats warmly with",
	"smiles at"
    };

    static const char *other_politician_debate[] =
    {
	"debates the death penalty with",
	"debates gay rights with",
	"debates free speech with",
	"debates the Second Amendment with",
	"derides voodoo economics to",
	"dismisses the Reagan presidency to",
	"argues about tax cuts with",
	"explains Liberal philosophy to",
	"extends a dinner invitation to",
	"debates fiscal policy with",
	"chats warmly with",
	"smiles at"
    };

    static const char *media_debate[] =
    {
	"winks at",
	"smiles at",
	"smirks at",
	"chats warmly with",
	"yells slogans at"
    };

    static const char *military_debate[] =
    {
	"recites the Pledge of Allegiance to",
	"debates national security with",
	"debates terrorism with",
	"preaches about veterans to",
	"explains military spending to"
    };

    static const char *police_debate[] =
    {
	"reasons with ",
	"promises a fair trial to ",
	"offers a kind ear to ",
	"urges cooperation from ",
	"offers a hug to ",
	"suggests counseling to ",
	"gives a teddy bear to "
    };

    int resist=0;
    char str[200];

    clearmessagearea();
    set_color(COLOR_WHITE,COLOR_BLACK,1);

    strcpy(str,describe_entity_static(a));
    strcat(str," ");

    int attack=0;
    if(a.align!=1)
	attack=entity_attr_roll(a,EA_WIS)+t.get_attribute(EA_WIS,false);
    else if(a.align==1)
	attack=entity_attr_roll(a,EA_HRT)+t.get_attribute(EA_HRT,false);

    switch(a.type)
    {
	case CREATURE_JUDGE_CONSERVATIVE:
	case CREATURE_JUDGE_LIBERAL:
	    strcat(str,pickrandom(judge_debate));
	    strcat(str," ");
	    strcat(str,describe_entity_static(t));
	    strcat(str,"!");
	    if(t.align==1)
		resist=entity_skill_roll(t,ES_LAW)+
		    entity_attr_roll(t,EA_HRT);
	    else
		resist=entity_skill_roll(t,ES_LAW)+
		    entity_attr_roll(t,EA_WIS);
	    attack+=entity_skill_roll(a,ES_LAW);
	    break;
	case CREATURE_SCIENTIST_EMINENT:
	    switch(randval(3))
	    {
		case 0:strcat(str,"debates scientific ethics with");break;
		case 1:if(a.align==-1)strcat(str,"explains the benefits of research to");
			   else strcat(str,"explains ethical research to");break;
		case 2:strcat(str,"discusses the scientific method with");break;
	    }
	    strcat(str," ");
	    strcat(str,describe_entity_static(t));
	    strcat(str,"!");
	    if(t.align==1)
		resist=entity_skill_roll(t,ES_SCIENCE)+
		    entity_attr_roll(t,EA_HRT);
	    else
		resist=entity_skill_roll(t,ES_SCIENCE)+
		    entity_attr_roll(t,EA_WIS);
	    attack+=entity_skill_roll(a,ES_SCIENCE);
	    break;
	case CREATURE_POLITICIAN:
	    if(a.align==-1)
		strcat(str,pickrandom(conservative_politician_debate));
	    else
		strcat(str,pickrandom(other_politician_debate));
	    strcat(str," ");
	    strcat(str,describe_entity_static(t));
	    strcat(str,"!");
	    if(t.align==1)
		resist=entity_skill_roll(t,ES_LAW)+
		    entity_attr_roll(t,EA_HRT);
	    else
		resist=entity_skill_roll(t,ES_LAW)+
		    entity_attr_roll(t,EA_WIS);
	    attack+=entity_skill_roll(a,ES_LAW);
	    break;
	case CREATURE_CORPORATE_CEO:
	    if(a.align==-1)
		strcat(str,pickrandom(conservative_ceo_debate));
	    else
		strcat(str,pickrandom(other_ceo_debate));
	    strcat(str," ");
	    strcat(str,describe_entity_static(t));
	    strcat(str,"!");
	    if(t.align==1)
		resist=entity_skill_roll(t,ES_BUSINESS)+
		    entity_attr_roll(t,EA_HRT);
	    else
		resist=entity_skill_roll(t,ES_BUSINESS)+
		    entity_attr_roll(t,EA_WIS);
	    attack+=entity_skill_roll(a,ES_BUSINESS);
	    break;
	case CREATURE_RADIOPERSONALITY:
	case CREATURE_NEWSANCHOR:
	    strcat(str,pickrandom(media_debate));
	    strcat(str," ");
	    strcat(str,describe_entity_static(t));
	    strcat(str,"!");
	    if(t.align==1)
		resist=entity_attr_roll(t,EA_HRT);
	    else
		resist=entity_attr_roll(t,EA_WIS);
	    attack+=entity_attr_roll(a,EA_CHARISMA);
	    break;
	case CREATURE_MILITARYOFFICER:
	    strcat(str,pickrandom(military_debate));
	    strcat(str," ");
	    strcat(str,describe_entity_static(t));
	    strcat(str,"!");
	    if(t.align==1)
		resist=entity_attr_roll(t,EA_HRT);
	    else
		resist=entity_attr_roll(t,EA_WIS);
	    attack+=entity_attr_roll(a,EA_CHARISMA);
	    break;
	case CREATURE_COP:
	    if(a.enemy())
	    {
		strcat(str,pickrandom(police_debate));
		strcat(str,describe_entity_static(t));
		strcat(str,"!");

		resist=entity_attr_roll(t,EA_HRT);

		attack+=entity_skill_roll(a,ES_PERSUASION);
		break;
	    }
	    //No break. If the cop is a liberal it will do a musical attack instead.
	default:
	    if(a->weapon.has_musical_attack() || a.type==CREATURE_COP)
	    {
		switch(randval(5))
		{
		    case 0:strcat(str,"plays a song for");break;
		    case 1:strcat(str,"sings to");break;
		    case 2:if(a->weapon.has_musical_attack())
			   {
			       strcat(str,"strums the ");
			       strcat(str,get_weapon_name(a->weapon,));
			   }
			   else // let's use a small enough instrument for anyone to carry in their pocket
			       strcat(str,"blows a harmonica");
			   strcat(str," at");break;
		    case 3:if(a.align==1)strcat(str,"plays protest songs at");
			       else strcat(str,"plays country songs at");
			       break;
		    case 4:strcat(str,"rocks out at");break;
		}
		strcat(str," ");
		strcat(str,describe_entity_static(t));
		strcat(str,"!");

		attack=entity_skill_roll(a,ES_MUSIC);

		if(t.align==1)
		    resist=entity_attr_roll(t,EA_HRT);
		else resist=entity_attr_roll(t,EA_WIS);
		if(resist>0)
		    entity_train(a,ES_MUSIC,randval(resist)+1);
		else entity_train(a,ES_MUSIC,1);
	    }
	    break;
    }

    //move(16,1);
    g_addstr(str, gamelog);
    g_addstr("\n",NULL);

    if((t->animalgloss==ANIMALGLOSS_TANK||(t->animalgloss==ANIMALGLOSS_ANIMAL&&law[LAW_ANIMALRESEARCH]!=2))
	    ||(a.enemy() && t.flag & CREATUREFLAG_BRAINWASHED))
    {
	//move(17,1);
	g_addstr(s+describe_entity_static(t)+" is immune to the attack!", gamelog);
    }
    else if (a.align == t.align)
    {
	move (17,1);
	g_addstr(s+describe_entity_static(t)+" already agrees with "+describe_entity_static(a)+".");
    }
    else if(attack>resist)
    {
	t.stunned+=(attack-resist)/4;
	if(a.enemy())
	{
	    if(t->juice>100)
	    {
		//move(17,1);
		g_addstr(s+describe_entity_static(t)+" loses juice!", gamelog);
		addjuice(t,-50,100);
	    }
	    else if(randval(15)>t.get_attribute(EA_WIS,true) || t.get_attribute(EA_WIS,true) < t.get_attribute(EA_HRT,true))
	    {
		//move(17,1);
		g_addstr(s+describe_entity_static(t)+" is tainted with Wisdom!", gamelog);
		t.adjust_attribute(EA_WIS,+1);
	    }
	    else if(t.align==ALIGN_LIBERAL && t.flag & CREATUREFLAG_LOVESLAVE)
	    {
		//move(17,1);
		g_addstr(s+describe_entity_static(t)+" can't bear to leave!", gamelog);
	    }
	    else
	    {
		if(a.align==-1)
		{
		    //move(17,1);
		    g_addstr(s+describe_entity_static(t)+" is turned Conservative", gamelog);
		    t.stunned=0;
		    if(t->prisoner!=NULL)
			freehostage(t,0);
		    g_addstr("!", gamelog);
		}
		else
		{
		    //move(17,1);
		    g_addstr(s+describe_entity_static(t)+" doesn't want to fight anymore", gamelog);
		    t.stunned=0;
		    if(t->prisoner!=NULL)
			freehostage(t,0);
		    g_addstr("!", gamelog);
		}

		for(int e=0;e<ENCMAX;e++)
		{
		    if(encounter[e].exists==0)
		    {
			encounter[e]=t;
			encounter[e].exists=1;
			if(a.align==-1)conservatise(encounter[e]);
			encounter[e]->cantbluff=2;
			encounter[e].squadid=-1;
			break;
		    }
		}

		bool flipstart=0;
		for(int p=0;p<6;p++)
		{
		    if(activesquad->squad[p]==&t)
		    {
			activesquad->squad[p]->die();
			activesquad->squad[p]->location=-1;
			activesquad->squad[p]=NULL;
			flipstart=1;
		    }
		    if(flipstart&&p<5) activesquad->squad[p]=activesquad->squad[p+1];
		}
		if(flipstart) activesquad->squad[5]=NULL;
	    }
	}
	else
	{
	    if(t->juice>=100)
	    {
		//move(17,1);
		g_addstr(s+describe_entity_static(t)+" seems less badass!", gamelog);
		addjuice(t,-50,99);
	    }
	    else if(!t.attribute_check(EA_HRT,DIFFICULTY_AVERAGE) ||
		    t.get_attribute(EA_HRT,true) < t.get_attribute(EA_WIS,true))
	    {
		//move(17,1);
		g_addstr(s+describe_entity_static(t)+"'s Heart swells!", gamelog);
		t.adjust_attribute(EA_HRT,+1);
	    }
	    else
	    {
		//move(17,1);
		g_addstr(s+describe_entity_static(t)+" has turned Liberal!", gamelog);
		t.stunned=0;

		liberalize(t);
		t.infiltration/=2;
		t.flag|=CREATUREFLAG_CONVERTED;
		t->cantbluff=0;
	    }
	}
    }
    else
    {
	//move(17,1);
	g_addstr(s+describe_entity_static(t)+" remains strong.", gamelog);
    }

    g_addstr("\n",NULL);

    printparty();
    if(mode==GM_CHASECAR||
	    mode==GM_CHASEFOOT) printchaseencounter();
    else printencounter();

    g_getkey();

    return;
}


/* destroys armor, masks, drops weapons based on severe damage */
void severloot(struct t_entity &cr,vector<Item *> &loot)
{
    int armok=2;
    if((cr->wound[EB_ARM_RIGHT] & WOUND_NASTYOFF)||
	    (cr->wound[EB_ARM_RIGHT] & WOUND_CLEANOFF)) armok--;
    if((cr->wound[EB_ARM_LEFT] & WOUND_NASTYOFF)||
	    (cr->wound[EB_ARM_LEFT] & WOUND_CLEANOFF)) armok--;
    if(cr.special[ESW_NECK]!=1) armok=0;
    if(cr.special[ESW_UPPERSPINE]!=1) armok=0;

    if(cr->weapon && armok==0)
    {
	clearmessagearea();
	g_attrset(CP_YELLOW);
	//move(16,1);
	g_addstr("The ", gamelog);
	g_addstr(get_weapon_name(cr->weapon,1), gamelog);
	g_addstr(" slips from", gamelog);
	//move(17,1);
	g_addstr(cr.name, gamelog);
	g_addstr("'s grasp.", gamelog);
	g_addstr("\n",NULL);

	g_getkey();

	if(mode==GM_SITE) cr.drop_weapons_and_clips(&loot);
	else cr.drop_weapons_and_clips(NULL);
    }

    if((((cr->wound[EB_BODY] & WOUND_CLEANOFF)||
		    (cr->wound[EB_BODY] & WOUND_NASTYOFF))&&
		cr.get_armor().covers(EB_BODY))||
	    ((cr->wound[EB_HEAD] & WOUND_NASTYOFF)&&
	     cr.get_armor().is_mask()))
    {
	clearmessagearea();
	g_attrset(CP_YELLOW);
	//move(16,1);
	g_addstr(cr.name, gamelog);
	g_addstr("'s ", gamelog);
	g_addstr(cr.get_armor().get_name(), gamelog);
	g_addstr(" has been destroyed.", gamelog);
	g_addstr("\n",NULL);

	g_getkey();

	cr.strip(NULL);
    }
}



/* damages the selected armor if it covers the body part specified */
void armordamage(Armor &armor,int bp, int damamount)
{
    if(armor.covers(bp) && randval(armor.get_durability()) < damamount)
    {
	if (armor.is_damaged())
	{
	    armor.decrease_quality(randval(armor.get_durability()) < randval(damamount)/armor.get_quality() ? 1:0);
	}
	else
	{
	    armor.set_damaged(true);
	}
    }
}



/* blood explosions */
void bloodblast(Armor* armor)
{
    //GENERAL
    if(armor!=NULL)
	armor->set_bloody(true);

    if(mode!=GM_SITE)
	return;

    levelmap[locx][locy][locz].flag|=SITEBLOCK_BLOODY2;

    //HIT EVERYTHING
    for(int p=0;p<6;p++)
    {
	if(activesquad->squad[p]==NULL) continue;
	if(!randval(2))
	    activesquad->squad[p]->get_armor().set_bloody(true);
    }

    for(int e=0;e<ENCMAX;e++)
    {
	if(!encounter[e].exists) continue;
	if(!randval(2))
	    encounter[e].get_armor().set_bloody(true);
    }

    //REFRESH THE SCREEN
    printsitemap(locx,locy,locz);
    refresh();
}



/* kills the specified creature from the encounter, dropping loot */
void delenc(short e,char loot)
{
    //MAKE GROUND LOOT
    if(mode!=GM_SITE) loot=0;
    if(loot) makeloot(encounter[e],groundloot);

    //BURY IT
    for(int en=e;en<ENCMAX;en++)
    {
	if(!encounter[en].exists) break;
	if(en<ENCMAX-1) encounter[en]=encounter[en+1];
    }
    encounter[ENCMAX-1].exists=0;
}



/* generates the loot dropped by a creature when it dies */
void makeloot(struct t_entity &cr,vector<Item *> &loot)
{
    cr.drop_weapons_and_clips(&loot);
    cr.strip(&loot);

    if(cr.money>0 && mode == GM_SITE)
    {
	loot.push_back(new Money(cr.money));
	cr.money=0;
    }
}



/* abandoned liberal is captured by conservatives */
void capturecreature(struct t_entity &t)
{
    t.activity.type=ACTIVITY_NONE;
    t.drop_weapons_and_clips(NULL);
    //t.strip(NULL);
    Armor clothes=Armor(*armortype[getarmortype("ARMOR_CLOTHES")]);
    t.give_armor(clothes,NULL);

    freehostage(t,2); // situation 2 = no message; this may want to be changed to 0 or 1
    if(t->prisoner)
    {
	if(t->prisoner->squadid==-1)
	    delete t->prisoner;
	t->prisoner=NULL; // Stop hauling people
    }
    if(t.flag & CREATUREFLAG_JUSTESCAPED)
    {
	t.location=cursite;
	if(sitetype==SITE_GOVERNMENT_PRISON||
		sitetype==SITE_GOVERNMENT_COURTHOUSE)
	{
	    Armor prisoner=Armor(*armortype[getarmortype("ARMOR_PRISONER")]);
	    t.give_armor(prisoner,NULL);
	}
	if(sitetype==SITE_GOVERNMENT_PRISON)
	{
	    // Clear criminal record?
	    t.heat=0;
	    for(int i=0;i<LAWFLAGNUM;i++)
		t.crimes_suspected[i]=0;
	}
    }
    else
	t.location = find_police_station(cursite);

    t.squadid=-1;
}



/* checks if the creature can fight and prints flavor text if they can't */
char incapacitated(struct t_entity &a,char noncombat,char* printed)
{
    *printed=0;

    if(a->animalgloss==ANIMALGLOSS_TANK)
    {
	if(a.blood<=20||(a.blood<=50&&(randval(2)||a.forceinc)))
	{
	    a.forceinc=0;
	    if(noncombat)
	    {
		clearmessagearea();

		set_color(COLOR_WHITE,COLOR_BLACK,1);

		//move(16,1);
		g_addstr("The ", gamelog);
		g_addstr(describe_entity_static(a), gamelog);
		switch(randval(3))
		{
		    case 0: g_addstr(" smokes...", gamelog); break;
		    case 1: g_addstr(" smolders.", gamelog); break;
		    case 2: g_addstr(" burns...", gamelog); break;
		}

		g_addstr("\n",NULL);

		*printed=1;
	    }
	    return 1;
	}

	return 0;
    }

    if(a->animalgloss==ANIMALGLOSS_ANIMAL)
    {
	if(a.blood<=20||(a.blood<=50&&(randval(2)||a.forceinc)))
	{
	    a.forceinc=0;
	    if(noncombat)
	    {
		clearmessagearea();
		set_color(COLOR_WHITE,COLOR_BLACK,1);

		//move(16,1);
		g_addstr("The ", gamelog);
		g_addstr(describe_entity_static(a));
		switch(randval(3))
		{
		    case 0: g_addstr(" yelps in pain...", gamelog); break;
		    case 1: if(law[LAW_FREESPEECH]==-2) g_addstr(" [makes a stinky].", gamelog);
				else g_addstr(" soils the floor.", gamelog); break;
		    case 2: g_addstr(" yowls pitifully...", gamelog); break;
		}

		g_addstr("\n",NULL);

		printed=1;
	    }
	    return 1;
	}

	return 0;
    }

    if(a.blood<=20||(a.blood<=50&&(randval(2)||a.forceinc)))
    {
	a.forceinc=0;
	if(noncombat)
	{
	    clearmessagearea();
	    set_color(COLOR_WHITE,COLOR_BLACK,1);

	    //move(16,1);
	    g_addstr(describe_entity_static(a));
	    switch(randval(54))
	    {
		case 0: g_addstr(" desperately cries out to Jesus."); break;
		case 1: if(law[LAW_FREESPEECH]==-2) g_addstr(" [makes a stinky].");
			    else g_addstr(" soils the floor."); break;
		case 2: g_addstr(" whimpers in a corner."); break;
		case 3: g_addstr(" begins to weep."); break;
		case 4: g_addstr(" vomits."); break;
		case 5: g_addstr(" chortles..."); break;
		case 6: g_addstr(" screams in pain."); break;
		case 7: g_addstr(" asks for mother."); break;
		case 8: g_addstr(" prays softly..."); break;
		case 9: g_addstr(" clutches at the wounds."); break;
		case 10: g_addstr(" reaches out and moans."); break;
		case 11: g_addstr(" hollers in pain."); break;
		case 12: g_addstr(" groans in agony."); break;
		case 13: g_addstr(" begins hyperventilating."); break;
		case 14: g_addstr(" shouts a prayer."); break;
		case 15: g_addstr(" coughs up blood."); break;
		case 16: if(mode!=GM_CHASECAR) g_addstr(" stumbles against a wall.");
			     else g_addstr(" leans against the door."); break;
		case 17: g_addstr(" begs for forgiveness."); break;
		case 18: g_addstr(" shouts \"Why have you forsaken me?\""); break;
		case 19: g_addstr(" murmurs \"Why Lord?   Why?\""); break;
		case 20: g_addstr(" whispers \"Am I dead?\""); break;
		case 21: if(law[LAW_FREESPEECH]==-2) g_addstr(" [makes a mess], moaning.");
			     else g_addstr(" pisses on the floor, moaning."); break;
		case 22: g_addstr(" whispers incoherently."); break;
		case 23: if(a.special[ESW_RIGHTEYE]&&a.special[ESW_LEFTEYE])
			     g_addstr(" stares off into space.");
			 else if(a.special[ESW_RIGHTEYE]||a.special[ESW_LEFTEYE])
			     g_addstr(" stares into space with one empty eye.");
			 else g_addstr(" stares out with hollow sockets."); break;
		case 24: g_addstr(" cries softly."); break;
		case 25: g_addstr(" yells until the scream cracks dry."); break;
		case 26: if(a.special[ESW_TEETH]>1) g_addstr("'s teeth start chattering.");
			     else if(a.special[ESW_TEETH]==1) g_addstr("'s tooth starts chattering.");
			     else g_addstr("'s gums start chattering."); break;
		case 27: g_addstr(" starts shaking uncontrollably."); break;
		case 28: g_addstr(" looks strangely calm."); break;
		case 29: g_addstr(" nods off for a moment."); break;
		case 30: g_addstr(" starts drooling."); break;
		case 31: g_addstr(" seems lost in memories."); break;
		case 32: g_addstr(" shakes with fear."); break;
		case 33: g_addstr(" murmurs \"I'm so afraid...\""); break;
		case 34: g_addstr(" cries \"It can't be like this...\""); break;
		case 35: if(a.age<20 && !a->animalgloss) g_addstr(" cries \"Mommy!\"");
			     else switch(a.type) {
				 case CREATURE_GENETIC:
				     g_addstr(" murmurs \"What about my offspring?\""); break;
				 case CREATURE_GUARDDOG:
				     g_addstr(" murmurs \"What about my puppies?\""); break;
				 default:
				     g_addstr(" murmurs \"What about my children?\""); break;
			     } break;
		case 36: g_addstr(" shudders quietly."); break;
		case 37: g_addstr(" yowls pitifully."); break;
		case 38: g_addstr(" begins losing faith in God."); break;
		case 39: g_addstr(" muses quietly about death."); break;
		case 40: g_addstr(" asks for a blanket."); break;
		case 41: g_addstr(" shivers softly."); break;
		case 42: if(law[LAW_FREESPEECH]==-2)g_addstr(" [makes a mess].");
			     else g_addstr(" vomits up a clot of blood."); break;
		case 43: if(law[LAW_FREESPEECH]==-2)g_addstr(" [makes a mess].");
			     else g_addstr(" spits up a cluster of bloody bubbles."); break;
		case 44: g_addstr(" pleads for mercy."); break;
		case 45: g_addstr(" quietly asks for coffee."); break;
		case 46: g_addstr(" looks resigned."); break;
		case 47: g_addstr(" scratches at the air."); break;
		case 48: g_addstr(" starts to giggle uncontrollably."); break;
		case 49: g_addstr(" wears a look of pain."); break;
		case 50: g_addstr(" questions God."); break;
		case 51: g_addstr(" whispers \"Mama baby.  Baby loves mama.\""); break;
		case 52: g_addstr(" asks for childhood toys frantically."); break;
		case 53: g_addstr(" murmurs \"But I go to church...\""); break;
	    }

	    printed=1;
	}

	return 1;
    }
    else if(a.stunned)
    {
	if(noncombat)
	{
	    a.stunned--;
	    clearmessagearea();
	    set_color(COLOR_WHITE,COLOR_BLACK,1);

	    //move(16,1);
	    g_addstr(describe_entity_static(a), gamelog);
	    switch(randval(11))
	    {
		case 0: g_addstr(" seems hesitant.", gamelog); break;
		case 1: g_addstr(" is caught in self-doubt.", gamelog); break;
		case 2: g_addstr(" looks around uneasily.", gamelog); break;
		case 3: g_addstr(" begins to weep.", gamelog); break;
		case 4: g_addstr(" asks \"Is this right?\"", gamelog); break;
		case 5: g_addstr(" asks for guidance.", gamelog); break;
		case 6: g_addstr(" is caught in indecision.", gamelog); break;
		case 7: g_addstr(" feels numb.", gamelog); break;
		case 8: g_addstr(" prays softly.", gamelog); break;
		case 9: g_addstr(" searches for the truth.", gamelog); break;
		case 10: g_addstr(" tears up.", gamelog); break;
	    }

	    g_addstr("\n",NULL);

	    printed=1;
	}
	return 1;
    }

    if(a.special[ESW_NECK]==2||
	    a.special[ESW_UPPERSPINE]==2)
    {
	if(!noncombat)
	{
	    clearmessagearea();
	    set_color(COLOR_WHITE,COLOR_BLACK,1);

	    //move(16,1);
	    g_addstr(describe_entity_static(a), gamelog);
	    switch(randval(5))
	    {
		case 0: g_addstr(" looks on with authority.", gamelog); break;
		case 1: g_addstr(" waits patiently.", gamelog); break;
		case 2: g_addstr(" sits in thought.", gamelog); break;
		case 3: g_addstr(" breathes slowly.", gamelog); break;
		case 4: g_addstr(" considers the situation.", gamelog); break;
	    }

	    g_addstr("\n",NULL);

	    printed=1;
	}

	return 1;
    }

    return 0;
}



/* describes a character's death */
void adddeathmessage(struct t_entity &cr)
{
    g_attrset(CP_YELLOW);

    //move(16,1);
    char str[200];

    if((cr->wound[EB_HEAD] & WOUND_CLEANOFF)||
	    (cr->wound[EB_HEAD] & WOUND_NASTYOFF))
    {
	strcpy(str,cr.name);
	switch(randval(4))
	{
	    case 0:
		strcat(str," reaches once where there ");
		g_addstr(str, gamelog);
		//move(17,1);
		if(mode!=GM_CHASECAR)
		    g_addstr("is no head, and falls.", gamelog);
		else g_addstr("is no head, and slumps over.", gamelog);
		break;
	    case 1:
		if(mode!=GM_CHASECAR)
		    strcat(str," stands headless for a ");
		else strcat(str," sits headless for a ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("moment then crumples over.", gamelog);
		break;
	    case 2:
		strcat(str," squirts ");
		if(law[LAW_FREESPEECH]==-2)strcat(str,"[red water]");
		else strcat(str,"blood");
		strcat(str," out of the ");
		g_addstr(str, gamelog);
		//move(17,1);
		if(mode!=GM_CHASECAR)
		    g_addstr("neck and runs down the hall.", gamelog);
		else g_addstr("neck and falls to the side.", gamelog);
		break;
	    case 3:
		strcat(str," sucks a last breath through ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("the neck hole, then is quiet.", gamelog);
		break;
	}
    }
    else if((cr->wound[EB_BODY] & WOUND_CLEANOFF)||
	    (cr->wound[EB_BODY] & WOUND_NASTYOFF))
    {
	strcpy(str,cr.name);
	switch(randval(2))
	{
	    case 0:strcat(str," breaks into pieces."); break;
	    case 1:strcat(str," falls apart and is dead."); break;
	}
	g_addstr(str, gamelog);
    }
    else
    {
	strcpy(str,cr.name);
	switch(randval(11))
	{
	    case 0:
		strcat(str," cries out one last time ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("then is quiet.", gamelog);
		break;
	    case 1:
		strcat(str," gasps a last breath and ");
		g_addstr(str, gamelog);
		//move(17,1);
		if(law[LAW_FREESPEECH]==-2)g_addstr("[makes a mess].", gamelog);
		else g_addstr("soils the floor.", gamelog);
		break;
	    case 2:
		strcat(str," murmurs quietly, breathing softly. ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("Then all is silent.", gamelog);
		break;
	    case 3:
		strcat(str," shouts \"FATHER!  Why have you ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("forsaken me?\" and dies in a heap.", gamelog);
		break;
	    case 4:
		strcat(str," cries silently for mother, ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("breathing slowly, then not at all.", gamelog);
		break;
	    case 5:
		strcat(str," breathes heavily, coughing up ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("blood...  then is quiet.", gamelog);
		break;
	    case 6:
		strcat(str," silently drifts away, and ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("is gone.", gamelog);
		break;
	    case 7:
		strcat(str," sweats profusely, murmurs ");
		g_addstr(str, gamelog);
		//move(17,1);
		if(law[LAW_FREESPEECH]==-2)g_addstr("something [good] about Jesus, and dies.", gamelog);
		else g_addstr("something about Jesus, and dies.", gamelog);
		break;
	    case 8:
		strcat(str," whines loudly, voice crackling, ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("then curls into a ball, unmoving.", gamelog);
		break;
	    case 9:
		strcat(str," shivers silently, whispering ");
		g_addstr(str, gamelog);
		//move(17,1);
		g_addstr("a prayer, then all is still.", gamelog);
		break;
	    case 10:
		strcat(str," speaks these final words: ");
		g_addstr(str, gamelog);
		//move(17,1);
		switch(cr.align)
		{
		    case ALIGN_LIBERAL:
		    case ALIGN_ELITELIBERAL:
			g_addstr(slogan, gamelog); break;
		    case ALIGN_MODERATE:
			g_addstr("\"A plague on both your houses...\"", gamelog); break;
		    default:
			g_addstr("\"Better dead than liberal...\"", gamelog); break;
		}
		break;
	}
    }
    g_addstr("\n",NULL);
}



/* pushes people into the current squad (used in a siege) */
void autopromote(int loc)
{
    if(!activesquad) return;

    int partysize=squadsize(activesquad),partyalive=squadalive(activesquad),libnum=0;

    if(partyalive==6) return;

    for(int pl=0;pl<len(pool);pl++)
    {
	if(pool[pl]->location!=loc) continue;
	if(pool[pl]->alive&&pool[pl]->align==1) libnum++;
    }

    if(partysize==libnum) return;

    char conf;
    for(int p=0;p<6;p++)
    {
	conf=0;
	if(activesquad->squad[p]==NULL) conf=1;
	else if(!activesquad->squad[p]->alive) conf=1;

	if(conf)
	{
	    for(int pl=0;pl<len(pool);pl++)
	    {
		if(pool[pl]->location!=loc) continue;
		if(pool[pl]->alive&&pool[pl]->squadid==-1&&
			pool[pl]->align==1)
		{
		    if(activesquad->squad[p]!=NULL) activesquad->squad[p]->squadid=-1;
		    activesquad->squad[p]=pool[pl];
		    activesquad->squad[p]->squadid=activesquad->id;
		    break;
		}
	    }
	}
    }
}
