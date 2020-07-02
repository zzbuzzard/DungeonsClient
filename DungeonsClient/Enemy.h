#pragma once
#include "LivingEntity.h"

// Enemy concepts:
//  1) Just walk towards you and shoot. Most will be like this.
//  2) Stationary and shoots multiple players
//  3) Debuffs - see below
//  4) The longer it has you in range, the faster it attacks.
//  5) An enemy that spawns damaging tiles
//  6) An enemy that spawns non damaging, non walkable tiles that trap a player in with it

// Boss concepts:
//  1) Stationary and spawns damaging tiles moving left to right
//  2) Moves slowly and targets many with slow shots.
//     Once v. near to a player (3), becomes stationary,
//      targets only 1 player and shoots them REALLY QUICK LOADS
//    Continues until they are about (5) tiles away.
//  3) Boss that spawns in other enemies. The enemies flood in from outside.
//  4) Boss that is invincible until a central thing is destroyed.
//     Pretty fast. Needs to be tanked by another player.
//     Maybe deals more damage when close to the central beacon?
//     Or, better, medium range but can target multiple players with WEAK status effect.
//      Must be lured away from the central thing by a tank. However, there are slowing enemies at the outskirts.

// Debuff concepts:
//  1) Slowed
//  2) Poisoned
//  3) Range reduced

// Buff concepts:
//  1) Heal
//  2) Move faster
//  3) Deal more damage

#ifndef CLIENT
#include <map>
#include <vector>
#include "Item.h"
#include <memory>

typedef std::pair<ITEM, float> itemProb;
typedef std::vector<itemProb> LootList;
#endif

class Enemy : public LivingEntity
{
public:
	bool isBoss = false;
#ifdef CLIENT
	Enemy(EntitySpecies spec, pi pos_, int life, ID_t id, pi tileSize_ = pi(1, 1));
#else
	Enemy(EntitySpecies spec, pi pos_, int life, int triggerRange_, exp_t exp_, pi tileSize_ = pi(1, 1));
	~Enemy();

	void takeDamage(int dmg, ID_t damageDealer);
	void onDie(GameState *state);
	void update(GameState *state);

	void setTriggerRange(int t);

	std::shared_ptr<int> decrementVar;
	//int *decrementVariable = nullptr;

	const exp_t exp;
	pi spawnP; // Spawners do some shady stuff and need access to this

	bool dropsLoot = true;

protected:
	void moveTowardsPlayer(GameState *state, bool returnsToSpawn=true, bool ignoreOtherEntities=false);
	void moveToSpawn(GameState *state);
	void targetClosestPlayer(GameState *state);
	void targetClosestPlayers(GameState *state, int k);
	void fleeClosest(GameState *state);
	bool targetsOutOfRange(GameState *state);
	int triggerRange;

	const LootList *loot = nullptr;
	float bfsCooldown;
	float retargetCooldown = -1;

private:
	std::map<ID_t, int> damageMap;

	static ID_t idd;
#endif
};

