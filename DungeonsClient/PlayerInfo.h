#pragma once
#include "CSMode.h"
#include "Inventory.h"
#include "Stats.h"
#include "Util.h"
#include <map>

class PlayerInfo
{
public:
	Stats getOverallStats();

	bool lootExists(pi pos);
	void pickUp(pi pos, item_num_t ind);
	ITEM dropItem(item_num_t ind);
	void createLoot(pi pos, const std::vector<ITEM> &items);
	void clearLoot();
	const std::vector<ITEM> *getLootAt(pi pos);

#ifdef CLIENT
	void removeItem(pi pos, item_num_t ind);

	// UI Stuff
	void levelUpUI(GameState *state, level_t lev);
	void gainXP(exp_t gain, GameState *state);
#else
	void gainXP(exp_t gain);
#endif
	exp_t getXP();
	Stats getBaseStats();
	void setXP(exp_t xp);

	level_t getPlayerLevel();
	exp_t getLeftoverXP();
	static level_t getLevel(exp_t XP);
	static exp_t getLevelUpXP(level_t lev);

	void useStatPoint(Stat stat);
	void setBaseStats(Stats stats);
	bool canUpgrade(Stat stat);

	const int &getStatPoints();
private:

public:
	Inventory inv;

private:
	Stats baseStats;

	int statPoints = 0;

	level_t lastLevel = 0;
	exp_t lastLevelXP = -2;
	exp_t XP = -1;
	std::map<pi, std::vector<ITEM>, piComp> lootMap;
};

// Base stats
// Inventory
// LootMap
// XP
