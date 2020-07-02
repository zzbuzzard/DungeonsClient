#include "pch.h"
#include "PlayerInfo.h"

#ifdef CLIENT
#include "ConnectionManager.h"
#include "Controller.h"
#include "Packet.h"
#endif

Stats PlayerInfo::getOverallStats() {
	return baseStats + inv.equipmentBonus;
}

bool PlayerInfo::lootExists(pi pos) {
	return lootMap.find(pos) != lootMap.end();
}

const std::vector<ITEM> *PlayerInfo::getLootAt(pi pos) {
	if (lootMap.find(pos) == lootMap.end()) return nullptr;
	return &lootMap[pos];
}

void PlayerInfo::pickUp(pi pos, item_num_t ind) {
	if (lootMap.find(pos) == lootMap.end()) return;
	auto &items = lootMap[pos];

	if (ind < 0 || ind >= items.size()) return;

	int i = inv.getFirstFree();
	if (i == -1) return;

#ifdef CLIENT
	cout << "Client sending a pickup request" << endl;

	// We expect a new inventory confirmation after this
	inv.sends++;

	sf::Packet packet;
	packet.append(&P_PICKUP_LOOT, sizeof(Packet));
	packet.append(&ind, sizeof(item_num_t));

	connection->send(packet);

#else
	ITEM grab = items[ind];
	items.erase(items.begin() + ind);
	inv.items[i] = grab;

	if (items.size() == 0) {
		lootMap.erase(pos);
	}
#endif
}


ITEM PlayerInfo::dropItem(item_num_t ind) {
	if (ind < 0 || ind >= INV_SIZE || inv.items[ind] == I_NONE)
		return I_NONE;
	ITEM item = inv.items[ind];
	inv.items[ind] = I_NONE;

#ifdef CLIENT
	inv.sends++;

	// TODO Send P_DROP_ITEM ind
	sf::Packet packet;
	packet.append(&P_DROP_ITEM, sizeof(Packet));
	packet.append(&ind, sizeof(item_num_t));
	connection->send(packet);
#endif
	return item;
}


void PlayerInfo::createLoot(pi pos, const std::vector<ITEM> &items) {
	if (lootMap.find(pos) != lootMap.end()) {
		for (ITEM i : items) lootMap[pos].push_back(i);
	}
	else {
		lootMap[pos] = items;
	}
}

void PlayerInfo::clearLoot() {
	lootMap.clear();
}

level_t PlayerInfo::getPlayerLevel() {
	if (XP == lastLevelXP) return lastLevel;
	lastLevel = getLevel(XP);
	lastLevelXP = XP;
	return lastLevel;
}

exp_t PlayerInfo::getLeftoverXP() {
	exp_t Xp = XP;
	level_t n = 1;
	exp_t xp;
	while (true) {
		if (n >= 100) break;
		xp = getLevelUpXP(n);
		if (xp <= Xp) {
			Xp -= xp;
			n++;
		}
		else {
			break;
		}
	}
	return Xp;
}


#ifdef CLIENT
void PlayerInfo::removeItem(pi pos, item_num_t ind) {
	if (lootMap.find(pos) == lootMap.end()) return;
	auto &items = lootMap[pos];
	if (ind < 0 || ind >= items.size()) return;
	int i = inv.getFirstFree();
	if (i == -1) return;
	ITEM grab = items[ind];
	items.erase(items.begin() + ind);
	inv.items[i] = grab;
	if (items.size() == 0) {
		lootMap.erase(pos);
	}
}


#include "GameState.h"
#include "DamageText.h"

// CONSIDER passing instead a pointer to LevelUpUI class or XPUi class or soemthing
void PlayerInfo::levelUpUI(GameState *state, level_t lev) {
	state->gameMenu.chat.displaySystemMessage("You reached level " + std::to_string(lev) + "!", sf::Color::Green);
}

void PlayerInfo::gainXP(exp_t gain, GameState *state) {
	if (gain <= 0) return;

	level_t a = getPlayerLevel();
	XP += gain;
	level_t b = getPlayerLevel();

	if (a != b) {
		for (level_t i = a + 1; i <= b; i++) {
			levelUpUI(state, i);
			statPoints += Stats::getStatPointsAt(i);
		}
	}
	float yOffset = - 0.5f * (float)PIXEL_PER_UNIT;
	state->addEntity(new DamageText((Entity*)state->getLocalPlayer(), pf(0, yOffset), gain, true));
}
#else
void PlayerInfo::gainXP(exp_t gain) {
	level_t a = getPlayerLevel();
	XP += gain;
	level_t b = getPlayerLevel();

	if (a != b) {
		for (level_t i = a + 1; i <= b; i++) {
			statPoints += Stats::getStatPointsAt(i);
		}
		cout << "Player hit level " << (int)b << endl;
		cout << "Stat points = " << statPoints << endl;
	}
}
#endif

exp_t PlayerInfo::getXP() {
	return XP;
}

void PlayerInfo::setXP(exp_t xp) {
	XP = xp;
}

typedef std::pair<exp_t, exp_t> Pi;

// xpP[i] = Pi(a, b)
//  meaning level to go from level n->n+1 takes a*n-b XP
//  for i*10<=n<(i+10)*10
Pi xpP[10] = {
	Pi(1000, 0),
	Pi(2000, 1000),
	Pi(4000, 50000),
	Pi(7500, 155000),
	Pi(15000, 455000),
	Pi(30000, 1205000),
	Pi(50000, 2405000),
	Pi(100000, 5905000),
	Pi(150000, 9905000),
	Pi(300000, 23405000)
};

level_t PlayerInfo::getLevel(exp_t XP) {
	level_t n = 1;
	exp_t xp;
	while (true) {
		if (n >= 100) break;
		xp = getLevelUpXP(n);
		if (xp <= XP) {
			XP -= xp;
			n++;
		}
		else {
			break;
		}
	}
	return n;
}

exp_t PlayerInfo::getLevelUpXP(level_t lev) {
	if (lev <= 0) return 0;
	if (lev >= 99) return 6500000;

	level_t ind = (lev-1) / 10;

	// e.g. 21 -> 2
	//      20 -> 1
	//      30 -> 2

	exp_t a = xpP[ind].first, b = xpP[ind].second;
	return a * (exp_t)lev - b;
}


void PlayerInfo::useStatPoint(Stat stat) {
	if (statPoints <= 0) return;
	if (!canUpgrade(stat)) return;

	cout << "Spending a stat point on " << statNames[(int)stat] << endl;

	statPoints--;

#ifdef CLIENT
	inv.sends++;
	sf::Packet p;
	p.append(&P_USE_STAT, sizeof(Packet));
	p.append(&stat, sizeof(Stat));
	connection->send(p);
#else
#endif
	if ((int)stat <= 1)
		baseStats.stats[(int)stat] += 5;
	else
		baseStats.stats[(int)stat] += 1;
}

void PlayerInfo::setBaseStats(Stats stats) {
	if (XP == -1)
		cout << "Playerinfo: MUST SET XP BEFORE BASESTATS" << endl;

	baseStats = stats;
	statPoints = baseStats.getSpareStatPointsOverall(getPlayerLevel());
	//cout << "Stat points unspent = " << statPoints << endl;
}


const int &PlayerInfo::getStatPoints() {
	return statPoints;
}

#define MAX_STAT_UPGRADES 40
bool PlayerInfo::canUpgrade(Stat stat) {

	// Def has a flexible max
	if (stat == Stat::DEF) {
		level_t lev = getPlayerLevel();
		stat_t maxDef = lev / 10;
		if (lev >= 5) maxDef++;
		if (lev >= 50) maxDef++;
		if (lev >= 75) maxDef++;
		if (lev >= 100) maxDef++;

		stat_t def = baseStats.getDef();
		if (def >= maxDef) return false;
	}

	//if (stat == Stat::DEF && getPlayerLevel() < 10) return false;

	int used = baseStats.getSpentStatPoints(stat);
	if (used >= MAX_STAT_UPGRADES) return false;
	return true;
}


Stats PlayerInfo::getBaseStats() {
	return baseStats;
}
