#include "pch.h"
#include "Spawner.h"

#ifndef CLIENT
#include "Player.h"
#include "Packet.h"

// Spawner

Spawner::Spawner(int maxEnemies_, float spawnCool_)
	: maxEnemies(maxEnemies_), spawnCool(spawnCool_)
{
	enemyCounter = std::shared_ptr<int>(new int);
	*enemyCounter = 0;
	//enemyCount = 0;
	spawnT = 0.0f;

	firstUpdate = true;
}

void Spawner::update(GameState *state) {
	if (firstUpdate) {
		for (int i = 0; i < maxEnemies; i++) {
			spawnEnemy(state);
		}
		firstUpdate = false;
	}

	if ((*enemyCounter) < maxEnemies) {
		spawnT -= deltaTime;
		if (spawnT < 0) {
			spawnT += spawnCool;

			spawnEnemy(state);
		}
	}
	else spawnT = spawnCool;
}

// OverworldSpawner

const float overworldnemyDensity = 0.015f;
float overworldRespawnRate = 35.0f;
OverworldSpawner::OverworldSpawner(BiomeType biome_, const std::vector<pi> &spawnPoints_, pi end_, float maxDist_)
	: Spawner((int)(overworldnemyDensity*spawnPoints_.size()), overworldRespawnRate),
	biome(biome_), spawnPoints(spawnPoints_), maxDist(maxDist_), end(end_)
{
	const auto &vec = biomes[(int)biome].spawns;
	total = 0;
	for (const auto &p : vec) {
		total += p.second;
	}
}

#define NUM_RETRIES 50

void OverworldSpawner::spawnEnemy(GameState *state) {
	const auto &vec = biomes[(int)biome].spawns;
	if (vec.size() == 0) return;

	//float low = diff * 2.0f / 3.0f;
	//float up = low + 1.0f / 3.0f;

	//int L = util::randomRound(low * vec.size());
	//int H = util::randomRound(up * vec.size());

	//// Sanity check
	//if (L < 0) L = 0;
	//if (L >= vec.size()) L = vec.size() - 1;
	//if (H < 0) H = 0;
	//if (H >= vec.size()) H = vec.size() - 1;
	//if (H < L) H = L;

	//EntitySpecies spec = randomEntityFromVector(vec, L, H);

	//Entity *e = spawnEntityID(spec, 0, pi(0, 0));

	// Try random a number of times, else reset the cooldown

	bool w = false;
	pi pos;
	int ind;

	for (int i = 0; i < NUM_RETRIES; i++) {
		ind = util::randint(0, spawnPoints.size() - 1);
		pos = spawnPoints[ind];
		if (state->canMove(pos, pi(3, 3))) {
			w = true;
			break;
		}
	}

	if (w) {
		float d = 1 - util::dist((pf)pos, (pf)end) / maxDist;

		//cout << "Percentage through = " << d << endl;

		float val = total * d * util::rand(0.8f, 1.2f);
		if (val > total) val = total;

		EntitySpecies spec = vec[vec.size() - 1].first;
		int ii = 0;
		for (const spawnPair &p : vec) {
			val -= p.second;
			if (val <= 0) {
				spec = p.first;
				break;
			}
			ii++;
		}

		//cout << "Gave us index = " << ii << " / " << vec.size() << endl;

		Entity *e = spawnEntityID(spec, 0, pos);

		(*enemyCounter)++;

		e->setPos(pos);
		((Enemy*)e)->spawnP = pos;
		((Enemy*)e)->decrementVar = enemyCounter;
		state->addEntity(e);
	}
	else {
		cout << "Overworld failed to place an enemy" << endl;
	}
}

// DungeonSpawner

DungeonSpawner::DungeonSpawner(const spawnVector *spawns_, pi BL_, pi TR_, int maxEnemies_, float respawnRate)
	: Spawner(maxEnemies_, respawnRate), spawns(spawns_), BL(BL_), TR(TR_)
{

}

void DungeonSpawner::spawnEnemy(GameState *state) {
	if (spawns->size() == 0) return;

	EntitySpecies spec = randomEntityFromVector(*spawns);
	Entity *e = spawnEntityID(spec, 0, pi(0, 0));

	bool w = false;
	pi pos;

	for (int i = 0; i < NUM_RETRIES; i++) {
		pos = pi(util::randint(BL.x, TR.x), util::randint(BL.y, TR.y));
		if (state->canMove(pos, e->tileSize)) {
			w = true;
			break;
		}
	}

	if (w) {
		e->setPos(pos);
		((Enemy*)e)->spawnP = pos;
		state->addEntity(e);
		((Enemy*)e)->decrementVar = enemyCounter;

		(*enemyCounter)++;
	}
	else {
		cout << "Dungeon failed to place an enemy" << endl;
		delete e;
	}
}


// OneTimeSpawner

BossSpawner::BossSpawner(EntitySpecies spawn, pi pos)
	: Spawner(1, 100.0f), spec(spawn), spawnPos(pos)
{
}

#define BOSS_SPAWN_TIME 33

void BossSpawner::update(GameState *state) {
	if (bossSpawnTimerActive) {
		bossSpawnTimer -= deltaTime;
		if (bossSpawnTimer <= 3 && !wallSpawned) {
			setWall(state, true);
			wallSpawned = true;
		}

		if (bossSpawnTimer <= 0) {
			cout << "Timer hit 0" << endl;

			spawnEnemy(state);
			bossIsAlive = true;
			bossSpawnTimerActive = false;

		}
	}

	if (checkingForPlayers) {
		if (checkTimer < 0) {

			if (playerInBossRoom(state)) {
				beginTimer(state);
			}

			checkTimer = 1.0f;
		}
		else checkTimer -= deltaTime;
	}

	if (bossIsAlive) {
		if ((*enemyCounter) == 0) {
			cout << "Boss is dead" << endl;

			bossIsAlive = false;
			setWall(state, false);

			state->currentWorld->bossDefeated = true;

			// TODO: Spawn other dungeons
		}
		else {
			if (checkTimer < 0) {
				cout << "Checking if players are still in the room" << endl;
				if (!playerInBossRoom(state)) {
					cout << "No players in boss room" << endl;
					reset(state);
				}
				checkTimer = 5.0f;
			}
			else checkTimer -= deltaTime;
		}
	}
}

void BossSpawner::spawnEnemy(GameState *state) {
	Entity *e = spawnEntityID(spec, 0, spawnPos);
	state->addEntity(e);
	(*enemyCounter)++;
	((Enemy*)e)->isBoss = true;
	((Enemy*)e)->decrementVar = enemyCounter;
	bossID = ((Enemy*)e)->ID;

	spawned = true;
}

void BossSpawner::beginTimer(GameState *state) {
	cout << "The timer begins" << endl;

	checkingForPlayers = false;
	bossSpawnTimerActive = true;
	bossSpawnTimer = BOSS_SPAWN_TIME;

	// TODO: Notify players
	sf::Packet p;
	std::string msg = "Boss will spawn in 30 seconds";
	p.append(&P_MSG, sizeof(Packet)); ID_t x = -1;
	p.append(&x, sizeof(ID_t));
	msg_len_t len = msg.size();
	p.append(&len, sizeof(msg_len_t));
	for (char c : msg) p.append(&c, sizeof(char));
	state->sendPacketToAllPlayers(p);
}

bool BossSpawner::playerInBossRoom(GameState *state) {
	for (auto it = state->idToPlayer.begin(); it != state->idToPlayer.end(); ++it) {
		Player *p = it->second;
		if (!p->spawned) continue;
		pi pos = p->getCollisionPos();
		if (state->currentWorld->isBossRoom(pos)) {
			return true;
		}
	}
	return false;
}

void BossSpawner::reset(GameState *state) {
	cout << "Reseting" << endl;

	if (state->idToEnemy.find(bossID) != state->idToEnemy.end()) {
		Enemy *b = state->idToEnemy[bossID];
		b->dropsLoot = false;
		b->setLife(-1);
	}

	bossID = 0;
	bossIsAlive = false;
	checkingForPlayers = true;
	bossSpawnTimerActive = false;
	spawned = false;
	wallSpawned = false;

	setWall(state, false);
}


void BossSpawner::setWall(GameState *state, bool t) {
	pi o = state->currentWorld->origin;
	Tile tile = (t ? wallTile : normalTile);

	cout << "About to set " << wallPos.size() << " walls" << endl;
	cout << "bool = " << t << endl;

	for (pi i : wallPos) {
		cout << "Sending packet for wall at " << i << endl;
		sf::Packet packet;
		packet.append(&P_CHANGE_TILE, sizeof(Packet));
		packet.append(&i.x, sizeof(pos_t));
		packet.append(&i.y, sizeof(pos_t));
		packet.append(&tile, sizeof(Tile));

		state->sendPacketToAllPlayers(packet);
		state->currentWorld->tiles[i.x + o.x][i.y + o.y] = tile;
	}
}

#endif