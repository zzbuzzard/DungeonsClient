#pragma once
#include <vector>
#include <list>
#include <map>
#include <set>
#include "CSMode.h"
#include "Util.h"
#include "World.h"
#include "TopLayerTile.h"

#ifdef CLIENT
#include "PlayerInfo.h"
#include "PlayerViewUI.h"
#include "TileBox.h"
#include "Minimap.h"
#include "State.h"
#include "GameMenu.h"
#include "LootUI.h"
#include "LivingEntity.h"
#include <deque>
#else
#include "SFML/Network.hpp"
#include "Item.h"
#endif

class Entity;
class Player;
class Enemy;
class LivingEntity;

// The client needs maps from ID to pointers
// The server, however, does not.

#ifdef CLIENT
class GameState : public State
#else
class GameState
#endif
{
// Public functions
public:
#ifdef CLIENT
	GameState(sf::RenderWindow *window);
	~GameState();

	// handle output from server
	void handleHello(const void *data);
	void handleUpdate(const void *data);
	void handleEquipmentUpdate(const void *data);
	void handleBaseStatsInv(const void *data);
	void handleLoot(const void *data);
	void handleEnterDungeon(const void *data);
	void handlePlayerDeath(const void *data);
	void handleMessage(const void *data);
	void handleSpecial(const void *data);
	void handleChangeTile(const void *data);
	void handleTlTile(const void *data);
	void handleTlTiles(const void *data);
	void handleConfirmedItem(const void *data);
	void handleRequestedUsername(const void *data);
	void handleMyPos(pi p);

	void draw(const sf::View *v);
	void resize(const sf::View *v);
	sf::Vector2f whereToCenter();

	Player *getLocalPlayer();
	pi getLocalPlayerPos();
	pf getLocalPlayerPosWorld();
	bool isInView(pi pos);

	void initialiseOverworld(seed_t seed, ID_t worldID);
	void initialiseDungeon(BiomeType b, seed_t seed, ID_t worldID);
#else
	GameState(BiomeType biome);
	~GameState();

	const bool isOverworld;
	bool isAlive() const;

	void getUpdatePacket(sf::Packet &packet);
	void getEquipmentPacket(sf::Packet &packet);
	void sendPacketToAllPlayers(sf::Packet &packet);
	Player *getClosestPlayer(pi pos);
	std::vector<Player*> getClosestPlayers(pi pos, int k);
	void disconnectPlayer(ID_t id);
	void playerMoved(ID_t id);
	void playerDied(Player *p);
	void playerRespawned(Player *p);
	void specialUsed(Special *s, ID_t a, ID_t b, sf::Packet &packet);

	std::vector<pi> sendTLtilesPacket(TLTileType type, const std::vector<pi> points); // Returns the list that was sent

	Dir BFSTowards(pi a, pi b, range_t depth, pi tileSize = pi(1, 1)); // Get the direction from a to b with a BFS
	Dir BFSTowardsRanged(pi a, pi b, range_t depth, range_t range, pi tileSize = pi(1, 1)); // Get the direction from a to a point in range of b with a BFS
	Dir BFSTowardsNoEntities(pi a, pi b, range_t depth, pi tileSize = pi(1, 1)); // Get the direction from a to b with a BFS
#endif

	void update();
	void addEntity(Entity* entity);
	void addPermTopLayerTile(Entity* entity);
	bool addTempTopLayerTile(Entity* entity, bool sendPacket = false);
	
	Entity *getTLTile(pi pos);
	bool canMove(pi pos);
	bool canMove(pi pos, pi tileSize);
	bool canMoveDir(pi pos, pi tileSize, Dir d);
	bool canMoveDirWorld(pi pos, pi tileSize, Dir d); //Only checks the world, not entities

// Private functions
private:
#ifdef CLIENT
	void handlePlayerRespawn();
	void deleteAllEntities();

	void useSpecial();
	void cleanseSpecialTargets();

	void loadWorld(World *world);
	void adjustTileBox();
	Player *createNewPlayer(ID_t id, pos_t x, pos_t y);
#endif
	void removeDead();

// Public variables
public:
#ifdef CLIENT
	PlayerInfo pInfo;
	GameMenu gameMenu; // TODO: Move back to private after XP/level up UI is sorted out

	double specialUsedTime = -1;

	Entity *clickedObj = nullptr;
	Entity *rightClickedObj = nullptr;

	bool ready = false;
	bool isDead = false;
	bool waitingToRespawn = false;
	World *overworld = nullptr;

	bool lootChanged = false;

	pi lastSeenPlayerPos;
#endif
	std::map<ID_t, Player*> idToPlayer; // public as main needs to access for socket shenanigans
	std::map<ID_t, Enemy*> idToEnemy;
	std::set<pi, piComp> *takenPos;

	// Server: initialise in constructor
	// Client: initialise when we receive the seed
	World *currentWorld = nullptr;

// Private variables
private:
#ifdef CLIENT
	LootUI lootUI;
	PlayerViewUI playerViewUI;

	Player *myPlayer;
	//Dir queuedDir = D_NONE; TODO: But be careful, this would complicate things {server count etc - dont send packet, keep it local until we turn}
	TileBox tileBox;
	Minimap *currentMinimap = nullptr;

	DungeonEntrance *currentDungeonEntrance = nullptr;

	std::vector<ID_t> specialTargets;
	std::map<ID_t, Minimap*> rememberedMaps;

	std::deque<Dir> moves;
	proc_moves_t queueMoveNum = 0;
	proc_moves_t serverProc = 0;
	double lastDownTime;
#else
	std::vector<std::vector<int16_t> > tileMarkerNum;
	std::vector<std::vector<Dir> > tileMarkerDir;
#endif
	std::set<pi, piComp> *newTakenPos;
	//std::list<Entity*> topLayerTiles;
	std::list<Entity*> entities;
};