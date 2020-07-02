#include "pch.h"
#include "World.h"

#include <iostream>
#include <stack>

#include "TopLayerTile.h"

#include "Spawner.h"
#include "OverworldGenerator.h"
#include "DungeonGenerator.h"

#include "DesertDungeonGenerator.h"
#include "GrasslandsDungeonGenerator.h"

#ifndef CLIENT
static ID_t idd = 0;

World::World() {
	worldID = idd++;
	if (idd < 0) idd = 0; //underflow
}

World::~World() {
	for (Spawner *sp : spawners) delete sp;
}

void World::update(GameState *state) {
	for (Spawner *sp : spawners) {
		sp->update(state);
	}
}

pi World::getSpawnPosition() const {
	if (isOverworld) {
		return pi(util::randint(-5, 5), util::randint(-5, 5));
	}
	return pi(util::randint(-3, 3), util::randint(-3, 3));
}

#endif


// Each biome has one dungeon
// Each dungeon has a number of bosses

// Description of the end goal:
//  Spawn in middle. Same biome every time.
//  We

// Generate a GRAPH.
// It is a tree stemming out from spawn.
// Every little tree tree has a fixed number of branches
// And it has a list of LEFT and RIGHT nodes
// 


void World::seedRandom() {
	gen = std::mt19937(seed);
	dist = std::uniform_real_distribution<float>(0, 1);
}

float World::rand(float a, float b)
{
	return a + (b - a)*dist(gen);
}

int World::randint(int a, int b) {
	if (b < 0) return -randint(-b, -a);
	return (int)rand((float)a, (float)b + 0.999999f);
}


void World::generateMap(seed_t seed_) {
	seed = seed_;
	seedRandom();

	isOverworld = true;

	OverworldGenerator gen(this);
	gen.generate();

	//makeDungeon(pi(1, 0), BiomeType::DESERT);
	//makeDungeon(pi(-1, 0), BiomeType::GRASS);

	initialised = true;
}

void World::generateDungeon(seed_t seed_, BiomeType biome) {
	seed = seed_;
	seedRandom();

	DungeonGenerator *gen;

	switch (biome) {
	case BiomeType::DESERT:
		gen = new DesertDungeonGenerator(this);
		break;
	case BiomeType::GRASS:
		gen = new GrasslandsDungeonGenerator(this);
		break;
	default:
		cout << "No known generator for biome = " << (int)biome << endl;
		gen = new DungeonGenerator(this, biome);
	}

	// Re-generate until it is valid
	while (!gen->generate()) {
		cout << "Map generation failed: retrying" << endl;
		seed++;
		seedRandom();
	}
	delete gen;

	initialised = true;
}


void World::fillWith(pi start, Tile t, const std::set<pi, piComp> &border) {
	std::stack<pi> P;
	P.push(start);
	tiles[start.x][start.y] = t;

	while (P.size()) {
		pi pos = P.top();
		P.pop();

		//cout << pos << endl;

		for (auto i = 0; i < 4; i++) {
			pi g = pos + dirOffset[i];
			if (g.x >= 0 && g.x < tiles.size()) {
				if (g.y >= 0 && g.y < tiles[0].size()) {
					if (tiles[g.x][g.y] != t) {
						if (border.find(g-origin) == border.end()) {
							tiles[g.x][g.y] = t;
							P.push(g);
						}
					}
				}
			}
		}
	}
}


template <typename T>
void World::shuffle(std::vector<T> &vec) {
	int N = vec.size();

	std::vector<T> vec2 = vec;
	std::vector<int> indices = std::vector<int>(N);
	for (int i = 0; i < N; i++) {
		indices[i] = i;
	}

	for (int i = 0; i < N; i++) {
		int j = randint(0, indices.size() - 1);
		int pickedInd = indices[j];
		vec[pickedInd] = vec2[i];
		indices.erase(indices.begin() + j);
	}
}

template void World::shuffle<int>(std::vector<int> &vec);
template void World::shuffle<pi>(std::vector<pi> &vec);
template void World::shuffle<std::pair<int, bool>>(std::vector<std::pair<int, bool>> &vec);

Tile World::getTile(pi pos) const {
	pi index = pos + origin;
	if (index.x >= tiles.size()) return Tile::NONE;
	if (index.y >= tiles[0].size()) return Tile::NONE;
	return tiles[index.x][index.y];
}

bool World::canWalk(pi pos) const {
	Tile t = getTile(pos);
	if (t == Tile::NONE) return false;
	return tileWalkable[static_cast<int>(t)];
}

bool World::isBossRoom(pi pos) const {
	if (pos.x >= bossRoomBL.x && pos.x <= bossRoomTR.x)
		if (pos.y >= bossRoomBL.y && pos.y <= bossRoomTR.y)
			return true;
	return false;
}

void World::makeDungeon(pi pos, BiomeType biomeType) {
	DungeonEntrance d = DungeonEntrance(biomeType, pos);
	dungeonEntrances[pos] = d;
}


#include "Entity.h"

bool World::addPermTopLayerTile(Entity* entity) {
	pi p = entity->getPos();
	p += origin;

	if (topLayerTileArray[p.x][p.y] == nullptr) {
		topLayerTileArray[p.x][p.y] = entity;
		TLset1.insert(p - origin);
		return true;
	}
	else {
		cout << "Failed to place permanent TL tile : Place was taken" << endl;
		delete entity;
		return false;
	}
}

bool World::addTempTopLayerTile(Entity* entity) {
	pi p = entity->getPos();
	p += origin;

	// Either 1) tile is free 2) tile not free but we are more important 3) tile not free but we're not a ET_TOP_LAYER_TILE
	bool w = false;
	if (topLayerTileArray[p.x][p.y] == nullptr) {
		w = true;
	}
	else {
		if (entity->type == ET_TOP_LAYER_TILE) {
			TopLayerTile *tl1 = (TopLayerTile*)entity;

			Entity *e = topLayerTileArray[p.x][p.y];

			if (e->type == ET_TOP_LAYER_TILE) {
				TopLayerTile *tl2 = (TopLayerTile*)e;

				if (tl1->getData()->priority > tl2->getData()->priority) {
					w = true;
					delete topLayerTileArray[p.x][p.y];
				}
			}
		}
		else {
			// Wtf better not cancel it
			// (client side - could be loot chest)
			w = true;
		}
	}

	if (w) {
		topLayerTileArray[p.x][p.y] = entity;
		TLset2.insert(p - origin);
	}
	else {
		//cout << "Failed to place temporary TL tile : Place was taken" << endl;
		delete entity;
	}
	return w;
}