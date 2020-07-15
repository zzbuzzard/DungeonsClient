#include "pch.h"
#include "OverworldGenerator.h"
#include "World.h"
#include "Spawner.h"

struct OG_Node {
public:
	int depth;
	BiomeType biome;
	std::vector<OG_Node*> children;
	pi pos;
};

OverworldGenerator::OverworldGenerator(World *world) : MapGenerator(world) {

}

int counts[] = {
	3,3,3,1,       // 0
	3,3,2,2,       // 4
	3,3,2,1,1,     // 8
	3,2,2,2,1,     // 13
	3,2,2,1,1,1,   // 18
	3,3,1,1,1,1,   // 24
	2, 2, 2, 2, 1, 1 }; // 30

int indices[] = { 0,4,8,13,18,24,30 };
#define numIndices 7

#define edgeLengthLower 30
#define edgeLengthUpper 40

#define spawnRadiusLower 20
#define spawnRadiusUpper 30

#define edgeWidthLower 2
#define edgeWidthUpper 4

#define PAD_X 30
#define PAD_Y 30

static Tile overworldCenterTile = Tile::DIRT;


//     F(Node* myNode, leftAng, rightAng, int *dungeons)
//       (if we branch into 2, there must be at least 2 dungeons in dungeons. 
//        we then reduce dungeons by 1 before recursing so they know there's 1 reserved elsewhere)

bool OverworldGenerator::generate() {
	int ind = indices[world->randint(0, numIndices - 1)];
	std::vector<int> branchDungeons;
	int total = 0;
	for (int i = ind;; i++) {
		int x = counts[i];
		total += x;
		branchDungeons.push_back(x);
		if (total >= 10) break;
	}

	world->shuffle(branchDungeons);

	int branches = branchDungeons.size();

	cout << "Dungeon branches: ";
	for (int i : branchDungeons) {
		cout << i << " ";
	}
	cout << endl;

	std::vector<OG_Node> roots = std::vector<OG_Node>(branches);

	float L = 0;

	for (int i = 0; i < branches; i++) {
		BiomeType b = getBaseBiome();
		roots[i].biome = b;
		roots[i].depth = 0;

		float rad = world->rand(spawnRadiusLower, spawnRadiusUpper);
		float angSize = 2 * PI / 10 * branchDungeons[i];

		float angPos = L + world->rand(0.4f*angSize, 0.6f*angSize);

		sf::Vector2f posf = rad * sf::Vector2f(cos(angPos), sin(angPos));
		roots[i].pos = (pi)posf;

		int branchNum = branchDungeons[i];
		generateFrom(&roots[i], L, L + angSize, &branchNum, true);
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
	for (int i = 0; i < branches; i++) {
		std::vector<pi> path = getPath(roots[i].pos, roots[(i + 1) % branches].pos);
		border.insert(path.begin(), path.end());
	}

	world->fillWith(world->origin, overworldCenterTile, border);

	for (int i = 0; i < branches; i++) {
		drawFrom(&roots[i], roots[i].pos);
	}

	drawOutline();


	cout << "Finished overworld map generation" << endl;

	for (int i = 0; i < branches; i++) {
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


void OverworldGenerator::generateFrom(OG_Node *node, float left, float right, int *dungeons, bool last) {
	if (node->pos.x < bottomLeft.x) bottomLeft.x = node->pos.x;
	if (node->pos.y < bottomLeft.y) bottomLeft.y = node->pos.y;

	if (node->pos.x > topRight.x) topRight.x = node->pos.x;
	if (node->pos.y > topRight.y) topRight.y = node->pos.y;

	// Dungeon spawn chance
	// depth  1  => 0%
	// depth  2  => 0%
	// depth  3  => 100%
	// depth >3  => 100%
	if (!last && node->depth == 2) {
		//if (world->rand(0, 1) < 0.25f) {
			world->makeDungeon(node->pos, node->biome);
			return;
		//}
	}
	//if (!last && node->depth == 3) {
	//	if (world->rand(0, 1) < 0.65f) {
	//		world->makeDungeon(node->pos, node->biome);
	//		return;
	//	}
	//}
	if (node->depth > 2) {
		world->makeDungeon(node->pos, node->biome);
		return;
	}

	int minBranch = 1;
	int maxBranch = *dungeons;

	int c = world->randint(minBranch, maxBranch);
	if (last && node->depth == 3) c = maxBranch;

	*dungeons -= c - 1;

	//cout << "I am branching " << c << " times" << endl;

	// lets say they always pick 1 ; then there is only 1

	// divide my remaining space into C
	float angleEach = (right - left) / c;

	for (int i = 0; i < c; i++) {
		float ll = left + angleEach * i;
		float rr = left + angleEach * (i + 1);

		OG_Node *newNode = new OG_Node();
		node->children.push_back(newNode);

		float ang = ll + world->rand(angleEach*0.1, angleEach*0.9);
		float length = world->rand(edgeLengthLower, edgeLengthUpper);

		float myRad = util::mag((sf::Vector2f)node->pos);
		float rad = length + myRad;

		sf::Vector2f pos = sf::Vector2f(cos(ang), sin(ang)) * rad;

		newNode->pos = (pi)pos;
		newNode->biome = node->biome;
		newNode->depth = node->depth + 1;

		//cout << node->pos << " -> " << newNode->pos << endl;

		// they are last only if they're my last child AND I am last
		bool isL = (i == c - 1) && last;
		generateFrom(newNode, ll, rr, dungeons, isL);
	}
}

#define numBases 2
BiomeType bases[numBases] = {
	BiomeType::DESERT,
	BiomeType::GRASS,
// 	BiomeType::SNOW,
};
BiomeType OverworldGenerator::getBaseBiome() {
	return bases[world->randint(0, numBases - 1)];
}

pi OverworldGenerator::drawFrom(OG_Node *node, pi start) {
	pi closest(0, 0);
	float closestDist = 1000000;

	for (OG_Node *n : node->children) {
		BiomeType b = n->biome;
		//cout << "Getting path " << node->pos << " ... " << n->pos << endl;
		std::vector<pi> path = getPath(node->pos, n->pos);

		// Now we must inflate the path
		// It will be iterative.
		//  Edge is a vector of pis.
		//  Each turn we inflate it:
		//   Everything in Edges expands out to things that have background = wall
		//   These form the new Edge vector.

		Tile t = biomes[(int)b].biomeTile;

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
						if (ind.x >= 0 && ind.x < world->tiles.size()) {
							if (ind.y >= 0 && ind.y < world->tiles[0].size()) {

								currentTile = world->tiles[ind.x][ind.y];
								if (currentTile == Tile::NONE || currentTile == overworldCenterTile) {
									world->tiles[ind.x][ind.y] = t;
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

		Spawner *spawn = new OverworldSpawner(b, allPoints, end, maxDist);
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
	for (int i = 1; i < world->tiles.size()-1; i++) {
		for (int j = 1; j < world->tiles[0].size()-1; j++) {
			if (world->tiles[i][j] != Tile::NONE) continue;
			int8_t c = 0;
			for (int8_t k = 0; k < 4; k++) {
				Tile t = world->tiles[i + dirOffset[k].x][j + dirOffset[k].y];
				if (t != Tile::WALL && t != Tile::NONE) c++;
			}
			if (c > 0) world->tiles[i][j] = Tile::WALL;
		}
	}

	// Get rid of sharp edges
	std::vector<pi> p;
	for (int i = 1; i < world->tiles.size() - 1; i++) {
		for (int j = 1; j < world->tiles[0].size() - 1; j++) {
			if (world->tiles[i][j] != Tile::NONE) continue;
			int8_t c = 0;
			for (int8_t k = 0; k < 4; k++) {
				Tile t = world->tiles[i + dirOffset[k].x][j + dirOffset[k].y];
				if (t == Tile::WALL) c++;
			}
			if (c >= 2) p.push_back(pi(i, j));
		}
	}

	for (pi pp : p) world->tiles[pp.x][pp.y] = Tile::WALL;
}
