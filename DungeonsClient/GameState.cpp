#include "pch.h"
#include "GameState.h"
#include <iostream>
#include "Entity.h"
#include "Player.h"
#include <queue>
#include "Biome.h"
#include "Enemy.h"

#ifdef CLIENT
#include "Util.h"
#include "Controller.h"
#include "ConnectionManager.h"
#include "Packet.h"
#include "DisplayTileEntity.h"
#include "LootChest.h"
#else
#include "State.h"
#include "Packet.h"
#include "../../DungeonsServer/DungeonsServer/ServerGameStates.h"
#endif

using std::cout;
using std::endl;

#ifdef CLIENT
bool dungeonEnterButtonPressed = false;
bool specialButtonPressed = false;

// usually x=13, y=7
// x=35, y=17 alright for debugging
#define XVIEW 13
#define YVIEW 7

GameState::GameState(sf::RenderWindow *window) : State(window), tileBox(2*XVIEW+1, 2*YVIEW+1), lootUI(&pInfo) {
	g_gameState = true;

	connection = new ConnectionManager();
	cout << "Connecting to server..." << endl;
	connection->connect();
	cout << "Connected!" << endl;

	myPlayer = nullptr;
	resize(&window->getView());

	takenPos = new std::set<pi, piComp>();
	newTakenPos = new std::set<pi, piComp>();
}

GameState::~GameState() {
	g_gameState = false;

	delete takenPos;
	delete newTakenPos;
}

void GameState::resize(const sf::View *v) {
	State::resize(v);
}
#else
//#include "Enemy1.h"
GameState::GameState(BiomeType biome) : isOverworld(biome == BiomeType::NONE) {
	takenPos = new std::set<pi, piComp>();
	newTakenPos = new std::set<pi, piComp>();

	currentWorld = new World();

	if (isOverworld) {
		currentWorld->generateMap((seed_t)(time(0)));
	}
	else {
		currentWorld->generateDungeon((seed_t)util::randint(0, 1<<31), biome);
	}
	int X = currentWorld->tiles.size();
	int Y = currentWorld->tiles[0].size();

	tileMarkerNum = std::vector<std::vector<int16_t> >(X, std::vector<int16_t>(Y, 0));
	tileMarkerDir = std::vector<std::vector<Dir> >(X, std::vector<Dir>(Y, D_NONE));
}

GameState::~GameState() {
	delete takenPos;
	delete newTakenPos;
	delete currentWorld;
}

bool GameState::isAlive() const {
	if (isOverworld) return true;
	if (currentWorld->bossDefeated) {
		if (idToPlayer.size() == 0) {
			return false;
		}
	}
	return true;
}
#endif

bool charHeld = false;

void GameState::update() {
#ifdef CLIENT
	State::update();

	if (isDead && !waitingToRespawn) {
		if (windowFocus && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
			handlePlayerRespawn();
		}
	}

	if (totalTime - specialUsedTime > 1.0f) specialUsedTime = -1;

	if (totalTime - serverLastHeard > 5.0f && totalTime - serverLastHeard < 5.05f) {
		gameMenu.chat.displaySystemMessage("Haven't heard from server for 5 seconds...", sf::Color::Red);
	}

	clickedObj = nullptr;
	rightClickedObj = nullptr;

	if (currentMinimap != nullptr)
		currentMinimap->update(this);

	if (ready && currentWorld != nullptr && !isDead && currentWorld->initialised && myPlayer != nullptr && connection->isConnected()) {
		adjustTileBox();

		gameMenu.update(this);
		lootUI.update(this);

		Dir d = getCurrentDir();
		if (d != D_NONE) {
			if (myPlayer->currentMovement == D_NONE) {
				pi newPos = myPlayer->getPos() + dirOffset[d];
				if (canMove(newPos)) {
					myPlayer->currentMovement = d;

					sf::Packet packet;
					packet.append(&P_MOVE, sizeof(Packet));
					packet.append(&d, sizeof(Dir));

					//cout << "Sending direction packet" << endl;
					connection->send(packet);
					moves.push_back(d);

					//int newX = myPlayer->getPos().x + dirOffset[d].x;
					//int newY = myPlayer->getPos().y + dirOffset[d].y;

					//pi newPos = myPlayer->getPos() + dirOffset[d];

					//pi newPos = myPlayer->getCollisionPos();
					if (currentWorld->dungeonEntrances.find(newPos) != currentWorld->dungeonEntrances.end()) {
						currentDungeonEntrance = &currentWorld->dungeonEntrances[newPos];

						// TODO: Enable UI for entering dungeon
					}
					else {
						currentDungeonEntrance = nullptr;
						// TODO: Disable UI for entering dungeon
					}
				}
			}
		}
	}

	if (!isDead && sf::Keyboard::isKeyPressed(sf::Keyboard::E) && windowFocus) {
		if (!dungeonEnterButtonPressed) {
			if (!isTyping && currentDungeonEntrance != nullptr) {
				cout << "Attempting to enter a dungeon" << endl;
				sf::Packet p;
				p.append(&P_ENTER_DUNGEON, sizeof(Packet));
				connection->send(p);
			}
		}
		dungeonEnterButtonPressed = true;
	}
	else
		dungeonEnterButtonPressed = false;


	if (!isDead && sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && windowFocus) {
		if (!specialButtonPressed) {
			if (!isTyping) {
				useSpecial();
			}
		}
		specialButtonPressed = true;
	}
	else
		specialButtonPressed = false;

	

	// if we're moving
	//sf::Packet packet;
	//connection->send(packet);

#else
	//tt--;
	//if (tt <= 0) {
	//	tt = 3000;
	//	//if (idToPlayer.size() == 1)
	//	//	tt = 75;
	//	//if (idToPlayer.size() == 2)
	//	//	tt = 50;
	//	//if (idToPlayer.size() == 3)
	//	//	tt = 30;
	//	//if (idToPlayer.size() > 3)
	//	//	tt = 15;
	//	if (idToPlayer.size() > 0 && idToEnemy.size() < 100) {
	//		float t = util::rand(0, 1);
	//		if (t < 0.2f)
	//			addEntity(new EEyeSpore(pi(util::randint(-5, 5), util::randint(-5, 5))));
	//		else {
	//			if (t < 0.5f)
	//				addEntity(new EMuncher(pi(util::randint(-5, 5), util::randint(-5, 5))));
	//			else
	//				addEntity(new Enemy1(pi(util::randint(-5, 5), util::randint(-5, 5))));
	//		}
	//	}
	//}
	currentWorld->update(this);
#endif

	newTakenPos->clear();
	// Update top layer tiles -> update other entities

	// TOP LAYER TILE LOOP

	if (currentWorld != nullptr && currentWorld->initialised) {
		for (auto it = currentWorld->TLset2.begin(); it != currentWorld->TLset2.end(); ++it) {
			pi pos = *it + currentWorld->origin;
			Entity *e = currentWorld->topLayerTileArray[pos.x][pos.y];

			e->update(this);
			if (!e->canWalk) {
				newTakenPos->insert(pos);
			}
		}
	}

	// ENTITY LOOP
	for (auto it = entities.begin(); it != entities.end(); ++it) {
		// Server/client conditions for not updating dead players
		if ((*it)->type == ET_PLAYER) {
#ifdef CLIENT
			if (isDead && ((Player*)(*it))->ID == myPlayer->ID)
				continue;
#else
			if (!((Player*)(*it))->spawned)
				continue;
#endif
		}
		(*it)->update(this);
		if (!(*it)->canWalk) {
			pi pos = (*it)->getCollisionPos();

			if ((*it)->tileSize != pi(1, 1)) {
				pi t = (*it)->tileSize;
				for (int i = 0; i < t.x; i++) {
					for (int j = 0; j < t.y; j++) {
						pi pos2 = pos + pi(i, j);
						newTakenPos->insert(pos2);
					}
				}
			}
			else {
				newTakenPos->insert(pos);
			}
		}
	}
	std::set<pi, piComp> *t = takenPos;
	takenPos = newTakenPos;
	newTakenPos = t;

#ifdef CLIENT
	// Handle clickedObj
	if (myPlayer != nullptr && !isDead) {
		if (clickedObj != nullptr) {
			if (clickedObj->type == ET_ENEMY) {
				LivingEntity *liv = (LivingEntity*)clickedObj;
				ID_t target_id = liv->ID;

				sf::Packet packet;
				packet.append(&P_TARGET, sizeof(Packet));
				packet.append(&target_id, sizeof(ID_t));

				myPlayer->handleTarget(target_id);

				cout << "Sending target packet: id = " << target_id << endl;
				connection->send(packet);
			}
			if (clickedObj->type == ET_PLAYER) {
				if (clickedObj != myPlayer) {
					playerViewUI.setDisplay((Player*)clickedObj);
				}
			}
		}
		else {
			if (mouseClicked) {
				playerViewUI.close();
			}
		}

		// Special targetting
		if (rightClickedObj != nullptr) {
			LivingEntity *liv = (LivingEntity*)rightClickedObj;
			ID_t id = liv->ID;
			if (vectorContains(specialTargets, id)) {
				for (int i = 0; i < specialTargets.size(); i++) {
					if (specialTargets[i] == id) {
						specialTargets.erase(specialTargets.begin() + i);
						break;
					}
				}
			}
			else {
				specialTargets.push_back(id);
			}
		}


		myPlayer->popTargets(pInfo.inv.getNumTargets());
		cleanseSpecialTargets();

		for (ID_t i : myPlayer->targets) {
			if (i < 0) {
				if (idToEnemy.find(i) != idToEnemy.end())
					idToEnemy[i]->isTargetted = true;
			}
			//else {
			//	if (idToPlayer.find(i) != idToPlayer.end())
			//		idToPlayer[i]->isTargetted = true;
			//}
		}

		for (ID_t i : specialTargets) {
			if (i < 0) {
				if (idToEnemy.find(i) != idToEnemy.end())
					idToEnemy[i]->isSpecialTargetted = true;
			}
			else {
				if (idToPlayer.find(i) != idToPlayer.end())
					idToPlayer[i]->isSpecialTargetted = true;
			}
		}

	}

	lootChanged = false;
#endif

	removeDead();
}

#ifdef CLIENT

void GameState::useSpecial() {
	ITEM item = pInfo.inv.equips[(int)Equipment::SPECIAL];
	if (item != I_NONE) {
		Special *s = (Special*)itemTypes[item];

		for (ID_t id : specialTargets) {
			bool enemy = id<0;
			LivingEntity *liv = nullptr;
			if (enemy && idToEnemy.find(id) != idToEnemy.end()) liv = idToEnemy[id];
			if (!enemy && idToPlayer.find(id) != idToPlayer.end()) liv = idToPlayer[id];

			if (liv != nullptr) {
				if ((enemy && s->targetsEnemies) || (!enemy && s->targetsPlayers)) {
					if (util::taxicabSize(myPlayer->getCollisionPos(), liv->getCollisionPos(), liv->tileSize) <= s->range) {
						if (myPlayer->mpOn >= s->mpCost) {
							myPlayer->mpOn -= s->mpCost;
							s->use(this, myPlayer->ID, id);

							sf::Packet p;
							p.append(&P_USE_SPECIAL, sizeof(Packet));
							p.append(&myPlayer->ID, sizeof(ID_t));
							p.append(&id, sizeof(ID_t));

							specialUsedTime = totalTime;

							cout << "Sending special use request" << endl;
							connection->send(p);
						}
					}
				}
			}
		}
	}

}

void GameState::cleanseSpecialTargets() {
	ITEM item = pInfo.inv.equips[(int)Equipment::SPECIAL];

	// default if i==I_NONE
	bool en = false, pl = false;
	int c = 0;

	if (item != I_NONE) {
		Special *s = (Special*)itemTypes[item];
		en = s->targetsEnemies;
		pl = s->targetsPlayers;
		c = s->num_targets;
	}

	// Remove invalid targets [e.g. after changing special] or dead targets
	for (int i = 0; i < specialTargets.size(); i++) {
		ID_t id = specialTargets[i];

		// Player
		if (id >= 0) {
			if (!pl || idToPlayer.find(id) == idToPlayer.end()) {
				specialTargets.erase(specialTargets.begin() + i);
				i--;
			}
		}
		else {
			if (!en || idToEnemy.find(id) == idToEnemy.end()) {
				specialTargets.erase(specialTargets.begin() + i);
				i--;
			}
		}
	}

	// Remove excess targets
	while (specialTargets.size() > c) {
		specialTargets.erase(specialTargets.begin());
	}
}

Player *GameState::getLocalPlayer() {
	return myPlayer;
}

pi GameState::getLocalPlayerPos() {
	if (myPlayer == nullptr) return pi(0, 0);
	return myPlayer->getCollisionPos();
}

pf GameState::getLocalPlayerPosWorld() {
	if (myPlayer == nullptr) return pf(0, 0);
	return myPlayer->getPosWorldCentered();
}


bool GameState::isInView(pi pos) {
	pi player = getLocalPlayerPos();
	if (pos.x >= player.x - XVIEW - 3 && pos.x <= player.x + XVIEW + 3) {
		if (pos.y >= player.y - YVIEW - 3 && pos.y <= player.y + YVIEW + 3) {
			return true;
		}
	}
	return false;
}


void GameState::initialiseOverworld(seed_t seed, ID_t worldID) {
	overworld = new World();
	overworld->worldID = worldID;
	overworld->generateMap(seed);

	loadWorld(overworld);
}

void GameState::initialiseDungeon(BiomeType b, seed_t seed, ID_t worldID) {
	cout << "Initialising dungeon" << endl;

	World *w = new World();
	w->worldID = worldID;
	w->generateDungeon(seed, b);

	loadWorld(w);
}

void GameState::loadWorld(World *world) {
	// Remove stuff from the world we are leaving: loot and entities
	pInfo.clearLoot();
	deleteAllEntities();
	myPlayer->setPos(pi(0, 0));

	if (currentWorld != overworld && currentWorld != nullptr) delete currentWorld;
	currentWorld = world;

	ID_t id = world->worldID;
	if (rememberedMaps.find(id) == rememberedMaps.end()) {
		currentMinimap = new Minimap(0, 0, currentWorld->tiles.size(), currentWorld->tiles[0].size(), currentWorld);
		rememberedMaps[id] = currentMinimap;
	}
	else {
		currentMinimap = rememberedMaps[id];
	}

	tileBox.setWorld(currentWorld);
	adjustTileBox();

	int X = currentWorld->tiles.size();
	int Y = currentWorld->tiles[0].size();

	const auto &cont = currentWorld->dungeonEntrances;
	for (auto it = cont.begin(); it != cont.end(); ++it) {
		BiomeType b = it->second.biomeType;

		// TODO: Change perm to temp so update() is called
		//  And make a more complex class: DungeonEntranceEntity
		//  Which has a mechanism in place to make it disappear at whatever time
		addPermTopLayerTile(new DisplayTileEntity(it->first, biomes[(int)b].doorTexture));
	}
}


void GameState::adjustTileBox() {
	pf p = myPlayer->getPosWorld() / (float)PIXEL_PER_UNIT;
	//pi pos = myPlayer->getCollisionPos();
	pi pos = pi(util::round(p.x), util::round(p.y));

	tileBox.adjust(pos.x - XVIEW, pos.x + XVIEW, pos.y - YVIEW, pos.y + YVIEW, currentWorld);
	if (currentMinimap != nullptr)
		currentMinimap->adjust(pos.x, pos.y, currentWorld);
}

#endif

Entity *GameState::getTLTile(pi pos) {
	if (currentWorld == nullptr || !currentWorld->initialised) return nullptr;
	pos += currentWorld->origin;
	return currentWorld->topLayerTileArray[pos.x][pos.y];
}

bool GameState::canMove(pi pos) {
	if (takenPos->find(pos) != takenPos->end()) return false;
	if (newTakenPos->find(pos) != newTakenPos->end()) return false;

	if (currentWorld != nullptr) {
		if (!currentWorld->canWalk(pos)) return false;
		pos += currentWorld->origin;
		Entity *e = currentWorld->topLayerTileArray[pos.x][pos.y];
		if (e != nullptr && !e->canWalk) return false;
	}
	return true;
}

bool GameState::canMove(pi pos, pi tileSize) {
	for (auto x = 0; x < tileSize.x; x++) {
		for (auto y = 0; y < tileSize.y; y++) {
			if (!canMove(pos + pi(x, y))) return false;
		}
	}
	return true;
}

// For things bigger than 1x1
bool GameState::canMoveDir(pi pos, pi tileSize, Dir d) {
	if (d == D_NONE) return true;
	int x, y;
	switch (d) {
	case D_RIGHT:
		//cout << "Tile size = " << tileSize << endl;
		//cout << "Moving right from " << pos << endl;
		x = pos.x + tileSize.x;
		for (y = pos.y; y < pos.y + tileSize.y; y++) {
			if (!canMove(pi(x, y))) {
				//cout << "Cant move to " << pi(x, y) << endl;
				return false;
			}
		}
		break;
	case D_LEFT:
		x = pos.x - 1;
		for (y = pos.y; y < pos.y + tileSize.y; y++) {
			if (!canMove(pi(x, y))) return false;
		}
		break;
	case D_DOWN:
		y = pos.y + tileSize.y;
		for (x = pos.x; x < pos.x + tileSize.x; x++) {
			if (!canMove(pi(x, y))) return false;
		}
		break;
	case D_UP:
		y = pos.y - 1;
		for (x = pos.x; x < pos.x + tileSize.x; x++) {
			if (!canMove(pi(x, y))) return false;
		}
		break;
	}
	return true;
}

// For things bigger than 1x1
bool GameState::canMoveDirWorld(pi pos, pi tileSize, Dir d) {
	if (d == D_NONE) return true;
	int x, y;
	switch (d) {
	case D_RIGHT:
		//cout << "Tile size = " << tileSize << endl;
		//cout << "Moving right from " << pos << endl;
		x = pos.x + tileSize.x;
		for (y = pos.y; y < pos.y + tileSize.y; y++) {
			if (!currentWorld->canWalk(pi(x, y))) return false;
			
		}
		break;
	case D_LEFT:
		x = pos.x - 1;
		for (y = pos.y; y < pos.y + tileSize.y; y++) {
			if (!currentWorld->canWalk(pi(x, y))) return false;
		}
		break;
	case D_DOWN:
		y = pos.y + tileSize.y;
		for (x = pos.x; x < pos.x + tileSize.x; x++) {
			if (!currentWorld->canWalk(pi(x, y))) return false;
		}
		break;
	case D_UP:
		y = pos.y - 1;
		for (x = pos.x; x < pos.x + tileSize.x; x++) {
			if (!currentWorld->canWalk(pi(x, y))) return false;
		}
		break;
	}
	return true;
}


void GameState::addEntity(Entity* entity) {
	if (entity->type == ET_PLAYER) {
		Player *p = (Player*)entity;
		idToPlayer[p->ID] = p;
	}
	if (entity->type == ET_ENEMY) {
		Enemy *e = (Enemy*)entity;
		idToEnemy[e->ID] = e;
	}

	entities.push_back(entity);
}

void GameState::addPermTopLayerTile(Entity* entity) {
	if (currentWorld == nullptr || !currentWorld->initialised) return;
	currentWorld->addPermTopLayerTile(entity);

	//pi p = entity->getPos();
	//p += currentWorld->origin;

	//if (currentWorld->topLayerTileArray[p.x][p.y] == nullptr) {
	//	currentWorld->topLayerTileArray[p.x][p.y] = entity;
	//	currentWorld->TLset1.insert(p - currentWorld->origin);
	//}
	//else {
	//	cout << "Failed to place permanent TL tile : Place was taken" << endl;
	//	delete entity;
	//}
}

bool GameState::addTempTopLayerTile(Entity* entity, bool sendPacket) {
	pi pos = entity->getPos();
	if (currentWorld == nullptr || !currentWorld->initialised) return false;

#ifndef CLIENT
	// Client places regardless
	if (!currentWorld->canWalk(pos)) return false;
#endif
	if (currentWorld->addTempTopLayerTile(entity)) {
#ifndef CLIENT
		if (sendPacket) {
			TopLayerTile *tl = (TopLayerTile*)entity;

			sf::Packet p;
			p.append(&P_TL_TILE, sizeof(Packet));

			p.append(&pos.x, sizeof(pos_t));
			p.append(&pos.y, sizeof(pos_t));

			p.append(&tl->tileType, sizeof(TLTileType));

			cout << "Sending TL tile to all players" << endl;
			sendPacketToAllPlayers(p);
		}
#endif
		return true;
	}

	return false;

	//pi p = entity->getPos();
	//p += currentWorld->origin;

	//if (currentWorld->topLayerTileArray[p.x][p.y] == nullptr) {
	//	currentWorld->topLayerTileArray[p.x][p.y] = entity;
	//	currentWorld->TLset2.insert(p - currentWorld->origin);
	//}
	//else {
	//	cout << "Failed to place temporary TL tile : Place was taken" << endl;
	//	delete entity;
	//}
}


void GameState::removeDead() {
	// Remove top layer 
	if (currentWorld != nullptr && currentWorld->initialised) {
		for (auto it = currentWorld->TLset2.begin(); it != currentWorld->TLset2.end();) {
			pi p = *it + currentWorld->origin;
			
			Entity *e = currentWorld->topLayerTileArray[p.x][p.y];
			if (e == nullptr) {
				cout << "While removing dead, found nullptr entity in TL tiles" << endl;
			}
			if (!e->isAlive()) {
				delete e;
				currentWorld->topLayerTileArray[p.x][p.y] = nullptr;
				it = currentWorld->TLset2.erase(it);
			}
			else {
				++it;
			}
		}
	}

	//for (auto it = topLayerTiles.begin(); it != topLayerTiles.end(); ) {
	//	if (!(*it)->isAlive()) {
	//		Entity *deleteMe = *it;

	//		auto previt = it;
	//		++it;

	//		// Deallocate memory, then remove invalid pointer from the list
	//		delete deleteMe;
	//		topLayerTiles.erase(previt);
	//	}
	//	else ++it;
	//}

	// Remove entities
	for (auto it = entities.begin(); it != entities.end(); ) {
		if (!(*it)->isAlive()) {
			if ((*it)->type == ET_ENEMY) {
#ifndef CLIENT
				((Enemy*)(*it))->onDie(this); // process loot for players $$$
#endif
				ID_t ID = ((LivingEntity*)(*it))->ID;
				idToEnemy.erase(idToEnemy.find(ID));
			}
			if ((*it)->type == ET_PLAYER) {
				ID_t ID = ((LivingEntity*)(*it))->ID;

#ifdef CLIENT
				if (ID == myPlayer->ID) {
					++it;
					continue;
				}
				cout << "Player id " << ID << " is dead" << endl;
				Player *p = (Player*)(*it);

				// TODO: BETTER SYSTEM FOR CHECKING DEATH VS DISCONNECT VS ENTER DUNGEON
				// (we will need a P_PLAYER_DIED packet to replace P_YOU_DIED)
				if (currentWorld->dungeonEntrances.find(p->getCollisionPos()) == currentWorld->dungeonEntrances.end())
					gameMenu.chat.displaySystemMessage(p->name + " was killed", sf::Color::Yellow);

				idToPlayer.erase(idToPlayer.find(ID));
#else
				Player *p = (Player*)(*it);
				if (!p->spawned) {
					++it;
					continue;
				}
				cout << "Player id " << ID << " is dead" << endl;

				sf::Packet packet;
				packet.append(&P_YOU_DIED, sizeof(Packet));
				p->sendPacket(packet);

				++it;
				playerDied(p);
				continue;
#endif
			}

			Entity *deleteMe = *it;

			auto previt = it;
			++it;

			// Deallocate memory, then remove invalid pointer from the list
			delete deleteMe;
			entities.erase(previt);
		}
		else ++it;
	}
}

#ifdef CLIENT
void GameState::draw(const sf::View *v) {
	window->clear(sf::Color::White);
	tileBox.draw(window);

	// Draw TL tiles that are on screen using binary search on the set
	if (currentWorld != nullptr && currentWorld->initialised) {
		int x1 = myPlayer->getCollisionPos().x - XVIEW;
		int x2 = x1 + XVIEW * 2;

		for (auto it = currentWorld->TLset1.lower_bound(pi(x1, 0)); it != currentWorld->TLset1.end(); ++it) {
			pi pos = *it;
			if (pos.x > x2) break;
			pos += currentWorld->origin;
			currentWorld->topLayerTileArray[pos.x][pos.y]->draw(window, this);
		}

		for (auto it = currentWorld->TLset2.lower_bound(pi(x1, 0)); it != currentWorld->TLset2.end(); ++it) {
			pi pos = *it;
			if (pos.x > x2) break;
			pos += currentWorld->origin;
			currentWorld->topLayerTileArray[pos.x][pos.y]->draw(window, this);
		}
	}
	//for (auto it = topLayerTiles.begin(); it != topLayerTiles.end(); ++it) {
	//	(*it)->draw(window, this);
	//}

	// Draw entities that are onscreen
	for (auto it = entities.begin(); it != entities.end(); ++it) {
		if (isInView((pi)((*it)->getPosWorldCentered() / (float)PIXEL_PER_UNIT))) {
			(*it)->draw(window, this);
		}
	}

	lootUI.draw(window);
	playerViewUI.draw(this, window);
	gameMenu.draw(this, window);
	if (currentWorld != nullptr && currentMinimap != nullptr)
		currentMinimap->draw(this, window);
}

sf::Vector2f GameState::whereToCenter() {
	if (myPlayer != nullptr) {
		return myPlayer->getPosWorldCentered();
	}
	return sf::Vector2f(0, 0);
	//if (myPlayer == nullptr) 
	//return sf::Vector2f(myPlayer->getPos().x, 0);
	//return sf::Vector2f(0, 0);
}

Player *GameState::createNewPlayer(ID_t id, pos_t x, pos_t y) {
	Player *obj = new Player(pi(x, y), id);
	cout << "Creating new player with id " << id << endl;
	addEntity(obj);
	return obj;
}

void GameState::handleBaseStatsInv(const void *data) {
	stat_t *Stat = (stat_t*)((Packet*)data + 1);

	stat_t statList[NUM_STATS];
	for (int i = 0; i < NUM_STATS; i++) {
		statList[i] = *Stat;
		Stat++;
	}
	if (pInfo.inv.loadFromData(Stat))
		pInfo.setBaseStats(Stats(statList));
}

// ID, X, Y, SEED, WORLD_ID, XP, BASESTATS, INVENTORY
void GameState::handleHello(const void *data) {
	cout << "Handling server hello" << endl;

	ID_t* ID_T = (ID_t*)((Packet*)data+1);
	ID_t id = *ID_T;
	ID_T++;
	pos_t *Pos_t = (pos_t*)ID_T;
	pos_t x = *Pos_t;
	Pos_t++;
	pos_t y = *Pos_t;
	Pos_t++;
	seed_t *Seed = (seed_t*)Pos_t;
	seed_t overworldSeed = *Seed;
	Seed++;

	ID_T = (ID_t*)Seed;
	ID_t worldID = *ID_T;
	ID_T++;

	exp_t *Exp_t = (exp_t*)ID_T;
	pInfo.setXP(*Exp_t);
	Exp_t++;

	stat_t *Stat = (stat_t*)Exp_t;
	stat_t statList[NUM_STATS];
	for (int i = 0; i < NUM_STATS; i++) {
		statList[i] = *Stat;
		Stat++;
	}
	pInfo.setBaseStats(Stats(statList));
	pInfo.inv.loadFromData(Stat);
	cout << "Loaded xp + base stats + inventory" << endl;

	myPlayer = createNewPlayer(id, x, y);
	myPlayer->local = true;

	initialiseOverworld(overworldSeed, worldID);

	ready = true;
}


void GameState::handleUpdate(const void *data) {
	//if (isDead) return;

	player_count_t *numP = (player_count_t*)(((Packet*)data) + 1);
	player_count_t N = *numP;

	//cout << "THERE ARE " << N << " PLAYERS" << endl;

	bool removals = false;
	int t = isDead ? -1 : 0;
	// N is total, so all players and us
	if (N != idToPlayer.size() + t) {
		cout << "Player count changed: number of players = " << (int)N << endl;
		removals = true;
		for (auto it = idToPlayer.begin(); it != idToPlayer.end(); ++it) {
			it->second->exists = false;
		}
	}

	bool imInUpdate = false;

	const void *g = numP + 1;
	ID_t id;

	for (int i = 0; i < N; i++) {
		id = *(ID_t*)g;
		g = (ID_t*)g + 1;
		proc_moves_t procm = *(proc_moves_t*)g;
		if (idToPlayer.find(id) == idToPlayer.end()) {
			cout << "Creating a new player with id = " << id << endl;
			createNewPlayer(id, 0, 0);
		}
		if (id == myPlayer->getID()) {
			if (procm != serverProc) lastDownTime = totalTime;
			serverProc = procm;

			imInUpdate = true;
		}
		g = idToPlayer[id]->handleUpdate((proc_moves_t*)g + 1);
		idToPlayer[id]->exists = true;
	}

	if (waitingToRespawn && imInUpdate) {
		pi x = myPlayer->getCollisionPos();

		cout << "Respawned pos = " << x << endl;

		waitingToRespawn = false;
		isDead = false;
		loadWorld(overworld);
		myPlayer->setPos(x);
		adjustTileBox();
	}

	// ENEMIES
	for (auto it = idToEnemy.begin(); it != idToEnemy.end(); ++it) {
		it->second->exists = false;
	}

	N = *(enemy_count_t*)g;
	g = (enemy_count_t*)g + 1;
	for (int i = 0; i < N; i++) {
		id = *(ID_t*)g;
		g = (ID_t*)g + 1;
		EntitySpecies *Spec = (EntitySpecies*)g;
		EntitySpecies spec = *Spec;
		Spec++;

		//cout << "Got an ENEMY id: " << id << endl;
		//cout << "Their species = " << spec << endl;

		if (idToEnemy.find(id) == idToEnemy.end()) {
			Entity *enemy = spawnEntityID(spec, id, pi(0, 0));
			addEntity(enemy);
		}
		g = idToEnemy[id]->handleUpdate(Spec);
		idToEnemy[id]->exists = true;
	}
}

void GameState::handleEquipmentUpdate(const void *data) {
	player_count_t *P = (player_count_t*)((Packet*)data + 1);
	player_count_t n = *P;

	void *g = P + 1;

	for (auto i = 0; i < n; i++) {
		ID_t id = *((ID_t*)g);
		g = (ID_t*)g + 1; // pointing to first item

		if (idToPlayer.find(id) == idToPlayer.end()) {
			g = (ITEM*)g + NUM_EQUIP_LOCS;
			g = (stat_t*)g + 3;
			g = (level_t*)g + 1;
		}
		else {
			Player *p = idToPlayer[id];
			ITEM *it = (ITEM*)g;
			for (auto j = 0; j < NUM_EQUIP_LOCS; j++) {
				p->equips[j] = *it;
				it++;
			}
			stat_t *S = (stat_t*)it;
			if (id == myPlayer->ID) {
				S += 2;
			}
			else {
				p->maxLife = *S; S++;
				p->maxMP = *S; S++;
			}
			p->wis = *S; S++;

			level_t *L = (level_t*)S;
			p->level = *L;
			L++;
			g = L;
		}
	}
}


void GameState::handleLoot(const void *data) {
	exp_t *Gain = (exp_t*)((Packet*)data + 1);
	pInfo.gainXP(*Gain, this);
	pos_t *Pos_t = (pos_t*)(Gain+1);
	pos_t x = *Pos_t;
	Pos_t++;
	pos_t y = *Pos_t;
	Pos_t++;
	item_num_t *Item_num = (item_num_t*)Pos_t;
	item_num_t count = *Item_num;
	Item_num++;
	ITEM *item_p = (ITEM*)Item_num;

	if (count > 0) {
		ItemRarity rarity = ItemRarity::COMMON;

		std::vector<ITEM> loot;
		for (auto i = 0; i < count; i++) {
			ITEM got = *item_p;
			item_p++;
			loot.push_back(got);
			cout << "We got a " << itemTypes[got]->name << endl;

			if (itemTypes[got]->rarity > rarity)
				rarity = itemTypes[got]->rarity;
		}
		pInfo.createLoot(pi(x, y), loot);
		LootChest *chest = new LootChest(pi(x, y));
		chest->setRarity(rarity);
		addTempTopLayerTile(chest);

		lootChanged = true;
	}
}

void GameState::handleEnterDungeon(const void *data) {
	cout << "Handling entering a dungeon" << endl;

	// Packet BiomeType Seed ID
	BiomeType *b = (BiomeType*)((Packet*)data + 1);
	BiomeType biome = *b; b++;
	seed_t *Seed = (seed_t*)b;
	seed_t seed = *Seed; Seed++;
	ID_t worldID = *((ID_t*)Seed);

	initialiseDungeon(biome, seed, worldID);
}

void GameState::deleteAllEntities() {
	if (currentWorld != nullptr) {
		while (currentWorld->TLset1.size()) {
			pi pos = *currentWorld->TLset1.begin();
			currentWorld->TLset1.erase(currentWorld->TLset1.begin());

			pos += currentWorld->origin;
			Entity *x = currentWorld->topLayerTileArray[pos.x][pos.y];
			if (x == nullptr) {
				cout << "BUG: Null entity in top layer tiles" << endl;
			}
			else {
				delete x;
			}
			currentWorld->topLayerTileArray[pos.x][pos.y] = nullptr;
		}

		while (currentWorld->TLset2.size()) {
			pi pos = *currentWorld->TLset2.begin();
			currentWorld->TLset2.erase(currentWorld->TLset2.begin());

			pos += currentWorld->origin;
			Entity *x = currentWorld->topLayerTileArray[pos.x][pos.y];
			if (x == nullptr) {
				cout << "BUG: Null entity in top layer tiles 2" << endl;
			}
			else {
				delete x;
			}
			currentWorld->topLayerTileArray[pos.x][pos.y] = nullptr;
		}
	}


	//for (auto it = topLayerTiles.begin(); it != topLayerTiles.end(); ) {
	//	Entity *deleteMe = *it;
	//	auto previt = it;
	//	++it;

	//	// Deallocate memory, then remove invalid pointer from the list
	//	delete deleteMe;
	//	topLayerTiles.erase(previt);
	//}
	//cout << "Deleted top layer tiles" << endl;

	// Remove entities
	for (auto it = entities.begin(); it != entities.end(); ) {
		if ((*it)->type == ET_ENEMY) {
			ID_t ID = ((LivingEntity*)(*it))->ID;
			idToEnemy.erase(idToEnemy.find(ID));
		}
		if ((*it)->type == ET_PLAYER) {
			ID_t ID = ((LivingEntity*)(*it))->ID;
			if (ID == myPlayer->ID) { // Don't delete myPlayer
				++it;
				continue;
			}
			idToPlayer.erase(idToPlayer.find(ID));
		}
		Entity *deleteMe = *it;
		auto previt = it;
		++it;
		delete deleteMe;
		entities.erase(previt);
	}

	//cout << "Deleted entities" << endl;
}

// TODO: Something else..?
void GameState::handlePlayerDeath(const void *data) {
	gameMenu.chat.displaySystemMessage("You died :( Press R to respawn", sf::Color::Red);
	
	myPlayer->targets.clear();
	specialTargets.clear();

	isDead = true;
	myPlayer->setLife(0);
}

void GameState::handlePlayerRespawn() {
	sf::Packet p; p.append(&P_RESPAWN, sizeof(Packet));
	connection->send(p);

	//isDead = false;
	waitingToRespawn = true;
	//loadWorld(overworld);
}


void GameState::handleMessage(const void *data) {
	//cout << "Handling a message" << endl;

	ID_t *ID_T = (ID_t*)((Packet*)data + 1);
	ID_t id = *ID_T;
	if (id != -1 && idToPlayer.find(id) == idToPlayer.end()) {
		cout << "Received a message from ID = " << id << " but no such player exists" << endl;
		return;
	}

	ID_T++;

	msg_len_t *N = (msg_len_t*)ID_T;
	msg_len_t n = *N;

	char *c = (char*)(N + 1);
	std::string st = "";

	for (int i = 0; i < n; i++) {
		char x = *c;
		st.push_back(x);
		c++;
	}

	if (id != -1) {
		std::string name = idToPlayer[id]->name;
		gameMenu.chat.displayMessage(name, st);
	}
	else {
		gameMenu.chat.displaySystemMessage(st, sf::Color::Yellow);
	}
}

void GameState::handleSpecial(const void *data) {
	ID_t *ID_T = (ID_t*)((Packet*)data + 1);
	ID_t a = *ID_T; ID_T++;
	ID_t b = *ID_T;

	if (myPlayer != nullptr && a == myPlayer->ID) {
		specialUsedTime = -1;
		return;
	}

	if (idToPlayer.find(a) == idToPlayer.end()) return;
	ITEM i = idToPlayer[a]->equips[(int)Equipment::SPECIAL];
	if (i == I_NONE) return;

	Special *s = (Special*)itemTypes[i];
	s->use(this, a, b);
}

// x, y, newTile
void GameState::handleChangeTile(const void *data) {
	pos_t *P = (pos_t*)((Packet*)data + 1);
	pos_t x = *P; P++;
	pos_t y = *P; P++;
	Tile t = *((Tile*)P);

	if (currentWorld != nullptr && currentWorld->initialised) {
		x += currentWorld->origin.x;
		y += currentWorld->origin.y;

		auto &tiles = currentWorld->tiles;
		if (x >= 0 && x < tiles.size()) {
			if (y >= 0 && y < tiles[0].size()) {
				cout << "Changing a tile" << endl;
				tiles[x][y] = t;
				tileBox.refresh(currentWorld);
			}
		}
	}
}

// x, y, TileType
void GameState::handleTlTile(const void *data) {
	pos_t *P = (pos_t*)((Packet*)data + 1);
	pos_t x = *P; P++;
	pos_t y = *P; P++;
	TLTileType t = *((TLTileType*)P);

	pi pos(x, y);

	TLTileData *d = tileTypes[static_cast<int>(t)];
	if (d->lifetime == -1) {
		addPermTopLayerTile(new TopLayerTile(pos, t));
	}
	else {
		addTempTopLayerTile(new TimedTopLayerTile(pos, t));
	}
}

// type, num, x, y
void GameState::handleTlTiles(const void *data) {
	TLTileType *T = (TLTileType*)((Packet*)data + 1);
	TLTileType t = *T;
	T++;

	TLTileData *d = tileTypes[static_cast<int>(t)];
	bool perm = d->lifetime == -1;

	int8_t count = *((int8_t*)T);

	pos_t *P = (pos_t*)(((int8_t*)T)+1);

	for (auto i = 0; i < count; i++) {
		pos_t x = *P; P++;
		pos_t y = *P; P++;
		pi pos(x, y);

		if (perm) {
			addPermTopLayerTile(new TopLayerTile(pos, t));
		}
		else {
			addTempTopLayerTile(new TimedTopLayerTile(pos, t));
		}
	}
}

// x, y, ind
void GameState::handleConfirmedItem(const void *data) {
	cout << "Item pickup confirmed" << endl;

	pos_t *P = (pos_t*)((Packet*)data + 1);
	pi pos; pos.x = *P; P++; pos.y = *P; P++;
	item_num_t ind = *((item_num_t*)P);
	pInfo.removeItem(pos, ind);
	lootChanged = true;
};



void GameState::handleMyPos(pi p) {
	lastSeenPlayerPos = p;

	if (waitingToRespawn) {
		myPlayer->setPos(p);
		myPlayer->currentMovement = D_NONE;
		return;
	}

	// Server has processed 0..proc
	// So we need to apply moves proc+1...end

	//cout << "I have sent moves up to " << (int)queueMoveNum + (int)moves.size() - 1 << endl;
	//cout << "Server has processed moves up to " << (int)serverProc << endl;

	proc_moves_t firstMove = serverProc + 1;

	int count = 0;

	//int8_t X = queueMoveNum;
	//cout << "Move queue: " << endl;
	//for (auto it = moves.begin(); it != moves.end(); ++it) {
	//	cout << "  " << (int)X << " : Direction no. " << *it << endl;
	//	X++;
	//}

	//cout << "Server claims to have processed 0.." << (int)serverProc << " = " << p << endl;

	while (queueMoveNum != firstMove && moves.size()) {
		count++;

		moves.pop_front();
		queueMoveNum++;
	}

	if (moves.size() > 2) {
		cout << "WARNING: Moves.size() = " << moves.size() << endl;
	}

	if (queueMoveNum != firstMove) {
		cout << "Server has somehow overcounted our movement commands." << endl;
		queueMoveNum = firstMove;
	}

	pi posP = p;
	for (auto it = moves.begin(); it != moves.end(); ++it) {
		Dir d = *it;
		posP += dirOffset[d];
	}

	//cout << "Handling server position; applied " << moves.size() <<
	// " moves. Server pos = " << posP << " and my pos = " << myPlayer->getPos() << endl;

	if (posP != myPlayer->getCollisionPos()) {

		cout << "JERKING PLAYER" << endl;

		cout << "Start of queue move = " << (int)queueMoveNum << endl;
		cout << "Server processed to " << (int)serverProc << endl;

		cout << "We applied " << moves.size() << " moves to " << p << " to get " << posP << endl;
		cout << "Which aint the same as " << myPlayer->getCollisionPos() << " (moving from " << myPlayer->getPos() << ")" << endl;
		cout << "and we removed " << count << " queue items" << endl << endl;
		
		// Situation: 
		//  We press up [add to queue]
		//  We press up again [add to queue]
		//  We receive input that says boo
		
		myPlayer->jerk(posP);
	}

	// TODO: Unsure about this. Not actually helping anything - just in the event of a super unlikely bug where a packet is lost entirely.
	//if (moves.size() > 0 && totalTime - lastDownTime > 3.0f) {
	//	moves.clear();
	//	queueMoveNum = serverProc;
	//}
}


#else

#include "Packet.h"
#include "Player.h"

// ID, X, Y, LIFE, MOVEMENT, MOVED,
// CombatStats, NumTargets [char], Some targets [IDs]

//            ID  X   Y  Life Dir Moved  Combat Stats  Num targets     One target
// 1 target : 2 + 2 + 2 + 4 +  1 + 4 +    {4+4+2+1}  +     1        +      2
//  = 30 bytes per player
//   + 2 per additional targeted ID. 

void GameState::getUpdatePacket(sf::Packet &packet) {
	packet.append(&P_UPDATE, sizeof(Packet));

	std::vector<Entity*> players, enemies;

	for (Entity *entity : entities) {
		if (entity->type == ET_PLAYER) {
			if (((Player*)entity)->spawned)
				players.push_back(entity);
		}
		if (entity->type == ET_ENEMY) enemies.push_back(entity);
	}

	player_count_t Pnum = players.size();
	packet.append(&Pnum, sizeof(player_count_t));

	Player *p;
	for (Entity *e : players) {
		p = (Player*)e;

		p->calculateCombatStats(); // Update combat stats here, as opposed to in update, as this runs less frequently

		// TODO: MP + processedMoves only necessary for local player
		//  Could save a total of 4+1 bytes per update per player
		//  = 50 bytes per player less per sec (thats not much I dont think?)
		//  [less than that as new packet means new Packet header]
		pos_t x = p->getCollisionPos().x,
			y = p->getCollisionPos().y;
		packet.append(&p->ID, sizeof(ID_t));
		packet.append(&p->processedMoves, sizeof(proc_moves_t));
		packet.append(&x, sizeof(pos_t));
		packet.append(&y, sizeof(pos_t));
		packet.append(&p->getLife(), sizeof(life_t));
		packet.append(&p->getMP(), sizeof(mp_t));
		//packet.append(&p->currentMovement, sizeof(Dir));
		//packet.append(&p->moved, sizeof(float));
		packet.append(&p->combatStats, sizeof(CombatStats));
		target_count_t num = (target_count_t)p->targets.size();
		packet.append(&num, sizeof(target_count_t));
		for (auto i = 0; i < num; i++) 
			packet.append(&p->targets[i], sizeof(ID_t));
	}

// I load in.
// Boss targets me.
// I realise 0.1s later.
// Boss shoots me 1s after I join.
// I show the shot 0.1s after its actually shot.
// I show myself taking damage at time 1.15 seconds (takes 0.05s to hit)

	enemy_count_t Enum = enemies.size();
	packet.append(&Enum, sizeof(enemy_count_t));

	LivingEntity *e;
	for (Entity *o : enemies) {
		e = (LivingEntity*)o;
		pos_t x = e->getCollisionPos().x,
			y = e->getCollisionPos().y;
		packet.append(&e->ID, sizeof(ID_t));
		packet.append(&e->spec, sizeof(EntitySpecies));
		packet.append(&x, sizeof(pos_t));
		packet.append(&y, sizeof(pos_t));
		packet.append(&e->getLife(), sizeof(life_t));

		// Only add CombatStats if it can change for this enemy
		if (!speciesCombatStatsConstant[(int)e->spec])
			packet.append(&e->combatStats, sizeof(CombatStats));
		
		target_count_t num = (target_count_t)e->targets.size();
		packet.append(&num, sizeof(target_count_t));
		for (auto i = 0; i < num; i++)
			packet.append(&e->targets[i], sizeof(ID_t));
	}

	//cout << "Update packet is " << packet.getDataSize() << " bytes" << endl;
}

void GameState::specialUsed(Special *s, ID_t a, ID_t b, sf::Packet &packet) {
	LivingEntity *other;
	if (b < 0) {
		if (idToEnemy.find(b) == idToEnemy.end()) return;
		other = idToEnemy[b];
	}
	else {
		if (idToPlayer.find(b) == idToPlayer.end()) return;
		other = idToPlayer[b];
	}
	if ((b >= 0 && s->targetsPlayers) || (b < 0 && s->targetsEnemies)) {
		Player *p = idToPlayer[a];

		if (p->mpOn >= s->mpCost) {
			p->mpOn -= s->mpCost;

			pi pos = p->getCollisionPos();

			if (util::taxicabSize(pos, other->getCollisionPos(), other->tileSize) <= s->range) {
				cout << "Player ID " << a << " used their special successfully" << endl;
				s->use(this, a, b);

				// Send special_used packet to all players
				for (auto it = idToPlayer.begin(); it != idToPlayer.end(); ++it) {

					// If they are close to the user
					if (util::taxicab(it->second->getCollisionPos(), pos) < 30) {
						it->second->sendPacket(packet);
					}
				}

			}
		}
	}
}

std::vector<pi> GameState::sendTLtilesPacket(TLTileType type, const std::vector<pi> poses) {
	bool timed = tileTypes[(int)type]->lifetime != -1;
	if (timed == false) {
		cout << "SENDING NON-TIMED TL TILES IS CURRENTLY NOT SUPPORTED" << endl;
		return std::vector<pi>();
	}

	std::vector<pi> points;

	for (const pi &p : poses) {
		if (timed) {
			if (addTempTopLayerTile(new TimedTopLayerTile(p, type), false)) {
				points.push_back(p);
			}
		}
		else {
			// TODO: Only if we ever acc want this
		}
	}

	if (points.size() > 0) {
		sf::Packet p;
		p.append(&P_TL_TILES, sizeof(Packet));
		p.append(&type, sizeof(TLTileType));
		int8_t n = points.size();
		p.append(&n, sizeof(int8_t));

		for (auto i = 0; i < n; i++) {
			p.append(&points[i].x, sizeof(pos_t));
			p.append(&points[i].y, sizeof(pos_t));
		}

		sendPacketToAllPlayers(p);
	}
	return points;
}


void GameState::getEquipmentPacket(sf::Packet &packet) {
	packet.append(&P_PLAYER_EQUIPS, sizeof(Packet));
	player_count_t num = idToPlayer.size();
	packet.append(&num, sizeof(player_count_t));

	for (auto it = idToPlayer.begin(); it != idToPlayer.end(); ++it) {
		ID_t id = it->first;
		packet.append(&id, sizeof(ID_t));

		const Inventory &inv = it->second->pInfo.inv;

		for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
			ITEM item = inv.equips[i];
			packet.append(&item, sizeof(ITEM));
		}
		
		// Bit messy. Should really be a part of CombatStats, however, no enemy needs it.
		// A bunch of stats that the others need to know about.
		// Note: all sent as stat_t, as life_t > stat_t [huge life_t is for enemies only]

		Stats st = it->second->pInfo.getOverallStats();

		packet.append(&st.stats[(int)Stat::HP], sizeof(stat_t));
		packet.append(&st.stats[(int)Stat::MP], sizeof(stat_t));
		packet.append(&st.stats[(int)Stat::WIS], sizeof(stat_t));

		// Level for display
		level_t lev = it->second->pInfo.getPlayerLevel();
		packet.append(&lev, sizeof(level_t));
	}
}

void GameState::sendPacketToAllPlayers(sf::Packet &packet) {
	for (auto it = idToPlayer.begin(); it != idToPlayer.end(); ++it) {
		it->second->sendPacket(packet);
	}
}

Player *GameState::getClosestPlayer(pi pos) {
	int minDist = 1000000;
	Player *x = nullptr;
	for (auto it = idToPlayer.begin(); it != idToPlayer.end(); ++it) {
		Player *p = (Player*)it->second;
		if (p != nullptr && p->isAlive() && p->spawned) {
			int d = util::taxicab(p->getCollisionPos(), pos);
			if (d < minDist) {
				minDist = d;
				x = p;
			}
		}
	}
	return x;
}

#include <queue>

//class PlayerDistComp {
//public:
//	PlayerDistComp(pi pos_) : pos(pos_) {
//	}
//	bool operator()(const Player *a, const Player *b) const {
//		return util::taxicab(a->getCollisionPos(), pos) < util::taxicab(b->getCollisionPos(), pos);
//	}
//private:
//	pi pos;
//};

typedef std::pair<int, Player*> playerDist;

std::vector<Player*> GameState::getClosestPlayers(pi pos, int k) {
	// We want the furthest away to come first, so we may pop them
	std::priority_queue<playerDist, std::vector<playerDist>, std::less<playerDist> > Q;

	for (auto it = idToPlayer.begin(); it != idToPlayer.end(); ++it) {
		Player *p = (Player*)it->second;
		if (p != nullptr && p->isAlive() && p->spawned) {
			int d = util::taxicab(p->getCollisionPos(), pos);

			// Don't add if the queue's full and we'd be furthest
			if (Q.size() < k || d < Q.top().first) {
				Q.push(playerDist(d, p));
				if (Q.size() > k) Q.pop();
			}
		}
	}

	std::vector<Player*> V;
	while (Q.size()) {
		V.push_back(Q.top().second);
		Q.pop();
	}
	return V;
}

int16_t bfsNum = 0;
Dir GameState::BFSTowards(const pi a, const pi b, const range_t depth, const pi tileSize) {
	if (a == b || util::taxicab(a,b)>depth) return D_NONE;
	//cout << "BFS" << endl;

	// Search outwards from A
	// To check if visited we use world.tileMarker

	// Search outwards from A
	// Each tile has a marker saying where it came from.

	// 1) We reach b
	//    Follow path back to the start
	//    Use the last direction in there
	// 2) We don't reach b, return D_NONE

	if ((int16_t)(bfsNum + 1) < bfsNum) {
		cout << "Bfs wrapping" << endl;
	}

	bfsNum++;

	std::queue<pi> Q;
	Q.push(a);
	tileMarkerNum[a.x + currentWorld->origin.x][a.y + currentWorld->origin.y] = bfsNum;

	bool oneTile = (tileSize == pi(1, 1));

	pi end = b;

	//cout << "Search begins" << endl;
	bool w = false;
	while (Q.size()) {
		pi p = Q.front();
		Q.pop();

		//cout << "Considering point " << p << endl;

		for (int i = 0; i < 4; i++) {
			pi p2 = p + dirOffset[i];
			if (tileMarkerNum[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] != bfsNum) {
				bool tileFree;

				if (oneTile) {
					tileFree = canMove(p2);
				}
				else {
					tileFree = canMoveDir(p, tileSize, (Dir)i);
				}

				bool pOnB = ((b.x - p2.x < tileSize.x && b.x >= p2.x) && (b.y - p2.y < tileSize.y && b.y >= p2.y));

				if ((pOnB || tileFree) && (util::taxicab(a, p2) <= depth)) {
					tileMarkerNum[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] = bfsNum;
					tileMarkerDir[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] = (Dir)((i + 2) % 4); // reverse direction

					if (pOnB) {
						end = p2;
						w = true;
						break;
					}

					Q.push(p2);
				}
			}
		}
		if (w) break;
	}

	if (!w) return D_NONE;

	Dir d = D_NONE;
	pi t = end;
	
	while (t != a) {
		d = tileMarkerDir[t.x + currentWorld->origin.x][t.y + currentWorld->origin.y];
		t += dirOffset[(int)d];
	}
	if (d == D_NONE) {
		cout << "HUH?" << endl << endl;
		return d;
	}
	// reverse the direction again, giving the direction towards b from a
	return (Dir)(((int)d + 2) % 4);
}

Dir GameState::BFSTowardsRanged(const pi a, const pi b, const range_t depth, const range_t range, const pi tileSize) {
	if (a == b || util::taxicabSize(a, b, tileSize) > depth || util::taxicabSize(a, b, tileSize) <= range) return D_NONE;

	bfsNum++;

	std::queue<pi> Q;
	Q.push(a);
	tileMarkerNum[a.x + currentWorld->origin.x][a.y + currentWorld->origin.y] = bfsNum;

	bool oneTile = (tileSize == pi(1, 1));

	pi end = b;

	//cout << "Search begins" << endl;
	bool w = false;
	while (Q.size()) {
		pi p = Q.front();
		Q.pop();

		//cout << "Considering point " << p << endl;

		for (int i = 0; i < 4; i++) {
			pi p2 = p + dirOffset[i];
			if (tileMarkerNum[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] != bfsNum) {
				bool tileFree;

				if (oneTile) {
					tileFree = canMove(p2);
				}
				else {
					tileFree = canMoveDir(p, tileSize, (Dir)i);
				}

				bool pOnB = util::taxicabSize(p2, b, tileSize) <= range;
				//bool pOnB = ((b.x - p2.x < tileSize.x && b.x >= p2.x) && (b.y - p2.y < tileSize.y && b.y >= p2.y));

				if ((pOnB || tileFree) && (util::taxicabSize(a, p2, tileSize) <= depth)) {
					tileMarkerNum[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] = bfsNum;
					tileMarkerDir[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] = (Dir)((i + 2) % 4); // reverse direction

					if (pOnB) {
						end = p2;
						w = true;
						break;
					}

					Q.push(p2);
				}
			}
		}
		if (w) break;
	}

	if (!w) return D_NONE;

	Dir d = D_NONE;
	pi t = end;

	while (t != a) {
		d = tileMarkerDir[t.x + currentWorld->origin.x][t.y + currentWorld->origin.y];
		t += dirOffset[(int)d];
	}
	if (d == D_NONE) {
		cout << "HUH? (something went wrong)" << endl << endl;
		return d;
	}
	// reverse the direction again, giving the direction towards b from a
	return (Dir)(((int)d + 2) % 4);
}

Dir GameState::BFSTowardsNoEntities(const pi a, const pi b, const range_t depth, const pi tileSize) {
	if (a == b || util::taxicab(a, b) > depth) return D_NONE;

	// Search outwards from A
	// To check if visited we use world.tileMarker

	// Search outwards from A
	// Each tile has a marker saying where it came from.

	// 1) We reach b
	//    Follow path back to the start
	//    Use the last direction in there
	// 2) We don't reach b, return D_NONE

	bfsNum++;

	std::queue<pi> Q;
	Q.push(a);
	tileMarkerNum[a.x + currentWorld->origin.x][a.y + currentWorld->origin.y] = bfsNum;

	bool oneTile = (tileSize == pi(1, 1));

	pi end = b;

	//cout << "Search begins" << endl;
	bool w = false;
	while (Q.size()) {
		pi p = Q.front();
		Q.pop();

		//cout << "Considering point " << p << endl;

		for (int i = 0; i < 4; i++) {
			pi p2 = p + dirOffset[i];
			if (tileMarkerNum[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] != bfsNum) {
				bool tileFree;

				if (oneTile) {
					tileFree = currentWorld->canWalk(p2);
				}
				else {
					tileFree = canMoveDirWorld(p, tileSize, (Dir)i);
				}

				bool pOnB = ((b.x - p2.x < tileSize.x && b.x >= p2.x) && (b.y - p2.y < tileSize.y && b.y >= p2.y));

				if ((pOnB || tileFree) && (util::taxicab(a, p2) <= depth)) {
					tileMarkerNum[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] = bfsNum;
					tileMarkerDir[p2.x + currentWorld->origin.x][p2.y + currentWorld->origin.y] = (Dir)((i + 2) % 4); // reverse direction

					if (pOnB) {
						end = p2;
						w = true;
						break;
					}

					Q.push(p2);
				}
			}
		}
		if (w) break;
	}

	if (!w) return D_NONE;

	Dir d = D_NONE;
	pi t = end;

	while (t != a) {
		d = tileMarkerDir[t.x + currentWorld->origin.x][t.y + currentWorld->origin.y];
		t += dirOffset[(int)d];
	}
	if (d == D_NONE) {
		cout << "HUH?" << endl << endl;
		return d;
	}
	// reverse the direction again, giving the direction towards b from a
	return (Dir)(((int)d + 2) % 4);
}

void GameState::disconnectPlayer(ID_t id) {
	idToPlayer.erase(id);
	for (auto it = entities.begin(); it != entities.end(); ++it) {
		if ((*it)->type == ET_PLAYER) {
			Player *p = (Player*)(*it);
			if (p->ID == id) {
				p->socket->disconnect();
				delete p->socket;
				delete p;
				entities.erase(it);
				return;
			}
		}
	}
	cout << "(couldnt find player to be disconnected in the entity list)" << endl;
}

// Same as above but don't deallocate the pointer or socket.
void GameState::playerMoved(ID_t id) {
	idToPlayer.erase(id);
	for (auto it = entities.begin(); it != entities.end(); ++it) {
		if ((*it)->type == ET_PLAYER) {
			Player *p = (Player*)(*it);
			if (p->ID == id) {
				entities.erase(it);
				return;
			}
		}
	}
	cout << "(couldnt find player who moved in the entity list)" << endl;
}

void GameState::playerDied(Player *p) {
	p->spawned = false;
	p->targets.clear();
}

void GameState::playerRespawned(Player *p) {
	if (p->spawned) return;
	GameState *state = getMainState();
	if (state != this) {
		cout << "Moving player to another state" << endl;
		state->addEntity(p);
		p->setPos(state->currentWorld->getSpawnPosition());
		playerMoved(p->getID());
	}
	else {
		p->setPos(state->currentWorld->getSpawnPosition());
	}
	p->fullHeal();
	p->spawned = true;
}

#endif