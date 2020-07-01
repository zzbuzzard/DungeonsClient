#pragma once
#include "Util.h"

class World;
class GameState;

// World : generateOverworld()
//  

class MapGenerator
{
public:
	MapGenerator(World *world);
	virtual ~MapGenerator();

	virtual bool generate() = 0;
	std::vector<pi> getPath(pi a, pi b);
protected:
	World *world;

	// bottom left of the bounding box of the entire map
	pi bottomLeft = pi(0, 0), topRight = pi(0, 0);

	void initTiles(int a, int b);
};