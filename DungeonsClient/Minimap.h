#pragma once
#include "Types.h"

class GameState;
class World;

// 1) An image, which is the pixels from the map. Added to every time we move, then the texture is updated. [only if there is acc a change]
// 2) There are also lots of little shapes for enemies, etc. 
//    These are drawn on top.
//    Every draw step, we loop through gamestate, get the enemy entities and draw those that are in a big square range
// 3) Every draw step, we set the rect's position, and update with getPosWorld the top left's world pos
//    We set the rectangle's TextureRect
//    Then we loop thru gamestate and draw those nearby. Use one RectangleShape which moves about and is drawn multiple times.
// - When new territory is found, adjust deals with it.

class Minimap
{
public:
	Minimap();
	Minimap(int x, int y, int worldX, int worldY, const World *world);
	void setPos(pf pos);
	void draw(GameState *state, sf::RenderWindow *window);
	void update(GameState *state);

	void refresh(const World *world);
	void adjust(int X, int Y, const World *world);
private:
public:
private:
	bool changed = true;

	int x, y; // coordinates on map of top left currently on screen
	int w;    // width of visible view
	int view; // pixels in view
	int sizeX, sizeY;

	sf::Image image;
	sf::Texture texture;
	sf::RectangleShape shape;

	sf::RectangleShape enemies;
	sf::RectangleShape players;

	float zoomCool = -1.0f;
};

