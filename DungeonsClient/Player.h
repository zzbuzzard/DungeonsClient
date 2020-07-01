#pragma once
#include "LivingEntity.h"
#include "SFML/Network.hpp"

#ifdef CLIENT
#include "Item.h"
class Player : public LivingEntity
{
public:
	Player(pi pos_, ID_t id);
	std::string name;

	void jerk(pi pos);
	void update(GameState *state);
	const pf getPosWorld() const;

	void draw(sf::RenderWindow *window, GameState *state);

	ITEM equips[NUM_EQUIP_LOCS];
	mp_t mpOn, maxMP;
	stat_t wis;
	level_t level = 1;
private:
	// static as there is only one local player
	//static sf::Vector2f jerkingTowards, jerkPos;
	static pf jerkMover, jerkPos;
	static float jerkTime;

	float TLdmgCool = 0;

	void resizeManaBar();
	sf::RectangleShape manabarbox, manabar;

	sf::Text nameText;
};
#else
#include "PlayerInfo.h"
#include <queue>
#include <map>
class GameState;
class Player : public LivingEntity
{
public:
	Player(pi pos_, sf::TcpSocket *sock);
	void update(GameState *state);

	void appendBaseInv(sf::Packet &packet);
	void calculateCombatStats();

	void handleDirectionInput(Dir d, GameState *state);
	void handleTarget(ID_t id);
	void fullHeal();

	const mp_t &getMP();

	void sendPacket(sf::Packet &packet);

	mp_t mpOn, maxMP;

	sf::TcpSocket *socket;
	std::string name;
	std::queue<Dir> queuedDirs;
	proc_moves_t processedMoves = -1;

	PlayerInfo pInfo;
	bool spawned = false;
private:
	float TLdmgCool = 0;

	static ID_t idd;
	float timeToNextHeal = 0,
		timeToNextMPHeal = 0;
	target_count_t numTargets = 0;
};
#endif
