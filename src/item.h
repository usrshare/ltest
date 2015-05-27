#ifndef ITEM_H
#define ITEM_H

enum t_itemtypes {
	IT_OTHER = 0,
	IT_WEAPON,
	IT_ARMOR,
	IT_CLIP,
	IT_LOOT,
	IT_MONEY,
}

struct t_item {

	enum t_itemtype type;

	long number_;
	char* itemtypename;
	long itemtypeid_;

	void* itemdetails;
	// points to appropriate struct for one of the other itemtypes.
};
