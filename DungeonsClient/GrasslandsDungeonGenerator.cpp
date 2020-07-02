#include "pch.h"
#include "GrasslandsDungeonGenerator.h"
#include "World.h"
#include "Spawner.h"
#include "TopLayerTile.h"

const static spawnVector plantRoomSpawns = { spawnPair(E_EYE_SPORE, 0.8f), spawnPair(E_MUNCHER, 1.0f), spawnPair(E_DARK_MUNCHER, 0.01f), spawnPair(E_LEAFLING, 0.5f) };
const static spawnVector entRoomSpawns = { spawnPair(E_ENT, 1.0f) };
const static spawnVector forestRoomSpawns = { spawnPair(E_ELF_KNIGHT, 0.8f), spawnPair(E_ELF_ARCHER, 1.0f), spawnPair(E_CURSED_ELF_KNIGHT, 0.005f) };
const static spawnVector mudRoomSpawns = { spawnPair(E_SLIME, 1.0f), spawnPair(E_SPIDER, 0.8f) };
const static EntitySpecies boss = E_MEGA_MUNCHER;

GrasslandsDungeonGenerator::GrasslandsDungeonGenerator(World *world)
	: DungeonGenerator(world, BiomeType::GRASS)
{
	spawnRoomX = 21; spawnRoomY = 21;
	roomXMin = 17; roomXMax = 24;
	roomYMin = 17; roomYMax = 24;

	bossRoomSize = 31; // If this is changed, make sure to update N for the maze generation in drawBoss()

	// TODO: REMOVE THESE (was 30..35 but thats a bit much maybe anyways - turn down and reduce maxWrong to 4)
	maxWrongRooms = 4;
	roomsMin = 25;
	roomsMax = 30;

	corridorLength = 10;

	wallTile = Tile::WALL_WOOD;
}


void GrasslandsDungeonGenerator::drawRoom(DNode* node) {
	// Do walls first
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
				world->tiles[xi][yi] = Tile::WALL_WOOD;
		}
	}

	if (node->isSpawn) {
		drawSpawn(node);
		return;
	}
	if (node->isBoss) {
		drawBoss(node);
		return;
	}

	float r = world->rand(0, 1);

	if (r <= 0.3f)
		drawPlantRoom(node);   // 0.3
	if (r > 0.3f && r <= 0.5f)
		drawEntRoom(node);     // 0.2
	if (r > 0.5f && r <= 0.7f)
		drawForestRoom(node);  // 0.2
	if (r > 0.7f && r <= 0.85f)
		drawClearing(node);    // 0.15
	if (r > 0.85f)
		drawMudRoom(node);     // 0.15
}

// right/down meaning can we move right/down
static struct BRNode {
	BRNode() : right(true), down(true) {
	}
	bool right, down;
};

typedef std::pair<int, bool> numDir;

#define N 10
int getWallCount(int x, int y, const BRNode nodes[N][N]) {
	const BRNode &n = nodes[x][y];
	int t = 0;
	if (!n.down) t++;
	if (!n.right) t++;

	if (x == 0 || !nodes[x - 1][y].right) {
		t++;
	}
	if (y == 0 || !nodes[x][y - 1].down) {
		t++;
	}
	return t;
}

#include <queue>
bool bfs(const BRNode nodes[N][N]) {
	bool vis[N][N];
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			vis[i][j] = false;
		}
	}
	vis[0][0] = true;

	std::queue<pi> Q;
	Q.push(pi(0, 0));
	int count = 1;
	while (Q.size()) {
		pi p = Q.front();
		Q.pop();

		const BRNode &x = nodes[p.x][p.y];
		if (x.right) {
			pi p2 = p + pi(1, 0);
			if (!vis[p2.x][p2.y]) {
				vis[p2.x][p2.y] = true;
				Q.push(p2);
				count++;
			}
		}
		if (x.down) {
			pi p2 = p + pi(0, 1);
			if (!vis[p2.x][p2.y]) {
				vis[p2.x][p2.y] = true;
				Q.push(p2);
				count++;
			}
		}

		if (p.x != 0 && nodes[p.x - 1][p.y].right) {
			pi p2 = p + pi(-1, 0);
			if (!vis[p2.x][p2.y]) {
				vis[p2.x][p2.y] = true;
				Q.push(p2);
				count++;
			}
		}

		if (p.y != 0 && nodes[p.x][p.y-1].down) {
			pi p2 = p + pi(0, -1);
			if (!vis[p2.x][p2.y]) {
				vis[p2.x][p2.y] = true;
				Q.push(p2);
				count++;
			}
		}
	}

	return count == N * N;
}

void GrasslandsDungeonGenerator::drawBoss(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	int xi, yi;
	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			xi = x + world->origin.x;
			yi = y + world->origin.y;
			float r = world->rand(0, 1);
			if (r <= 0.1f)
				world->tiles[xi][yi] = Tile::GRASS_DARK_BLOOD;
			if (r > 0.1f)
				world->tiles[xi][yi] = Tile::GRASS_DARK;
		}
	}

	// Generate a loopy maze
	// Store a list of 

	// There are 100 nodes. (10x10)
	// see #define N
	
	// 1) Make a 2d list of node classes
	BRNode nodes[N][N];

	// 2) Each one stores adjacency, right and down. Initialised to true.
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			nodes[i][j] = BRNode();

			if (i == N - 1) nodes[i][j].right = false;
			if (j == N - 1) nodes[i][j].down = false;
		}
	}
	// 3) Generate all edges : (a, D), 
	std::vector<numDir> edges;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			int me = i * 10 + j;
			if (i != N - 1) 
				// True means right
				edges.push_back(numDir(me, true));
			if (j != N - 1)
				// False means down
				edges.push_back(numDir(me, false));
		}
	}

	// 4) Shuffle the edges
	world->shuffle(edges);

	int t = 0;

	// 5) Loop through
	for (const numDir &nd : edges) {
		int x = nd.first / N;
		int y = nd.first % N;

		int c = getWallCount(x, y, nodes);
		if (c >= 2) continue;

		bool right = nd.second;

		if (right)
			c = getWallCount(x + 1, y, nodes);
		else
			c = getWallCount(x, y + 1, nodes);
		if (c >= 2) continue;

		if (right) nodes[x][y].right = false;
		else nodes[x][y].down = false;

		if (!bfs(nodes)) {
			if (right) nodes[x][y].right = true;
			else nodes[x][y].down = true;
		}
		else {
			t++;
		}
	}

	cout << "Boss room: Placed " << t << " edges" << endl;

	xi = X1 + world->origin.x;
	yi = Y1 + world->origin.y;

	int xx, yy;

	Tile mazeWalls = Tile::WALL_WOOD;

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			xx = xi + 3 * i;
			yy = yi + 3 * j;

			world->tiles[xx+3][yy] = mazeWalls;
			world->tiles[xx][yy+3] = mazeWalls;

			if (!nodes[i][j].right) {
				world->tiles[xx + 3][yy+1] = mazeWalls;
				world->tiles[xx + 3][yy+2] = mazeWalls;
			}
			if (!nodes[i][j].down) {
				world->tiles[xx + 1][yy + 3] = mazeWalls;
				world->tiles[xx + 2][yy + 3] = mazeWalls;
			}
		}
	}

	// Draw corners of each thing (each draws 2 bottom points)

	// A node is a 10x10 mother 

	pi center = pi(1 + xi + 3 * (N / 2), 1 + yi + 3 * (N / 2)) - world->origin;

#ifndef CLIENT
	bossSpawner = new BossSpawner(boss, center);
	bossSpawner->normalTile = Tile::GRASS;
	bossSpawner->wallTile = Tile::WALL_WOOD;
	world->spawners.push_back(bossSpawner);
#endif
}

void GrasslandsDungeonGenerator::drawSpawn(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	int xi, yi;
	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			xi = x + world->origin.x;
			yi = y + world->origin.y;
			if (x >= -1 && x <= 1 && y >= -1 && y <= 1)
				world->tiles[xi][yi] = Tile::GRASS2;
			else
				world->tiles[xi][yi] = Tile::GRASS_DARK;
		}
	}
}

void GrasslandsDungeonGenerator::drawPlantRoom(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	int xi, yi;
	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			xi = x + world->origin.x;
			yi = y + world->origin.y;
			if (world->rand(0, 1) < 0.1f)
				world->tiles[xi][yi] = Tile::GRASS2;
			else
				world->tiles[xi][yi] = Tile::GRASS;

			if (x != X1 + 1 && x != X2 - 1 && y != Y1 + 1 && y != Y2 - 1 && world->rand(0, 1) < 0.05f) {
				world->addPermTopLayerTile(new TopLayerTile(pi(x, y), TLTileType::BUSH));
			}
		}
	}


#ifndef CLIENT
	int tileNum = node->size.x * node->size.y;
	Spawner *spawn = new DungeonSpawner(&plantRoomSpawns,
		node->BL + pi(1, 1), node->BL + node->size - pi(2, 2),
		(int)(0.03f * tileNum));
	world->spawners.push_back(spawn);
#endif
}

void GrasslandsDungeonGenerator::drawEntRoom(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	int mx = (X1 + X2) / 2, my = (Y1 + Y2) / 2;

	int xi, yi;
	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			xi = x + world->origin.x;
			yi = y + world->origin.y;
			world->tiles[xi][yi] = Tile::GRASS_DARK;
		}
	}

	pi p;
	for (int i = 0; i < 5; i++) {
		p = pi(mx, my);
		for (int j = 0; j < 8; j++) {
			p += world->origin;
			if (world->rand(0, 1) < 0.25f)
				world->tiles[p.x][p.y] = Tile::GRASS_BLOOD;
			else
				world->tiles[p.x][p.y] = Tile::GRASS;
			p -= world->origin;

			int i = world->randint(0, 3);
			p += dirOffset[i];
			if (p.x <= X1 || p.x >= X2 || p.y <= Y1 || p.y >= Y2) break;
		}
	}


#ifndef CLIENT
	int tileNum = node->size.x * node->size.y;
	Spawner *spawn = new DungeonSpawner(&entRoomSpawns,
		pi(mx-1,my-1), pi(mx+1,my+1), 1, 300.0f);
	world->spawners.push_back(spawn);
#endif
}

void GrasslandsDungeonGenerator::drawForestRoom(DNode *node) {
	// Forest rooms must be quite large
	if (node->size.x * node->size.y < 400) {
		drawPlantRoom(node);
		return;
	}

	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	int mx = (X1 + X2) / 2, my = (Y1 + Y2) / 2;
	int Sx = node->size.x / 8, Sy = node->size.y / 8;

	int xi, yi;
	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			xi = x + world->origin.x;
			yi = y + world->origin.y;
			if (abs(x - mx) <= Sx && abs(y - my) <= Sy) {
				world->tiles[xi][yi] = Tile::WALL_WOOD;
			}
			else {
				if (world->rand(0, 1) < 0.05f)
					world->tiles[xi][yi] = Tile::GRASS2;
				else
					world->tiles[xi][yi] = Tile::GRASS;
			}
		}
	}

#ifndef CLIENT
	int tileNum = node->size.x * node->size.y;
	Spawner *spawn = new DungeonSpawner(&forestRoomSpawns,
		node->BL + pi(1, 1), node->BL + node->size - pi(2, 2),
		(int)(0.02f * tileNum));
	world->spawners.push_back(spawn);
#endif
}

void GrasslandsDungeonGenerator::drawClearing(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	int xi, yi;
	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			xi = x + world->origin.x;
			yi = y + world->origin.y;
			if (world->rand(0, 1) < 0.18f)
				world->tiles[xi][yi] = Tile::GRASS2;
			else
				world->tiles[xi][yi] = Tile::GRASS;
		}
	}
}

void GrasslandsDungeonGenerator::drawMudRoom(DNode *node) {
	int X1 = node->BL.x, Y1 = node->BL.y;
	int X2 = X1 + node->size.x - 1, Y2 = Y1 + node->size.y - 1;

	int xi, yi;
	for (int x = X1 + 1; x <= X2 - 1; x++) {
		for (int y = Y1 + 1; y <= Y2 - 1; y++) {
			xi = x + world->origin.x;
			yi = y + world->origin.y;
			world->tiles[xi][yi] = Tile::MUD;
		}
	}

#ifndef CLIENT
	int tileNum = node->size.x * node->size.y;
	Spawner *spawn = new DungeonSpawner(&mudRoomSpawns,
		node->BL + pi(1, 1), node->BL + node->size - pi(2, 2),
		(int)(0.04f * tileNum));
	world->spawners.push_back(spawn);
#endif
}
