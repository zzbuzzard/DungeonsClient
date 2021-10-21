#include "pch.h"
#include "MapGenerator.h"
#include "World.h"


MapGenerator::MapGenerator(World *world_) : world(world_) {

}

MapGenerator::~MapGenerator() {
}

// move right or up or down or diagonal, always choosing the option that minimises distance.
// shuffle the end result for a wobbly path.
std::vector<pi> MapGenerator::getPath(pi a, pi b) {
	if (a.x > b.x) {
		pi c = a;
		a = b;
		b = c;
	}

	std::vector<pi> path;

	pi on = a;

#define numD 5
	pi op[numD];
	pi disp[numD] = { pi(1,0), pi(1,1), pi(1,-1), pi(0,1), pi(0,-1) };

	while (on != b) {

		//cout << "R = " << R << endl;

		int ind = 0;
		float bestD = 10000000.0f;
		//float gp = 10000;
		for (int i = 0; i < numD; i++) {
			op[i] = on + disp[i];
			if (op[i] == b) {
				ind = i;
				break;
			}

			float d = util::dist((sf::Vector2f)op[i], (sf::Vector2f)b);

			if (d < bestD) {
				bestD = d;
				ind = i;
			}
		}

		on = op[ind];
		path.push_back(disp[ind]);
	}

	world->shuffle(path);
	on = a;
	for (int i = 0; i < (int)path.size(); i++) {
		pi disp = path[i];
		path[i] = on;
		on += disp;
	}
	path.push_back(on);

	return path;
}

class Entity;

void MapGenerator::initTiles(int a, int b) {
	world->tiles = std::vector<std::vector<Tile> >(a, std::vector<Tile>(b, Tile::NONE));
	world->topLayerTileArray = std::vector<std::vector<Entity*> >(a, std::vector<Entity*>(b, nullptr));;
}