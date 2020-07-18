#include "pch.h"
#include "Item.h"

Item *itemTypes[NUM_ITEMS] = {
	new Item("Helmet", ItemRarity::COMMON, T_TEST_HELMET, Equipment::HEAD, Stats(50, 0, 4, -5, 0, 0, 0, 0)), // what
	new Item("Cursed rags", ItemRarity::RARE, T_CURSED_RAGS, Equipment::HEAD, Stats(75, 0, 0, 8, 15, 0, 0, 0)), // what
	new Item("Slime facemask", ItemRarity::COMMON, T_SLIME_FACEMASK, Equipment::HEAD, Stats(50, 0, 3, -5, 0, 0, 0, 0)),
	new Item("Rusty helmet", ItemRarity::COMMON, T_RUSTY_HELMET, Equipment::HEAD, Stats(0, 0, 4, -3, 0, 0, 5, 0)),
	new Item("Cursed elven helmet", ItemRarity::RARE, T_CURSED_ELVEN_HELMET, Equipment::HEAD, Stats(-50, 0, 0, 10, 10, 0, 0, 0)),
	new Item("Vine helmet", ItemRarity::UNCOMMON, T_VINE_HELMET, Equipment::HEAD, Stats(50, 0, 4, 0, 0, 0, 5, 0)),
	new Item("Sand helmet", ItemRarity::COMMON, T_SAND_HELMET, Equipment::HEAD, Stats(0, 0, 3, -3, 0, 0, 0, 0)),
	new Item("Forest hood", ItemRarity::COMMON, T_FOREST_HOOD, Equipment::HEAD, Stats(0, 35, 3, 0, 0, 0, 0, 6)),

	new Item("Chestplate", ItemRarity::COMMON, T_TEST_ARMOUR, Equipment::TORSO, Stats(0, 0, 7, -7, 0, 0, 5, 0)), // what
	new Item("Plant armour", ItemRarity::UNCOMMON, T_PLANT_ARMOUR, Equipment::TORSO, Stats(40, 0, 5, 0, 0, 0, 10, 0)),
	new Item("Sand cape", ItemRarity::COMMON, T_SAND_CAPE, Equipment::TORSO, Stats(0, 0, 3, 5, 0, 0, 0, 0)),
	new Item("Scorpion scale armour", ItemRarity::UNCOMMON, T_SCORPION_SCALE_ARMOUR, Equipment::TORSO, Stats(0, 0, 5, 0, 4, 0, 0, 0)),
	new Item("Rusty armour", ItemRarity::COMMON, T_RUSTY_ARMOUR, Equipment::TORSO, Stats(0, 0, 6, -4, 0, 0, 0, 0)),
	new Item("Mage's robe", ItemRarity::COMMON, T_WARM_CLOAK, Equipment::TORSO, Stats(0, 70, 2, 0, 0, 10, 0, 10)),
	new Item("Cobweb armour", ItemRarity::COMMON, T_COBWEB_ARMOUR, Equipment::TORSO, Stats(0, 0, 6, 5, 0, 8, 0, 0)), // TODO get rid of... how the fuck do you make armour out of cobwebs
	new Item("Silk cape", ItemRarity::COMMON, T_SILK_CAPE, Equipment::TORSO, Stats(0, 0, 4, 8, 0, 0, 6, 0)),
	new Item("Wooden armour", ItemRarity::COMMON, T_WOODEN_ARMOUR, Equipment::TORSO, Stats(0, 0, 7, -6, 0, 0, 0, 5)),
	new Item("Beast hide armour", ItemRarity::UNCOMMON, T_BEAST_HIDE_ARMOUR, Equipment::TORSO, Stats(0, 0, 7, 5, 5, 0, 0, 0)),

	new Item("Speed charm", ItemRarity::UNCOMMON, T_TEST_ACCESSORY, Equipment::ACCESSORY, Stats(0, 0, 0, 10, 0, 0, 0, 0)),
	new Item("Sandstorm amulet", ItemRarity::UNCOMMON, T_SANDSTORM_AMULET, Equipment::ACCESSORY, Stats(0, 0, 4, 10, 0, 0, 0, 0)),
	new Item("Recovery charm", ItemRarity::UNCOMMON, T_RECOVERY_CHARM, Equipment::ACCESSORY, Stats(0, 0, 0, 0, 0, 0, 12, 0)),
	new Item("Desert ring", ItemRarity::UNCOMMON, T_DESERT_RING, Equipment::ACCESSORY, Stats(0,0,0,0,12,0,0,0)),
	new Item("Slime essence", ItemRarity::UNCOMMON, T_SLIME_ESSENCE, Equipment::ACCESSORY, Stats(40,0,0,-3,0,0,0,0)),
	new Item("Cursed cactus eye", ItemRarity::RARE, T_CURSED_CACTUS_EYE, Equipment::ACCESSORY, Stats(35,0,0,0,12,12,0,0)),
	new Item("Corrupt amulet", ItemRarity::UNCOMMON, T_CORRUPT_AMULET, Equipment::ACCESSORY, Stats(-25,0,-5,0,15,0,0,0)),

	new Weapon("Wooden bow", ItemRarity::COMMON, T_TEST_BOW, Stats(0,0,0,0,0,0,0,0), 35, 6, 0.3f, T_ARROW, 1),
	new Weapon("Bone bow", ItemRarity::COMMON, T_DESERT_BOW, Stats(0,0,0,0,0,0,0,0), 55, 7, 0.4f, T_ARROW, 1),
	new Weapon("Golden bow", ItemRarity::UNCOMMON, T_GOLDEN_BOW, Stats(0,0,0,5,0,0,0,0), 25, 9, 0.1f, T_GOLD_ARROW, 2), // buff and move drop
	new Weapon("Earth staff", ItemRarity::UNCOMMON, T_EARTH_STAFF, Stats(0,0,0,0,0,10,0,5), 60, 8, 0.35f, T_DUST, 2),
	new Weapon("Fire staff", ItemRarity::UNCOMMON, T_FIRE_STAFF, Stats(0,0,0,0,0,10,0,0), 35, 8, 0.25f, T_FIREBALL, 2),
	new Weapon("Sandstone sword", ItemRarity::UNCOMMON, T_SAND_SWORD, Stats(0,0,5,-3,0,0,0,0), 80, 3, 0.2f, T_SAND_SPIKE, 1),
	new Weapon("Cactus sword", ItemRarity::RARE, T_CACTUS_SWORD, Stats(0,0,3,0,0,0,0,0), 70, 4, 0.12f, T_THORN, 1),
	new Weapon("Scorpion dagger", ItemRarity::COMMON, T_SCORPION_DAGGER, Stats(0,0,2,8,0,0,5,0), 55, 3, 0.28f, T_SPINE, 1),
	new Weapon("Elven blade", ItemRarity::COMMON, T_ELVEN_BLADE, Stats(0,30,5,8,0,0,0,0), 75, 4, 0.165f, T_SHARD, 1),
	new Weapon("Elven bow", ItemRarity::COMMON, T_ELVEN_BOW, Stats(0,0,0,10,0,0,8,8), 80, 7, 0.45f, T_ARROW, 1),
	new Weapon("Spider fang dagger", ItemRarity::UNCOMMON, T_SPIDER_FANG_DAGGER, Stats(0,0,0,7,0,0,8,8), 65, 3, 0.18f, T_SHARD, 2),

	// Details in class constructor
	new SHolyOrb(),
	new SDamager(),
	new SBlessedSapling(),
};

Item::Item() {
	name = "???";
}

Item::Item(std::string name_, ItemRarity rarity_, TEXTURE texture_, Equipment location_, Stats bonus_)
	: name(name_), rarity(rarity_), texture(texture_), location(location_), bonus(bonus_)
{

}

Weapon::Weapon() {}

Weapon::Weapon(std::string name_, ItemRarity rarity_, TEXTURE texture_, Stats bonus_, damage_t damage_, range_t range_,
	float reload_, TEXTURE shot_, target_count_t num_targets_)
	: Item(name_, rarity_, texture_, Equipment::WEAPON, bonus_),
	damage(damage_), range(range_), reload(reload_), shot(shot_), num_targets(num_targets_)
{

}


Special::Special() {}

Special::Special(std::string name_, std::string desc_, ItemRarity rarity_, TEXTURE texture_, Stats bonus_, mp_t mpCost_, range_t range_,
	float cooldown_, target_count_t num_targets_, bool targetsEnemies_, bool targetsPlayers_)
	: Item(name_, rarity_, texture_, Equipment::SPECIAL, bonus_), mpCost(mpCost_), range(range_), cooldown(cooldown_),
	num_targets(num_targets_), targetsEnemies(targetsEnemies_), targetsPlayers(targetsPlayers_), desc(desc_)
{

}

#include "GameState.h"
#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Stats.h"

float Special::getWisM(Player *A) {
#ifdef CLIENT
	return Stats::getWisMultiplierOf(A->wis);
#else
	return A->pInfo.getOverallStats().getWisMultiplier();
#endif
}

stat_t Special::getWis(Player *A) {
#ifdef CLIENT
	return A->wis;
#else
	return A->pInfo.getOverallStats().stats[static_cast<int>(Stat::WIS)];
#endif
}


SHolyOrb::SHolyOrb()
	: Special("Sun's orb", "Heals a target player\nBase heal: 50", ItemRarity::UNCOMMON, T_HOLY_ORB, Stats(30, 0, 0, 0, 0, 0, 0, 0), 70, 10, 3.0f, 1, false, true)
{
}

void SHolyOrb::use(GameState *state, ID_t a, ID_t b) {
	// Player on player
	if (a < 0 || b < 0) return;
	if (state->idToPlayer.find(a) == state->idToPlayer.end() ||
		state->idToPlayer.find(b) == state->idToPlayer.end()) return;
	Player *A = state->idToPlayer[a];
	Player *B = state->idToPlayer[b];
	float W = getWisM(A);
	int heal = (int)(50 * W);
#ifdef CLIENT
	state->addEntity(new Projectile(T_HEAL_DUST, A->getPosWorldCentered(), B, -heal));
#else
	B->takeDamage(-heal, a);
#endif
}


SDamager::SDamager()
	: Special("Dust storm", "Base damage: 150", ItemRarity::UNCOMMON, T_DUST, Stats(0, 0, 0, 0, 5, 0, 0, 0), 50, 6, 3.0f, 1, true, false)
{
}

void SDamager::use(GameState *state, ID_t a, ID_t b) {
	// Player on enemy
	if (a < 0 || b >= 0) return;
	if (state->idToPlayer.find(a) == state->idToPlayer.end() ||
		state->idToEnemy.find(b) == state->idToEnemy.end()) return;
	Player *A = state->idToPlayer[a];
	Enemy *B = state->idToEnemy[b];
	float W = getWisM(A);
	int damage = (int)(150 * W);
#ifdef CLIENT
	state->addEntity(new Projectile(T_FIREBALL, A->getPosWorldCentered(), B, damage));
#else
	B->takeDamage(damage, a);
#endif
}



SBlessedSapling::SBlessedSapling()
	: Special("Blessed sapling", "Spawns a healing pool\nPool grows by 1 per 15 WIS", ItemRarity::RARE, T_BLESSED_SAPLING, Stats(0, 0, 0, 0, 0, 0, 0, 0), 120, 10, 6.0f, 1, false, true)
{
}

// The server uses this to spawn in the tiles
// The client, however, will hear about these tiles from the server, so just shows some random projectile thing
void SBlessedSapling::use(GameState *state, ID_t a, ID_t b) {
	// Player on player
	if (a < 0 || b < 0) return;
	if (state->idToPlayer.find(a) == state->idToPlayer.end() ||
		state->idToPlayer.find(b) == state->idToPlayer.end()) return;
	Player *A = state->idToPlayer[a];
	Player *B = state->idToPlayer[b];
#ifdef CLIENT
	state->addEntity(new Projectile(T_BLESSED_SAPLING, A->getPosWorldCentered(), B, 0));
#else
	stat_t wis = getWis(A);
	int size = 0;
	if (wis >= 15) size = 1;
	if (wis >= 30) size = 2;
	if (wis >= 45) size = 3;
	if (wis >= 60) size = 4;

	auto pis = util::borderPoints(B->getCollisionPos(), pi(1, 1), 0, size);
	state->sendTLtilesPacket(TLTileType::HEALING_POOL, pis);
#endif
}




