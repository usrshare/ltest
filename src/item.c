#include "item.h"
#include <stdlib.h>

struct t_item* inv_find_empty(struct t_item* inv) {

	for (int i=0; i < INVENTORY_SIZE; i++)
		if ((inv[i].type == IT_NONE) || (inv[i].itemcount == 0)) return &inv[i];
	return NULL;
}

struct t_item* inv_find_stack(struct t_item* inv, struct t_item* toadd) {
	
	for (int i=0; i < INVENTORY_SIZE; i++)
		if ((inv[i].type == toadd->type) || (inv[i].itemtypeid == toadd->itemtypeid)) return &inv[i];
	return NULL;
}

int inv_size(struct t_item* inv) {

	int s=0;
	for (int i=0; i < MAX_ITEMS_PER_LOOT; i++)
		if ((inv[i].type != IT_NONE) && (inv[i].itemcount != 0)) s++;
	return s;
}
int inv_add(struct t_item* inv, struct t_item* toadd) {

	if (it_stackable[toadd -> type]) {
		struct t_item* stack = inv_find_stack(inv, toadd);
		if (stack) { stack->item_count += toadd->item_count; return 0; }
	}

	struct t_item* newitem = find_empty_item(inv);
	if (newitem) { *newitem = *toadd; return 0; } else return 1;
}

int inv_del(struct t_item* inv, struct t_item* todel, int count) {

	if (it_stackable[todel->type]) {

		if ((todel->item_count) < count) return 1;

		todel->item_count -= count; return 0;
	}

	todel->type = IT_NONE; todel->item_count = 0; return 0;
}

struct t_item* inv_find_used(struct t_item* inv) {
	for (int i=0; i < INVENTORY_SIZE; i++)
		if ((inv[i].type != IT_NONE) && (inv[i].itemcount != 0)) return &inv[i];
	return NULL;
}
