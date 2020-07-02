#include "pch.h"
#include "DesertDungeonGenerator.h"
#include "World.h"
#include "Spawner.h"
#include "TopLayerTile.h"

const static spawnVector normalSpawns = { spawnPair(E_ENEMY1, 1.0f), spawnPair(E_SCORPION, 0.5f), spawnPair(E_ENEMY2, 0.25f), spawnPair(E_SLIME, 0.4f) };
const static spawnVector tankRoomSpawns = { spawnPair(E_SAND_GOLEM, 1.0f) };
const static spawnVector blockRoomSpawns = { spawnPair(E_SCORPION, 1.0f), spawnPair(E_ARCHER, 0.6f), spawnPair(E_FIRE_MAGE, 0.55f) };
const static EntitySpecies boss = E_CURSED_CACTUS;

DesertDungeonGenerator::DesertDungeonGenerator(World *world)
	: DungeonGenerator(world, BiomeType::DESERT)
{
	roomXMin = 12; roomXMax = 20;
	roomYMin = 12; roomYMax = 20;
	bossRoomSize = 31;
}


void DesertDungeonGenerator::drawRoom(DNode* node) {
	Tile t = Tile::SAND;

	if (node->isSpawn) t = Tile::VOLCANO;
	if (node->isBoss) t = Tile::VOLCANO;

	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	bool wx, wy; //true on borders
	for (int x = X1; x <= X2; x++) {
		wx = ((x == X1) || (x == X2));
		for (int y = Y1; y <= Y2; y++) {
			wy = ((y == Y1) || (y == Y2));

			int xi = x + world->origin.x,
				yi = y + world->origin.y;

			if (wx || wy)
				world->tiles[xi][yi] = Tile::WALL;
			else
				world->tiles[xi][yi] = t;
		}
	}

	if (node->isBoss)
		drawBoss(node);
	else {
		if (!node->isSpawn) { // No enemies in spawn room
			float r = world->rand(0, 1);

			if (r <= 0.1f)	drawTankRoom(node);                // 0.1
			if (r > 0.1f && r <= 0.3f)  drawBarren(node);      // 0.2
			if (r > 0.3f && r <= 0.65f) drawBlocky(node);      // 0.35
			if (r > 0.65f) drawNormal(node);                   // 0.35
		}
	}
}

void DesertDungeonGenerator::drawBoss(DNode *node) {
#ifndef CLIENT
	bossSpawner = new BossSpawner(boss, node->BL + pi(node->size.x/2, node->size.y/2));
	bossSpawner->normalTile = Tile::SAND;
	bossSpawner->wallTile = Tile::WALL;
	world->spawners.push_back(bossSpawner);
#endif
}

void DesertDungeonGenerator::drawBlocky(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	for (int x = X1 + 2; x <= X2 - 2; x++) {
		for (int y = Y1 + 2; y <= Y2 - 2; y++) {
			if (world->rand(0, 1) < 0.08f) {
				auto t = (world->rand(0, 1) < 0.3f ? TLTileType::CACTUS : TLTileType::ROCK);
				world->addPermTopLayerTile(new TopLayerTile(pi(x, y), t));
			}
				//world->tiles[x + world->origin.x][y + world->origin.y] = Tile::WALL;
		}
	}

#ifndef CLIENT
	int tileNum = node->size.x * node->size.y;
	Spawner *spawn = new DungeonSpawner(&blockRoomSpawns,
		node->BL + pi(1, 1), node->BL + node->size - pi(2, 2),
		(int)(0.02f * tileNum));
	world->spawners.push_back(spawn);
#endif
}

void DesertDungeonGenerator::drawBarren(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			world->tiles[x + world->origin.x][y + world->origin.y] = Tile::DARK_SAND;

			if (x!=X1+1 && x!=X2-1 && y!=Y1+1 && y!=Y2-1 && world->rand(0, 1) < 0.01f) {
				world->addPermTopLayerTile(new TopLayerTile(pi(x, y), TLTileType::ROCK));
			}
		}
	}
}

void DesertDungeonGenerator::drawTankRoom(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	for (int x = X1+1; x <= X2-1; x++) {
		for (int y = Y1+1; y <= Y2-1; y++) {
			if (world->rand(0, 1) < 0.1f)
				world->tiles[x + world->origin.x][y + world->origin.y] = Tile::SNOW;

			if (world->rand(0, 1) < 0.05f) {
				world->addPermTopLayerTile(new TopLayerTile(pi(x, y), TLTileType::LAVA_POOL));
			}
		}
	}

	// Create spawner
#ifndef CLIENT
	int tileNum = node->size.x * node->size.y;
	int K = 2;
	if (tileNum > 200) K = 3;
	Spawner *spawn = new DungeonSpawner(&tankRoomSpawns,
		node->BL + pi(1, 1), node->BL + node->size - pi(2, 2), K, 90.0f);
	world->spawners.push_back(spawn);
#endif
}


void DesertDungeonGenerator::drawNormal(DNode *node) {
#ifndef CLIENT
	int tileNum = node->size.x * node->size.y;
	Spawner *spawn = new DungeonSpawner(&normalSpawns,
		node->BL + pi(1, 1), node->BL + node->size - pi(2, 2),
		(int)(0.03f * tileNum));
	world->spawners.push_back(spawn);
#endif
}
