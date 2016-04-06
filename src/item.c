// vim: cin:sts=4:sw=4 
#include "item.h"
#include <stdlib.h>

bool it_stackable[IT_COUNT] = {
	0, //IT_NONE
	0, //IT_WEAPON
	0, //IT_ARMOR
	1, //IT_CLIP
	1, //IT_LOOT
	1, //IT_MONEY
	0, //IT_OTHER
};

struct t_item* inv_find_empty(struct t_item* inv) {

	for (int i=0; i < INVENTORY_SIZE; i++)
		if ((inv[i].type == IT_NONE) || (inv[i].itemcount == 0)) return &inv[i];
	return NULL;
}

struct t_item* inv_find_stack(struct t_item* inv, struct t_item* toadd) {
	
	for (int i=0; i < INVENTORY_SIZE; i++)
		if ((inv[i].type == toadd->type) && (inv[i].itemtypeid == toadd->itemtypeid)) return &inv[i];
	return NULL;
}

int inv_size(struct t_item* inv) {

	int s=0;
	for (int i=0; i < INVENTORY_SIZE; i++)
		if ((inv[i].type != IT_NONE) && (inv[i].itemcount != 0)) s++;
	return s;
}
struct t_item* inv_add(struct t_item* inv, struct t_item* toadd) {

	if (it_stackable[toadd -> type]) {
		struct t_item* stack = inv_find_stack(inv, toadd);
		if (stack) { stack->itemcount += toadd->itemcount; return stack; }
	}

	struct t_item* newitem = inv_find_empty(inv);
	if (newitem) { *newitem = *toadd; return newitem; } else return NULL;
}

int inv_del(struct t_item* inv, struct t_item* todel, int count) {

	if (it_stackable[todel->type]) {

		if ((todel->itemcount) < count) return 1;

		todel->itemcount -= count; return 0;
	}

	todel->type = IT_NONE; todel->itemcount = 0; return 0;
}

struct t_item* inv_find_used(struct t_item* inv) {
	for (int i=0; i < INVENTORY_SIZE; i++)
		if ((inv[i].type != IT_NONE) && (inv[i].itemcount != 0)) return &inv[i];
	return NULL;
}

int new_money(struct t_item* inv, int value) {

	struct t_item newmoney = {.type = IT_MONEY, .itemtypeid = 0, .value = value};
	inv_add(inv,&newmoney);
	return 0;
}

int inv_join(struct t_item* inv_to, struct t_item* inv_from) {

	while ((inv_size(inv_to) < INVENTORY_SIZE) && (inv_size(inv_from) != 0)) {

		struct t_item* i = inv_find_used(inv_from);

		inv_add(inv_to,i);
		inv_del(inv_from,i,1);
	}
	if (inv_size(inv_from) != 0) return 1; else return 0;
}


int inv_clear(struct t_item* inv) {
	while (inv_size(inv) > 0) {
		struct t_item* i = inv_find_used(inv);
		inv_del(inv,i,i->itemcount);
	}
	return 0;
}
