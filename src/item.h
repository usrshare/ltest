#ifndef ITEM_H
#define ITEM_H

#include <stdbool.h>

enum t_itemtypes {
	IT_NONE = 0,
	IT_WEAPON,
	IT_ARMOR,
	IT_CLIP,
	IT_LOOT,
	IT_MONEY,
	IT_OTHER,
	IT_COUNT
};

extern bool it_stackable[IT_COUNT];

extern const char* itemtypenames[];

struct t_item {

	enum t_itemtypes type;

	long number;
	long itemtypeid;

	union {
		int itemcount;
		int value; //for cash
	};
	
	union {
		int item_hp; //damage status for armor, ammo for weapons/clips?
		int ammo; //alternative name.
		int a_flags; //for armors.
	};

	void* itemdetails;
	// points to appropriate struct for one of the other itemtypes.
};

#define INVENTORY_SIZE 40

//inventories are defined as arrays of items. obviously.

struct t_item* inv_find_empty(struct t_item* inv);
int inv_size(struct t_item* inv);
int inv_add(struct t_item* inv, struct t_item* item);
int inv_pop(struct t_item* inv, struct t_item* o_item);
int inv_del(struct t_item* inv, struct t_item* todel, int count);

int inv_join(struct t_item* inv_to, struct t_item* inv_from);
int inv_clear(struct t_item* inv);


int new_money(struct t_item* inv, int value);
#endif
