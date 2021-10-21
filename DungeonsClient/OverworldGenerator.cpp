#include "pch.h"
#include "OverworldGenerator.h"
#include "World.h"
#include "Spawner.h"

struct OG_Node {
public:
	int depth;
	DungeonType dungeonType;
	std::vector<OG_Node*> children;
	pi pos;
};

OverworldGenerator::OverworldGenerator(World *world, WorldType worldType_) : MapGenerator(world), worldType(worldType_) {

}

//int counts[] = {
//	3,3,3,1,       // 0
//	3,3,2,2,       // 4
//	3,3,2,1,1,     // 8
//	3,2,2,2,1,     // 13
//	3,2,2,1,1,1,   // 18
//	3,3,1,1,1,1,   // 24
//	2, 2, 2, 2, 1, 1 }; // 30
//
//int indices[] = { 0,4,8,13,18,24,30 };
//#define numIndices 7

// was 30..40
#define edgeLengthLower 5
#define edgeLengthUpper 8

// was 20..30
#define spawnRadiusLower 12
#define spawnRadiusUpper 15

// was 2..4
#define edgeWidthLower 2
#define edgeWidthUpper 3

#define PAD_X 30
#define PAD_Y 30

// TODO_
//static Tile overworldCenterTile = Tile::DIRT;

bool OverworldGenerator::generate() {
	//int total = 0;
	//for (int i = ind;; i++) {
	//	int x = counts[i];
	//	total += x;
	//	branchDungeons.push_back(x);
	//	if (total >= 10) break;
	//}

	//world->shuffle(branchDungeons);

	//int branches = branchDungeons.size();

	//cout << "Dungeon branches: ";
	//for (int i : branchDungeons) {
	//	cout << i << " ";
	//}
	//cout << endl;

	const WorldTypeData &data = worldTypeData[(int)worldType];

	int dungeons = 4;

	std::vector<OG_Node> roots = std::vector<OG_Node>(dungeons);

	float L = world->rand(0, 2*PI);

	for (int i = 0; i < dungeons; i++) {
		DungeonType d = data.getRandomDungeon(world);

		//BiomeType b = getBaseBiome();
		roots[i].dungeonType = d;
		roots[i].depth = 0;

		float rad = world->rand(spawnRadiusLower, spawnRadiusUpper);
		float angSize = 2 * PI / dungeons;  // * branchDungeons[i];

		float angPos = L + world->rand(0.4f*angSize, 0.6f*angSize);

		sf::Vector2f posf = rad * sf::Vector2f(cos(angPos), sin(angPos));
		roots[i].pos = (pi)posf;

		generateFrom(&roots[i], L, L + angSize);
		L += angSize;

		//cout << "Branches unused = " << branchNum << endl;
	}

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

	std::set<pi, piComp> border;
	for (int i = 0; i < dungeons; i++) {
		std::vector<pi> path = getPath(roots[i].pos, roots[(i + 1) % dungeons].pos);
		border.insert(path.begin(), path.end());
	}

	// TODO2 Use all tile spawns
	world->fillWith(world->origin, worldTypeData[(int)worldType].tileSpawns[0].first, border);

	for (int i = 0; i < dungeons; i++) {
		drawFrom(&roots[i], roots[i].pos);
	}

	drawOutline();

	// Neat hash for checking if client and server are generating the same thing
	//T = 0;
	//Q = 1;
	//for (int i = 0; i < X; i++) {
	//	for (int j = 0; j < Y; j++) {
	//		//cout << Q << " * " << (int)(world->tiles[i][j]) << endl;
	//		T += (int)(world->tiles[i][j]) * Q;
	//		Q *= 11;
	//	}
	//}
	//cout << "Drawn outline T = " << T << endl;


	cout << "Finished overworld map generation" << endl;

	for (int i = 0; i < dungeons; i++) {
		deallocateFrom(&roots[i]);
	}

	return true; //cant fail

	// (0, 0) is actually at (0, 0) + (X, Y)

	// Spawn is a big circle. Center is at (0, 0)
	// Nodes are spawned 
}

// dont delete first caller as these are stored in the vector
void OverworldGenerator::deallocateFrom(OG_Node *node) {
	for (OG_Node* nodee : node->children) {
		deallocateFrom(nodee);
		delete nodee;
	}
}


void OverworldGenerator::generateFrom(OG_Node *node, float left, float right) {
	if (node->pos.x < bottomLeft.x) bottomLeft.x = node->pos.x;
	if (node->pos.y < bottomLeft.y) bottomLeft.y = node->pos.y;

	if (node->pos.x > topRight.x) topRight.x = node->pos.x;
	if (node->pos.y > topRight.y) topRight.y = node->pos.y;

	if (node->depth >= 8) {
		world->makeDungeon(node->pos, node->dungeonType);
		return;
	}

	//int minBranch = 1;
	//int maxBranch = *dungeons;

	//int c = world->randint(minBranch, maxBranch);
	//if (last && node->depth == 3) c = maxBranch;

	//*dungeons -= c - 1;

	//cout << "I am branching " << c << " times" << endl;

	// lets say they always pick 1 ; then there is only 1

	// divide my remaining space into C
	//float angleEach = (right - left) / c;

	float myang = util::ang((pf)node->pos);

	float ang2 = myang + world->rand(-0.2f, 0.2f);
	if (ang2 < left) ang2 = left;
	if (ang2 > right) ang2 = right;

	OG_Node *newNode = new OG_Node();

	float length = world->rand(edgeLengthLower, edgeLengthUpper);

	float myRad = util::mag((sf::Vector2f)node->pos);
	float rad = length + myRad;

	sf::Vector2f pos = sf::Vector2f(cos(ang2), sin(ang2)) * rad;
	newNode->pos = (pi)pos;
	newNode->dungeonType = node->dungeonType;
	newNode->depth = node->depth + 1;

	node->children.push_back(newNode);

	generateFrom(newNode, left, right);

	//for (int i = 0; i < c; i++) {
	//	float ll = left + angleEach * i;
	//	float rr = left + angleEach * (i + 1);

	//	node->children.push_back(newNode);

	//	float ang = ll + world->rand(angleEach*0.1, angleEach*0.9);

	//	newNode->pos = (pi)pos;
	//	newNode->biome = node->biome;
	//	newNode->depth = node->depth + 1;

	//	//cout << node->pos << " -> " << newNode->pos << endl;

	//	// they are last only if they're my last child AND I am last
	//	bool isL = (i == c - 1) && last;
	//	generateFrom(newNode, ll, rr, dungeons, isL);
	//}
}

pi OverworldGenerator::drawFrom(OG_Node *node, pi start) {
	pi closest(0, 0);
	float closestDist = 1000000;

	const tlTileSpawnVector &tltSpawns = dungeonTypeData[(int)node->dungeonType].overworldTlSpawns;
	const TileSpawnVector &tileVec = dungeonTypeData[(int)node->dungeonType].defaultTiles;

	// TODO2: We need a better system for this - as we can't just check for == centerTile
	Tile centerTile = worldTypeData[(int)worldType].tileSpawns[0].first;

	for (OG_Node *n : node->children) {

		//cout << "Getting path " << node->pos << " ... " << n->pos << endl;
		std::vector<pi> path = getPath(node->pos, n->pos);

		// Now we must inflate the path
		// It will be iterative.
		//  Edge is a vector of pis.
		//  Each turn we inflate it:
		//   Everything in Edges expands out to things that have background = wall
		//   These form the new Edge vector.

		//Tile t = biomes[(int)b].biomeTile;

#ifndef CLIENT
		std::vector<pi> allPoints = path;
#endif

		std::vector<pi> *Edge = &path;
		std::vector<pi> *newEdge = new std::vector<pi>();
		newEdge->reserve(path.size() * 2);

		bool deleteNewEdge = true;

		int count = world->randint(edgeWidthLower, edgeWidthUpper);

		for (int i = 0; i < count; i++) {
			newEdge->clear();
			Tile currentTile;
			pi p, ind;
			pi b;

			for (pi j : *Edge) {
				b = j;

				int repeats = 1;
				float r = world->rand(0, 1);
				if (r < 0.3f) repeats = 2;
				if (r < 0.075f) repeats = 3;

				for (int ii = 0; ii < repeats; ii++) {
					for (int i = 0; i < 4; i++) {
						p = b + dirOffset[i];
						ind = p + world->origin;
						if (ind.x >= 0 && ind.x < (pos_t)world->tiles.size()) {
							if (ind.y >= 0 && ind.y < (pos_t)world->tiles[0].size()) {

								currentTile = world->tiles[ind.x][ind.y];
								if (currentTile == Tile::NONE || currentTile == centerTile) {
									world->tiles[ind.x][ind.y] = world->randomFromWeightedVector(tileVec);

									if (tltSpawns.size() > 0) {
										TLTileType type = world->randomFromWeightedVector(tltSpawns);
										if (type != TLTileType::NONE) {
											world->addPermTopLayerTile(new TopLayerTile(pi(p.x, p.y), type));
										}
									}

									newEdge->push_back(p);

									b += dirOffset[i];

#ifndef CLIENT
									allPoints.push_back(p);
#endif
								}
							}
						}
					}
				}
			}
			auto pointa = Edge;
			Edge = newEdge;
			newEdge = pointa;
			deleteNewEdge = !deleteNewEdge;
		}
		//for (pi j : *Edge) {
		//	pi ind = j + world->origin;
		//	world->tiles[ind.x][ind.y] = Tile::WALL;
		//}
		if (deleteNewEdge)
			delete newEdge;
		else
			delete Edge;

		pi end = drawFrom(n, start);

		float dst = util::dist((pf)start, (pf)end);
		if (dst <= closestDist) {
			closestDist = dst;
			closest = end;
		}

#ifndef CLIENT
		float maxDist = util::dist((pf)start, (pf)end);

		Spawner *spawn = new OverworldSpawner(node->dungeonType, allPoints, end, maxDist);
		world->spawners.push_back(spawn);
#endif

		//for (pi i : path) {
		//	int x = i.x + origin.x;
		//	int y = i.y + origin.y;

		//	if (x >= 0 && x < tiles.size()) {
		//		if (y >= 0 && y < tiles[x].size()) {
		//			tiles[x][y] = t;
		//		}
		//	}
		//}
	}

	if (closest == pi(0, 0)) return node->pos;
	return closest;
}


void OverworldGenerator::drawOutline() {
	for (int i = 1; i < (int)world->tiles.size()-1; i++) {
		for (int j = 1; j < (int)world->tiles[0].size()-1; j++) {
			if (world->tiles[i][j] != Tile::NONE) continue;
			int8_t c = 0;
			for (int8_t k = 0; k < 4; k++) {
				Tile t = world->tiles[i + dirOffset[k].x][j + dirOffset[k].y];
				if (t != Tile::WALL && t != Tile::NONE) c++;
			}
			if (c > 0) {
				// This is where the difference between client/server is
				world->tiles[i][j] = Tile::WALL;
			}
		}
	}

	// Get rid of sharp edges
	std::vector<pi> p;
	for (int i = 1; i < (int)world->tiles.size() - 1; i++) {
		for (int j = 1; j < (int)world->tiles[0].size() - 1; j++) {
			if (world->tiles[i][j] != Tile::NONE) continue;
			int8_t c = 0;
			for (int8_t k = 0; k < 4; k++) {
				Tile t = world->tiles[i + dirOffset[k].x][j + dirOffset[k].y];
				if (t == Tile::WALL) c++;
			}
			if (c >= 2) p.push_back(pi(i, j));
		}
	}

	for (pi pp : p) {
		world->tiles[pp.x][pp.y] = Tile::WALL;
	}
}
