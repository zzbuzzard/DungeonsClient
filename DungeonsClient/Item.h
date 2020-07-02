#pragma once
#include "Media.h"
#include "Stats.h"

// Add new item : Add to ITEM enum, increment NUM_ITEMS, add data to Item.cpp

#define NUM_EQUIP_LOCS 5
enum class Equipment {
	HEAD,
	TORSO,
	WEAPON,
	ACCESSORY,
	SPECIAL,
};

#define NUM_ITEM_RARITIES 4
enum class ItemRarity {
	COMMON,
	UNCOMMON,
	RARE,
	LEGENDARY
};

// Uses int16_t as int8_t would limit number of items to 127. Wouldn't be unreasonable to exceed this.
// Order of these MUST NOT BE CHANGED EVER
// (once we start storing items on the server)
#define NUM_ITEMS 38
enum ITEM : int16_t {
	// Helmets
	I_HELMET,
	I_CURSED_RAGS,
	I_SLIME_FACEMASK,
	I_RUSTY_HELMET,
	I_CURSED_ELVEN_HELMET,
	I_VINE_HELMET,
	I_SAND_HELMET,

	// Torso stuff
	I_CHESTPLATE,
	I_PLANT_ARMOUR,
	I_SAND_CAPE,
	I_SCORPION_ARMOUR,
	I_RUSTY_ARMOUR,
	I_MAGE_ROBE,
	I_COBWEB_ARMOUR,
	I_SILK_CAPE,
	I_WOODEN_ARMOUR,
	I_BEAST_HIDE_ARMOUR,

	// Accessories
	I_SPEED_CHARM,
	I_SANDSTORM_AMULET,
	I_RECOVERY_CHARM,
	I_DESERT_RING,
	I_SLIME_ESSENCE,
	I_CURSED_CACTUS_EYE,
	I_CORRUPT_AMULET,

	// Weapons
	I_WOODEN_BOW,
	I_BONE_BOW,
	I_GOLDEN_BOW,
	I_EARTH_STAFF,
	I_FIRE_STAFF,
	I_SANDSTONE_SWORD,
	I_CACTUS_SWORD,
	I_SCORPION_DAGGER,
	I_ELVEN_BLADE,
	I_ELVEN_BOW,
	I_SPIDER_FANG_DAGGER,

	// Specials
	I_HOLY_ORB,
	I_DAMAGER,
	I_BLESSED_SAPLING,

	// New stuff

	I_NONE = -1,
};

class Item;

extern Item *itemTypes[NUM_ITEMS];

class Item
{
public:
	Item();
	Item(std::string name_, ItemRarity rarity_, TEXTURE texture_, Equipment location_, Stats bonus_);

	std::string name;
	//std::string desc; TODO
	TEXTURE texture;
	Equipment location;
	Stats bonus;
	ItemRarity rarity;
};

// A weapon is an item +
//  Damage
//  Range
//  Reload rate
//  Shot texture
//  Num targets
class Weapon : public Item {
public:
	Weapon();
	Weapon(std::string name_, ItemRarity rarity_, TEXTURE texture_, Stats bonus_, damage_t damage_,
		range_t range_, float reload_, TEXTURE shot_, target_count_t num_targets_);

	damage_t damage;
	range_t range;
	float reload;
	target_count_t num_targets;
	TEXTURE shot;
};

// Special ideas:
//	 Heals:
//      Big heal to 1 target
//      Medium heal to many targets
//		Place a healing pool 3x3
//   Distract enemy [decoy]
//   Place block [target floor tile -> place temporary obstacle. perhaps only enemies cant walk on it]
//      THIS IS OP : EG GET 5 PPL WITH LOTS OF MP BUFFS AND RANGE AND ONE BLOCK PER BOSS SIDE
//      AND WHEN ONE DISAPPEARS THE 5TH PERSON PLACES THERE AGAIN
//		E A S Y BOSS TRAP
//      Really cool idea though, just needs proper thought to prevent it being game-breaking.
//   You become untargetable for a few seconds
//   Spawns in an ally enemy who attackes other enemies
//   Spawns in an ally decoy that f l e e s
//   Navigator:
//      Unusable but shows the boss as a red dot on the minimap
//      Usable and adds a big area around the player to the minimap
//      Unusable but shows all enemies [bad idea, as we hopefully won't always be transmitting all of that]
//   Offensive:
//		Btec version of the Ent's TL tile attack
//		Attack an enemy and it spawns a pool 5x5 of TL tiles around it so basically splash damage
//		Deal a fixed amount of damage to the enemy [armour pierce?]
//   Teleportation [would need lots of thought to prevent it being OP, or just immense MP cost]

// Status effect based ideas:
//   Speed up for a few seconds
//   Attack speed up for a few seconds
//   Apply slow to the enemy

class GameState;
class Player;

class Special : public Item {
public:
	Special();
	Special(std::string name_, std::string desc_, ItemRarity rarity_, TEXTURE texture_, Stats bonus_, mp_t mpCost_, range_t range_,
		float cooldown_, target_count_t num_targets_, bool targetsEnemies_, bool targetsPlayers_);

	virtual void use(GameState *state, ID_t a, ID_t b) = 0;
	float getWisM(Player *A);
	stat_t getWis(Player *A);

	bool targetsEnemies, targetsPlayers;

	std::string desc;
	mp_t mpCost;
	float cooldown;
	range_t range;
	target_count_t num_targets;
};

class SHolyOrb : public Special {
public:
	SHolyOrb();
	void use(GameState *state, ID_t a, ID_t b);
};

class SDamager : public Special {
public:
	SDamager();
	void use(GameState *state, ID_t a, ID_t b);
};

class SBlessedSapling : public Special {
public:
	SBlessedSapling();
	void use(GameState *state, ID_t a, ID_t b);
};
