#include "pch.h"
#include "TileBox.h"

#include <iostream>
using std::cout;
using std::endl;

TileBox::TileBox(int num_x, int num_y) {
	shapes = std::vector<std::vector<sf::RectangleShape>>(num_x, std::vector<sf::RectangleShape>(num_y));

	for (int i = 0; i < num_x; i++) {
		for (int j = 0; j < num_y; j++) {
			shapes[i][j].setSize(sf::Vector2f(PIXEL_PER_UNIT, PIXEL_PER_UNIT));
			shapes[i][j].setOrigin(shapes[i][j].getSize() * 0.5f);
			shapes[i][j].setPosition(sf::Vector2f(i, j) * (float)PIXEL_PER_UNIT);
		}
	}

	//bottomLeft = pi(0, 0);
	bottomLeftReal = pi(0, 0);
}


void TileBox::draw(sf::RenderWindow *window) {
	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes[0].size(); j++) {
			window->draw(shapes[i][j]);
		}
	}
}

void TileBox::setIndex(int x, int y, const World *world) {
	int realX = x + bottomLeftReal.x,
		realY = y + bottomLeftReal.y;

	Tile t = world->getTile(pi(realX, realY));

	shapes[x][y].setSize(sf::Vector2f(PIXEL_PER_UNIT, PIXEL_PER_UNIT));
	shapes[x][y].setOrigin(shapes[x][y].getSize() * 0.5f);

	if (t != Tile::NONE) {
		TEXTURE tex = tileTextures[static_cast<int>(t)];
		shapes[x][y].setTexture(&textures[tex]);
		shapes[x][y].setFillColor(sf::Color::White);
	}
	else {
		shapes[x][y].setFillColor(sf::Color::Black);
	}
	shapes[x][y].setPosition(sf::Vector2f(realX, realY) * (float)PIXEL_PER_UNIT);
}


void TileBox::setWorld(const World *world) {
	bottomLeftReal = pi(0, 0);

	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes[0].size(); j++) {
			setIndex(i, j, world);
		}
	}

	//bottomLeft = pi(0, 0);
}

void TileBox::adjust(int x1, int x2, int y1, int y2, const World *world) {
	// x1 is too small, so we need to shift left
	while (x1 < bottomLeftReal.x) {
		shiftLeft(world);
	}
	while (x2 > bottomLeftReal.x + (int)shapes.size() - 1) {
		shiftRight(world);
	}
	while (y1 < bottomLeftReal.y) {
		shiftUp(world);
	}
	while (y2 > bottomLeftReal.y + (int)shapes[0].size() - 1) {
		shiftDown(world);
	}
	//cout << "Queried range = " << x1 << "..." << x2 << endl;
	//cout << "My range = " << bottomLeftReal.x << "..." << bottomLeftReal.x + (int)shapes.size() - 1 << endl;
}

// xoo
// xoo ->
// xoo

void TileBox::shiftRight(const World *world) {
	bottomLeftReal.x++;

	shapes.erase(shapes.begin());
	shapes.push_back(std::vector<sf::RectangleShape>( shapes[0].size() ));
	for (int y = 0; y < shapes[0].size(); y++) {
		setIndex(shapes.size() - 1, y, world);
	}
}

void TileBox::shiftLeft(const World *world) {
	bottomLeftReal.x--;

	shapes.insert(shapes.begin(), std::vector<sf::RectangleShape>(shapes[0].size()));
	shapes.erase(shapes.begin() + shapes.size() - 1);

	for (int y = 0; y < shapes[0].size(); y++) {
		setIndex(0, y, world);
	}
}

void TileBox::shiftUp(const World *world) {
	// we need to delete the bottom
	// shapes[x][-1]
	bottomLeftReal.y--;
	for (int x = 0; x < shapes.size(); x++) {
		shapes[x].erase(shapes[x].begin() + shapes[x].size()-1);
		shapes[x].insert(shapes[x].begin(), sf::RectangleShape());

		setIndex(x, 0, world);
	}
}

void TileBox::shiftDown(const World *world) {
	// we need to delete the top
	// shapes[x][0]
	bottomLeftReal.y++;
	for (int x = 0; x < shapes.size(); x++) {
		shapes[x].erase(shapes[x].begin());
		shapes[x].push_back(sf::RectangleShape());

		setIndex(x, shapes[x].size()-1, world);
	}
}

void TileBox::refresh(const World *world) {
	for (int x = 0; x < shapes.size(); x++) {
		for (int y = 0; y < shapes[0].size(); y++) {
			setIndex(x, y, world);
		}
	}
}




// ENTIRE CLASS REWRITTEN WITH ANOTHER METHOD : BUT ABOUT THE SAME SPEED
// (INSTEAD OF CREATING NEW VECTORS WE REUSE THE SAME ONES USING POINTERS)
/*
#include "pch.h"
#include "TileBox.h"

#include <iostream>
using std::cout;
using std::endl;

TileBox::TileBox(int num_x, int num_y) {
	shapes = std::vector<std::vector<sf::RectangleShape>*>(num_x, nullptr);

	for (int i = 0; i < num_x; i++) {
		shapes[i] = new std::vector<sf::RectangleShape>(num_y);
	}

	for (int i = 0; i < num_x; i++) {
		for (int j = 0; j < num_y; j++) {
			(*shapes[i])[j].setSize(sf::Vector2f(PIXEL_PER_UNIT, PIXEL_PER_UNIT));
			(*shapes[i])[j].setOrigin((*shapes[i])[j].getSize() * 0.5f);
			(*shapes[i])[j].setPosition(sf::Vector2f(i, j) * (float)PIXEL_PER_UNIT);
		}
	}

	//bottomLeft = pi(0, 0);
	bottomLeftReal = pi(0, 0);
}


void TileBox::draw(sf::RenderWindow *window) {
	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes[0]->size(); j++) {
			window->draw((*shapes[i])[j]);
		}
	}
}

void TileBox::setIndex(int x, int y, const World &world) {
	int realX = x + bottomLeftReal.x,
		realY = y + bottomLeftReal.y;

	Tile t = world.getTile(pi(realX, realY));

	(*shapes[x])[y].setSize(sf::Vector2f(PIXEL_PER_UNIT, PIXEL_PER_UNIT));
	(*shapes[x])[y].setOrigin((*shapes[x])[y].getSize() * 0.5f);

	if (t != Tile::NONE) {
		TEXTURE tex = tileTextures[static_cast<int>(t)];
		(*shapes[x])[y].setTexture(&textures[tex]);
		(*shapes[x])[y].setFillColor(sf::Color::White);
	}
	else {
		(*shapes[x])[y].setFillColor(sf::Color::Black);
	}
	(*shapes[x])[y].setPosition(sf::Vector2f(realX, realY) * (float)PIXEL_PER_UNIT);
}


void TileBox::setWorld(const World &world) {
	bottomLeftReal = pi(0, 0);

	for (int i = 0; i < shapes.size(); i++) {
		for (int j = 0; j < shapes[0]->size(); j++) {
			setIndex(i, j, world);
		}
	}

	//bottomLeft = pi(0, 0);
}

void TileBox::adjust(int x1, int x2, int y1, int y2, const World &world) {
	// x1 is too small, so we need to shift left
	while (x1 < bottomLeftReal.x) {
		shiftLeft(world);
	}
	while (x2 > bottomLeftReal.x + (int)shapes.size() - 1) {
		shiftRight(world);
	}
	while (y1 < bottomLeftReal.y) {
		shiftUp(world);
	}
	while (y2 > bottomLeftReal.y + (int)shapes[0]->size() - 1) {
		shiftDown(world);
	}
	//cout << "Queried range = " << x1 << "..." << x2 << endl;
	//cout << "My range = " << bottomLeftReal.x << "..." << bottomLeftReal.x + (int)shapes.size() - 1 << endl;
}

// 53 fps

// xoo
// xoo ->
// xoo

void TileBox::shiftRight(const World &world) {
	bottomLeftReal.x++;

	auto pointer = shapes[0];

	shapes.erase(shapes.begin());
	shapes.push_back(pointer);
	for (int y = 0; y < (*shapes[0]).size(); y++) {
		setIndex(shapes.size() - 1, y, world);
	}
}

void TileBox::shiftLeft(const World &world) {
	bottomLeftReal.x--;

	auto pointer = shapes[shapes.size() - 1];
	shapes.insert(shapes.begin(), pointer);
	shapes.erase(shapes.begin() + shapes.size() - 1);

	for (int y = 0; y < (*shapes[0]).size(); y++) {
		setIndex(0, y, world);
	}
}

void TileBox::shiftUp(const World &world) {
	// we need to delete the bottom
	// shapes[x][-1]
	bottomLeftReal.y--;
	for (int x = 0; x < shapes.size(); x++) {
		shapes[x]->erase(shapes[x]->begin() + shapes[x]->size()-1);
		shapes[x]->insert(shapes[x]->begin(), sf::RectangleShape());

		setIndex(x, 0, world);
	}
}

void TileBox::shiftDown(const World &world) {
	// we need to delete the top
	// shapes[x][0]
	bottomLeftReal.y++;
	for (int x = 0; x < shapes.size(); x++) {
		shapes[x]->erase(shapes[x]->begin());
		shapes[x]->push_back(sf::RectangleShape());

		setIndex(x, shapes[x]->size()-1, world);
	}
}



// ooooooo
// ooooooo
// xoooooo

// The screen is moving right;
// ooooooo
// ooooooo
// oxooooo
//
//void TileBox::shiftRight(const World &world) {
//	int changeX = bottomLeft.x;
//	int realX = bottomLeftReal.x + shapes.size();
//
//	bottomLeft.x++;
//	bottomLeft.x %= shapes.size();
//	bottomLeftReal.x++;
//
//	for (int y = 0; y < shapes[0].size(); y++) {
//		int realY = y + bottomLeftReal.y;
//		setIndex(changeX, y, realX, realY, world);
//	}
//}
//
//// The screen is moving left;
//// ooooooo
//// ooooooo
//// oxooooo
//
//void TileBox::shiftLeft(const World &world) {
//	int realX = bottomLeftReal.x - 1;
//
//	bottomLeft.x--;
//	bottomLeft.x = (bottomLeft.x + shapes.size()) % shapes.size();
//
//	int changeX = bottomLeft.x;
//
//	bottomLeftReal.x--;
//
//	for (int y = 0; y < shapes[0].size(); y++) {
//		int realY = y + bottomLeftReal.y;
//		setIndex(changeX, y, realX, realY, world);
//	}
//}
//
//void TileBox::shiftUp(const World &world) {
//	int realY = bottomLeftReal.y - 1;
//
//	bottomLeft.y--;
//	bottomLeft.y = (bottomLeft.y + shapes[0].size()) % shapes[0].size();
//
//	int changeY = bottomLeft.y;
//
//	bottomLeftReal.y--;
//
//	for (int x = 0; x < shapes.size(); x++) {
//		int realX = x + bottomLeftReal.x;
//		setIndex(x, changeY, realX, realY, world);
//	}
//}
//
//void TileBox::shiftDown(const World &world) {
//	int changeY = bottomLeft.y;
//	int realY = bottomLeftReal.y + shapes[0].size();
//
//	bottomLeft.y++;
//	bottomLeft.y %= shapes[0].size();
//	bottomLeftReal.y++;
//
//	for (int x = 0; x < shapes.size(); x++) {
//		int realX = x + bottomLeftReal.x;
//		setIndex(x, changeY, realX, realY, world);
//	}
//}
//

*/
