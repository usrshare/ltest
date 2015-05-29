#ifndef ITEM_H
#define ITEM_H

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

bool it_stackable[IT_COUNT] = {
	0, //IT_NONE
	0, //IT_WEAPON
	0, //IT_ARMOR
	1, //IT_CLIP
	1, //IT_LOOT
	1, //IT_MONEY
	0, //IT_OTHER
};


extern const char* itemtypenames[];

struct t_item {

	enum t_itemtypes type;

	long number_;
	long itemtypeid_;
	int itemcount;

	void* itemdetails;
	// points to appropriate struct for one of the other itemtypes.
};

#define INVENTORY_SIZE 40

//inventories are defined as arrays of items. obviously.

struct t_item* inv_find_empty(struct t_item* inv);
int inv_size(struct t_item* inv);
int inv_add(struct t_item* inv, struct t_item* item);
int inv_pop(struct t_item* inv, struct t_item* o_item);
int inv_del(struct t_item* inv_item);

#endif
