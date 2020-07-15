#include "pch.h"
#include "Inventory.h"

#ifdef CLIENT
#include "Packet.h"
#include "ConnectionManager.h"
#include "Controller.h"
#include "GameState.h"
#endif

Inventory::Inventory() {
	for (int i = 0; i < INV_SIZE; i++) items[i] = I_NONE;
	for (int i = 0; i < NUM_EQUIP_LOCS; i++) equips[i] = I_NONE;

	updateEquipmentBonus();
}

void Inventory::move(item_num_t a, item_num_t b) {
	if (a < 0 || b < 0) return;
	if (a >= INV_SIZE || b >= INV_SIZE) return;

	ITEM temp = items[b];
	items[b] = items[a];
	items[a] = temp;

#ifdef CLIENT
	sf::Packet p;
	p.append(&P_INV_MOVE, sizeof(Packet));
	p.append(&a, sizeof(item_num_t));
	p.append(&b, sizeof(item_num_t));

	((GameState*)g_state)->pInfo.inv.sends++;
	connection->send(p);
#endif
}

void Inventory::equip(item_num_t a, bool dungeonRules) {
	if (a < 0 || a >= INV_SIZE) return;
	if (items[a] == I_NONE) return;
	int equipSlot = (int)itemTypes[items[a]]->location;

	if (dungeonRules && equipSlot == (int)Equipment::SPECIAL) return;

	bool did = false;

	if (equipSlot >= 0 && equipSlot < NUM_EQUIP_LOCS) {
		// Move into empty slot
		if (equips[equipSlot] == I_NONE) {
			equips[equipSlot] = items[a];
			items[a] = I_NONE;
		}
		// Swap for current item
		else {
			ITEM temp = equips[equipSlot];
			equips[equipSlot] = items[a];
			items[a] = temp;
		}

		did = true;
		updateEquipmentBonus();
	}

#ifdef CLIENT
	if (did) {
		sf::Packet p;
		p.append(&P_INV_EQUIP, sizeof(Packet));
		p.append(&a, sizeof(item_num_t));

		((GameState*)g_state)->pInfo.inv.sends++;
		connection->send(p);
	}
#endif
}

int Inventory::getFirstFree() const {
	for (int i = 0; i < INV_SIZE; i++) {
		if (items[i] == I_NONE) return i;
	}
	return -1;
}

// 1) Move item in equipment -> empty inv slot
// 2) Swap item in equipment with full inv slot if it has the right type
void Inventory::unequip(item_num_t a, item_num_t b, bool dungeonRules) {
	if (a < 0 || a >= NUM_EQUIP_LOCS) return;
	if (b < 0 || b >= INV_SIZE) return;
	if (equips[a] == I_NONE) return;

	if (dungeonRules && itemTypes[equips[a]]->location == Equipment::SPECIAL) return;

	bool did = false;

	if (items[b] == I_NONE) {
		items[b] = equips[a];
		equips[a] = I_NONE;

		updateEquipmentBonus();
		did = true;
	}
	else {
		if ((int)itemTypes[items[b]]->location == a) {
			ITEM temp = items[b];
			items[b] = equips[a];
			equips[a] = temp;

			updateEquipmentBonus();
			did = true;
		}
	}

#ifdef CLIENT
	if (did) {
		sf::Packet p;
		p.append(&P_INV_UNEQUIP, sizeof(Packet));

		p.append(&a, sizeof(item_num_t));
		p.append(&b, sizeof(item_num_t));

		((GameState*)g_state)->pInfo.inv.sends++;
		connection->send(p);
	}
#endif
}

void Inventory::updateEquipmentBonus() {
	equipmentBonus = Stats(0,0,0,0,0,0,0,0);
	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		if (equips[i] != I_NONE)
			equipmentBonus += itemTypes[equips[i]]->bonus;
	}	
}

#include <iostream>
using std::cout; using std::endl;

bool Inventory::loadFromData(const void *data) {
	received++;
	//cout << "Inventory updates: Sent " << sends << ", received " << received << endl;

	if (sends == received) {
		ITEM *P = (ITEM*)data;
		for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
			equips[i] = *P;
			P++;
		}
		for (int i = 0; i < INV_SIZE; i++) {
			items[i] = *P;
			P++;
		}

		updateEquipmentBonus();
		return true;
	}
	return false;
}

void Inventory::appendToPacket(sf::Packet &packet) {
	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		packet.append(&equips[i], sizeof(ITEM));
	}
	for (int i = 0; i < INV_SIZE; i++) {
		packet.append(&items[i], sizeof(ITEM));
	}
}

int Inventory::getNumTargets() const {
	ITEM i = equips[(int)Equipment::WEAPON];
	if (i == I_NONE) return 0;
	return ((Weapon*)itemTypes[i])->num_targets;
}

void Inventory::setEquips(const ITEM items_[NUM_EQUIP_LOCS]) {
	for (auto i = 0; i < NUM_EQUIP_LOCS; i++) equips[i] = items_[i];
	updateEquipmentBonus();
}

void Inventory::setItems(const ITEM items_[INV_SIZE]) {
	for (auto i = 0; i < INV_SIZE; i++) items[i] = items_[i];
	updateEquipmentBonus();
}
