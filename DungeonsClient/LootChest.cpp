#include "pch.h"
#include "LootChest.h"
#include "GameState.h"
#include "Inventory.h"
#include "Media.h"

#include "Controller.h"
#include "ConnectionManager.h"
#include "Packet.h"

TEXTURE lootChestRarities[NUM_ITEM_RARITIES] = { T_CHEST_1, T_CHEST_2, T_CHEST_3, T_CHEST_4 };

LootChest::LootChest(pi pos_) 
	: Entity(E_NONE, ET_TOP_LAYER_TILE, pos_)
{
	// TODO : Change sprite based on rarity
	setBoxSize();
	setBoxTexture(T_CHEST_1);
	canWalk = true;
}

//void LootChest::pickUp(GameState *state, int i) {
//	if (i < 0 || i >= items.size()) return;
//	Inventory &inv = state->pInfo.inv;
//	
//	int ind = inv.getFirstFree();
//	if (ind == -1) return;
//
//	ITEM grab = items[i];
//	items.erase(items.begin() + i);
//
//	inv.items[ind] = grab;
//}

void LootChest::setRarity(ItemRarity maxRarity) {
	if ((int)maxRarity < 0 || (int)maxRarity >= NUM_ITEM_RARITIES) return;
	setBoxTexture(lootChestRarities[(int)maxRarity]);
}

void LootChest::update(GameState *state) {
	if (state->lootChanged)
		alive = state->pInfo.lootExists(pos);

	// TODO: Delete
	//if (state->getLocalPlayerPos() == pos &&
	//	items.size() > 0) {
	//	pickUp(state, 0);
	//}
}

bool LootChest::isAlive() {
	return alive;
}
