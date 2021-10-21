#pragma once
#include "MapGenerator.h"
#include "Biome.h"
struct DNode {
	DNode(pi size_);

	bool isBoss = false;
	bool isSpawn = false;
	pi size, BL;

	bool placed = false;

	std::vector<DNode*> children;
};

struct Rect {
	Rect(int x_, int y_, int w_, int h_);
	bool overlap(const Rect &ot) const;
	const int x, y, w, h;
};

class BossSpawner;

class DungeonGenerator : public MapGenerator
{
public:
	DungeonGenerator(World *world, DungeonType dungeonType_);
	bool generate();
	
	static bool anyOverlap(const Rect &a, const std::vector<Rect> &rects);
protected:
	DNode *bossRoom;
#ifndef CLIENT
	BossSpawner *bossSpawner = nullptr;
	std::vector<pi> bossRoomBlockOff;
#endif
	// Parameters for generate()'s default algorithm
	int maxWrongRooms = 6;
	int roomsMin = 25;
	int roomsMax = 30;

	int roomXMin = 9, roomXMax = 15, roomYMin = 9, roomYMax = 15;
	int roomPlaceRetries = 50;

	int bossRoomSize = 21;
	int spawnRoomX = 11, spawnRoomY = 11;

	int corridorLength = 5;
	int corridorThickness = 2;

	Tile wallTile = Tile::WALL;

	// Lower values (closer to 0) mean more likely to have 3-rooms
	// Higher values (closer to 1) mean more likely to have 4-rooms {but less deep}
	float orderConstant = 0.4f;

	// can be overriden if they really wanna
	virtual void generateTree(DNode* node, bool isRightWay, int weight);
	virtual void allocateSpace(DNode* node);
	virtual void drawRoomsFrom(DNode* node);

	// should definitely overwrite
	virtual void drawRoom(DNode* node);
private:
	pi getRandomRoomDim();

	void deallocateFrom(DNode *node);

	int minRoomOverlap;
	const DungeonType dungeonType;
	std::vector<Rect> rects;
	TileSpawnVector defaultTiles;

	//void generateFrom(Node *node, float left, float right, int *dungeons, bool last);
	//void drawFrom(Node *node);
	//BiomeType getBaseBiome();
};