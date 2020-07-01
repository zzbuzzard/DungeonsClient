#pragma once
#include <vector>
#include "SFML/Graphics.hpp"
#include "Util.h"
#include "World.h"

class TileBox
{
public:
	TileBox(int num_x, int num_y);
	void draw(sf::RenderWindow *window);
	void setWorld(const World *world);

	void refresh(const World *world);

	void adjust(int x1, int x2, int y1, int y2, const World *world);
private:
	std::vector<std::vector<sf::RectangleShape>> shapes;
	//pi bottomLeft;
	pi bottomLeftReal;

	void shiftRight(const World *world);
	void shiftLeft(const World *world);
	void shiftUp(const World *world);
	void shiftDown(const World *world);

	void setIndex(int x, int y, const World *world);
};


