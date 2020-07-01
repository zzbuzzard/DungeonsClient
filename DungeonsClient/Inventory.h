#pragma once
#include "Item.h"
#include "CSMode.h"
#include "SFML/Network.hpp"

#define INV_SIZE 12

// Serialization : Equipment, then items

class Inventory
{
public:
	Inventory();

	void move(item_num_t a, item_num_t b);
	void equip(item_num_t a);
	void unequip(item_num_t a, item_num_t b);

	int getFirstFree() const; // or -1 if full. does not check equipment slots.

	bool loadFromData(const void *data); // returns true if the update was actually used [if received==sent]
	void appendToPacket(sf::Packet &packet);

	int getNumTargets();

	ITEM equips[NUM_EQUIP_LOCS];
	ITEM items[INV_SIZE];

	Stats equipmentBonus;

	// starts at 1 : the CLIENT_HELLO is replied to by SERVER_HELLO which has the first inventory update
	// (this system is in place to stop us changing 2 things then witnessing it undo one, then redoing it as server updates roll in)
	int sends = 1;

private:
	int received = 0;
	void updateEquipmentBonus();
};

