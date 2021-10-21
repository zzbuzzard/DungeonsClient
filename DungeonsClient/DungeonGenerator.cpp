#include "pch.h"
#include "DungeonGenerator.h"
#include "World.h"
#include <queue>

#ifndef CLIENT
#include "Spawner.h"
#endif

Rect::Rect(int x_, int y_, int w_, int h_) 
	: x(x_), y(y_), w(w_), h(h_)
{
}

// X=10, W=10
// OT.X=-5, OT.W=11

// if (x<ot.x) false

// okay so
// if (10 > -5 + 11 -1)

bool Rect::overlap(const Rect &ot) const {
	// x  ot.x   x+w
	if (x < ot.x) {
		if (ot.x > x + w - 1)
			return false;
	}
	else {
		if (x > ot.x + ot.w - 1)
			return false;
	}

	if (y < ot.y) {
		if (ot.y > y + h - 1)
			return false;
	}
	else {
		if (y > ot.y + ot.h - 1)
			return false;
	}

	return true;
}



DNode::DNode(pi size_) : size(size_) {

}


DungeonGenerator::DungeonGenerator(World *world, DungeonType dungeonType_)
	: MapGenerator(world), dungeonType(dungeonType_)
{
	minRoomOverlap = corridorThickness + 3;
	defaultTiles = dungeonTypeData[(int)dungeonType].defaultTiles;
}

void DungeonGenerator::generateTree(DNode* node, bool isRightWay, int weight) {
	if (weight == 1) {
		if (isRightWay) {
			node->isBoss = true;
			node->size = pi(bossRoomSize, bossRoomSize);
			bossRoom = node;
		}
		return;
	}
	if (weight < 1) {
		cout << "Weight < 1 UH OH" << endl;
		return;
	}
	weight--; // count ourselves

	int wrong = world->randint(0, maxWrongRooms);
	if (wrong >= weight) wrong = weight - 1;

	DNode *rightWay = new DNode(getRandomRoomDim());
	node->children.push_back(rightWay);
	generateTree(rightWay, isRightWay, weight - wrong);

	if (wrong > 0) {
		// Chance of a 4-room
		if (wrong > 1 && world->rand(0, 1) <= orderConstant) {
			int a = world->randint(1, wrong - 1);
			int b = wrong - a;

			DNode *L = new DNode(getRandomRoomDim());
			DNode *R = new DNode(getRandomRoomDim());

			node->children.push_back(L);
			node->children.push_back(R);

			generateTree(L, false, a);
			generateTree(R, false, b);
		}
		else {
			DNode *L = new DNode(getRandomRoomDim());
			node->children.push_back(L);

			generateTree(L, false, wrong);
		}
	}

	//cout << "This node has weight = " << weight << " and " << node->children.size() << " children" << endl;
}

const static spawnVector testSpawns = { spawnPair(E_ENEMY1, 1) };

void DungeonGenerator::allocateSpace(DNode* node) {
	pi TR = node->BL + node->size - pi(1, 1);
	if (node->BL.x < bottomLeft.x) bottomLeft.x = node->BL.x;
	if (node->BL.y < bottomLeft.y) bottomLeft.y = node->BL.y;
	if (TR.x > topRight.x) topRight.x = TR.x;
	if (TR.y > topRight.y) topRight.y = TR.y;

	rects.push_back(Rect(node->BL.x, node->BL.y, node->size.x, node->size.y));

	for (int i = 0; i < (int)node->children.size(); i++) {
		DNode *n = node->children[i];
		bool p = false;

		int X, Y;

		for (int j = 0; j < roomPlaceRetries; j++) {
			Dir d = (Dir)world->randint(0, 3);

			if (d == D_RIGHT) {
				X = TR.x + corridorLength + 1;

				int Ytop = TR.y - minRoomOverlap + 1;
				int Ybtm = node->BL.y + minRoomOverlap - n->size.y;

				Y = world->randint(Ybtm, Ytop);
			}
			if (d == D_UP) {
				Y = TR.y + corridorLength + 1;

				int Xtop = TR.x - minRoomOverlap + 1;
				int Xbtm = node->BL.x + minRoomOverlap - n->size.x;

				X = world->randint(Xbtm, Xtop);
			}
			if (d == D_LEFT) {
				X = node->BL.x - corridorLength - n->size.x;

				int Ytop = TR.y - minRoomOverlap + 1;
				int Ybtm = node->BL.y + minRoomOverlap - n->size.y;

				Y = world->randint(Ybtm, Ytop);
			}
			if (d == D_DOWN) {
				Y = node->BL.y - corridorLength - n->size.y;

				int Xtop = TR.x - minRoomOverlap + 1;
				int Xbtm = node->BL.x + minRoomOverlap - n->size.x;

				X = world->randint(Xbtm, Xtop);
			}

			Rect r(X, Y, n->size.x, n->size.y);

			if (!anyOverlap(r, rects)) {
				p = true;
				break;
			}
		}

		if (p) {
			n->BL = pi(X, Y);
			allocateSpace(n);
			n->placed = true;
		}
		else {
			cout << "Failed to place a node" << endl;
		}
	}
}

void DungeonGenerator::drawRoom(DNode *node) {
	//Tile t = biomes[(int)biome].biomeTile;

	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	//cout << "X1 = " << X1 << ", X2 = " << X2 << endl;
	//cout << "Y1 = " << Y1 << ", Y2 = " << Y2 << endl

	bool wx, wy; //true on borders
	for (int x = X1; x <= X2; x++) {
		wx = ((x == X1) || (x == X2));
		for (int y = Y1; y <= Y2; y++) {
			wy = ((y == Y1) || (y == Y2));

			int xi = x + world->origin.x,
				yi = y + world->origin.y;

			if (wx || wy) world->tiles[xi][yi] = wallTile;
			else world->tiles[xi][yi] = world->randomFromWeightedVector(defaultTiles);

		}
	}

#ifndef CLIENT
	if (!node->isSpawn && !node->isBoss && world->rand(0, 1.0f) < 0.6f) {
		int tileNum = node->size.x * node->size.y;
		Spawner *spawn = new DungeonSpawner(&testSpawns, node->BL + pi(1, 1), node->BL + node->size - pi(2, 2), (int)(0.05f * tileNum));
		world->spawners.push_back(spawn);
	}
#endif
}

void DungeonGenerator::drawRoomsFrom(DNode* node) {
	if (node->placed == false) return;
	//cout << "Drawing a node at pos = " << node->BL << endl;
	//cout << "(index = " << node->BL + world->origin << ")" << endl;

	//Tile t = biomes[(int)biome].biomeTile;

	drawRoom(node);

	for (DNode *n : node->children) {
		drawRoomsFrom(n);

		if (n->placed) {
			int X1 = node->BL.x, Y1 = node->BL.y;
			int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

			int oX1 = n->BL.x;
			int oX2 = oX1 + n->size.x - 1;

			int a = std::max(X1, oX1) + 1,
				b = std::min(X2, oX2) - 1;

			int overlap = b - a + 1;
			if (overlap >= corridorThickness) {
				// x + corridor - 1 <= b
				// x <= b - corridor + 1

				int X = world->randint(a, b - corridorThickness + 1);
				cout << a << "..." << b - corridorThickness + 1 << " : " << X << endl;
				//int X = b - corridorThickness + 1;

				// from the lower top to the upper bottom

				int y1 = std::min(node->BL.y + node->size.y, n->BL.y + n->size.y) - 1;
				int y2 = std::max(node->BL.y, n->BL.y);

				bool nIsY1 = (y1 == n->BL.y + n->size.y - 1);

				for (int y = y1; y <= y2; y++) {
					int yi = y + world->origin.y;
					for (int x = X; x < X + corridorThickness; x++) {
						world->tiles[x + world->origin.x][yi] = world->randomFromWeightedVector(defaultTiles);

#ifndef CLIENT
						if (n->isBoss && y == (nIsY1?y1:y2)) {
							bossRoomBlockOff.push_back(pi(x, y));
						}
#endif
					}

					world->tiles[X + world->origin.x - 1][yi] = wallTile;
					world->tiles[X + world->origin.x + corridorThickness][yi] = wallTile;
				}
			}

			else {
				int oY1 = n->BL.y;
				int oY2 = oY1 + n->size.y - 1;

				int a = std::max(Y1, oY1) + 1,
					b = std::min(Y2, oY2) - 1;

				int overlap = b - a + 1;
				if (overlap >= corridorThickness) {
					// x + corridor - 1 <= b
					// x <= b - corridor + 1

					int Y = world->randint(a, b - corridorThickness + 1);

					// from the lower top to the upper bottom

					int x1 = std::min(node->BL.x + node->size.x, n->BL.x + n->size.x) - 1;
					int x2 = std::max(node->BL.x, n->BL.x);

					bool nIsX1 = (x1 == n->BL.x + n->size.x - 1);

					for (int x = x1; x <= x2; x++) {
						int xi = x + world->origin.x;
						for (int y = Y; y < Y + corridorThickness; y++) {
							world->tiles[xi][y + world->origin.y] = world->randomFromWeightedVector(defaultTiles);

#ifndef CLIENT
							if (n->isBoss && x == (nIsX1?x1:x2)) {
								bossRoomBlockOff.push_back(pi(x, y));
							}
#endif
						}
						world->tiles[xi][Y + world->origin.y - 1] = wallTile;
						world->tiles[xi][Y + world->origin.y + corridorThickness] = wallTile;
					}
				}


			}

			// Draws the tree in (ish)
			//std::vector<pi> BRUH = getPath(node->BL, n->BL);
			//for (pi i : BRUH) {
			//	pi j = i + world->origin;
			//	world->tiles[j.x][j.y] = Tile::SNOW;
			//}
		}
	}
}


#define PAD_X 5
#define PAD_Y 5

bool DungeonGenerator::generate() {
	// 1) Generate tree
	// 2) AllocateSpace
	// 3) Set rooms (this is overriden)

	DNode *root = new DNode(pi(spawnRoomX, spawnRoomY));

	int numRooms = world->randint(roomsMin, roomsMax);
	cout << "Goal number of rooms = " << numRooms << endl;
	generateTree(root, true, numRooms);

	root->BL = pi(-spawnRoomX/2, -spawnRoomY/2);
	root->placed = true;
	root->isSpawn = true;
	allocateSpace(root);

	int X = topRight.x - bottomLeft.x + 1,
		Y = topRight.y - bottomLeft.y + 1;

	// Padding for dungeons at the edges
	X += PAD_X;
	Y += PAD_Y;

	cout << "X tiles = " << X << endl;
	cout << "Y tiles = " << Y << endl;

	cout << "Processing a map of size " << X * Y << endl;

	initTiles(X, Y);
	world->origin = -bottomLeft + pi(PAD_X / 2, PAD_Y / 2);

	drawRoomsFrom(root);
	bool success = bossRoom->placed;

	if (success) {
#ifndef CLIENT
		if (bossSpawner != nullptr)
			bossSpawner->wallPos = bossRoomBlockOff;
#endif
		// Excludes the wall border
		world->bossRoomBL = bossRoom->BL + pi(1, 1);
		world->bossRoomTR = bossRoom->BL + bossRoom->size - pi(2, 2);
	}

	deallocateFrom(root);
	return success;

	// root bottom left = (0, 0)

	//int X = 13;
	//int Y = 13;

	//initTiles(X, Y);

	//Tile t = biomes[(int)biome].biomeTile;

	//world->fillWith(pi(0, 0), t, {});
	//world->origin = pi(6, 6);

	//cout << "Finished test map generation" << endl;
}

pi DungeonGenerator::getRandomRoomDim() {
	return pi(
		world->randint(roomXMin, roomXMax),
		world->randint(roomYMin, roomYMax)
	);
}

bool DungeonGenerator::anyOverlap(const Rect &a, const std::vector<Rect> &rects) {
	for (const Rect &i : rects) {
		if (a.overlap(i)) {
			return true;
		}
	}
	return false;
}


void DungeonGenerator::deallocateFrom(DNode *node) {
	for (DNode *i : node->children)
		deallocateFrom(i);
	delete node;
}

