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

#define MAX_ITEMS_PER_LOOT 16

struct t_loot {
	struct t_item i[MAX_ITEMS_PER_LOOT];
	
};

struct t_item* find_empty_item(struct t_loot* loot);

#endif
