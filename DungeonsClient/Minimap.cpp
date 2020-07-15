#include "pch.h"
#include "Minimap.h"
#include "Util.h"
#include "World.h"
#include "GameState.h"
#include "Enemy.h"
#include "Player.h"

static const float boxSize = 300, border = 25, zoomTime = 0.02f;
static const int viewChange = 20;

static const int minV = 16, maxV = 300;
static const float mul = 1.2f, minEnemyPlayerSize = 10.0f;

static const int W = 20, enemyVis = 10;

Minimap::Minimap() {

}

// other entities: 
//  a rectangle shape that we move about

Minimap::Minimap(int x, int y, int worldX, int worldY, const World *world) {
	sizeX = worldX; sizeY = worldY;
	shape.setSize(pf(boxSize, boxSize));

	shape.setOutlineThickness(4.0f);
	shape.setOutlineColor(sf::Color::White);

	image = sf::Image();
	image.create(worldX, worldY);

	w = W;
	view = 100;

	enemies.setFillColor(sf::Color::Red);
	players.setFillColor(sf::Color::Green);
}

void Minimap::update(GameState *state) {
	if (zoomCool > 0) zoomCool -= deltaTime;
	else {
		int l = (int)(view / mul), h = (int)(view * mul);

		if (l % 2 == 1)l--;
		if (h % 2 == 1)h++;

		if (h == view) h++;
		if (l == view) l--;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal) && l >= minV) {
			//view -= viewChange;
			view = l;
			zoomCool = zoomTime;
			changed = true;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Dash) && h <= maxV) {
			view = h;
			//view += viewChange;
			zoomCool = zoomTime;
			changed = true;
		}
	}
}


void Minimap::setPos(pf pos) {
	shape.setPosition(pos);
}

void Minimap::draw(GameState *state, sf::RenderWindow *window) {
	setPos(state->getBottomRightWorld() + pf(-boxSize - border, - VIEW_HEIGHT + border));

	int gap = view - w;

	if (changed) {
		pi o = state->currentWorld->origin;

		texture.loadFromImage(image);
		shape.setTexture(&texture);

		shape.setTextureRect(sf::IntRect(x+o.x - gap/2, y+o.y - gap/2, view, view));

		changed = false;
	}

	window->draw(shape);

	int tt = enemyVis;
	if (gap/2 -1 < enemyVis) tt = gap/2 - 1;

	int x1 = x - tt, x2 = x + w + tt;
	int y1 = y - tt, y2 = y + w + tt;
	float mUnit = boxSize / (float)view;

	pf shapePos = shape.getPosition();

	pf topLeft = pf(x, y) - pf((view - w) / 2, (view - w)/2);

	//float sizeThing = std::max(minEnemyPlayerSize, mUnit);

	for (auto it = state->idToEnemy.begin(); it != state->idToEnemy.end(); ++it) {
		Enemy *e = it->second;
		pi pos = e->getCollisionPos();
		if (pos.x >= x1 && pos.x <= x2 && pos.y >= y1 && pos.y <= y2) {
			pf displacement = e->getPosWorld()/(float)PIXEL_PER_UNIT - topLeft;

			enemies.setPosition(shapePos + mUnit * (pf)displacement);
			enemies.setSize(pf(
				std::max(minEnemyPlayerSize, mUnit * e->tileSize.x),
				std::max(minEnemyPlayerSize, mUnit * e->tileSize.y)));
			window->draw(enemies);
		}
	}

	x1 = x - gap / 2, x2 = x + w + gap / 2;
	y1 = y - gap / 2, y2 = y + w + gap / 2;

	players.setSize(pf(std::max(mUnit, minEnemyPlayerSize), std::max(mUnit, minEnemyPlayerSize)));

	for (auto it = state->idToPlayer.begin(); it != state->idToPlayer.end(); ++it) {
		Player *p = it->second;
		pi pos = p->getCollisionPos();
		if (pos.x >= x1 && pos.x <= x2 && pos.y >= y1 && pos.y <= y2) {
			pf displacement = p->getPosWorld() / (float)PIXEL_PER_UNIT - topLeft;

			players.setPosition(shapePos + mUnit * (pf)displacement);
			//players.setSize(pf(mUnit * p->tileSize.x, mUnit * e->tileSize.y));
			window->draw(players);
		}
	}

}

void Minimap::refresh(const World *world) {
	pi o = world->origin;

	for (int i = x; i < x + w; i++) {
		if (i+o.x < 0) continue;
		if (i+o.x >= sizeX) break;

		for (int j = y; j < y + w; j++) {
			if (j+o.y < 0) continue;
			if (j+o.y >= sizeY) break;

			Tile t = world->getTile(pi(i, j));
			image.setPixel(i+o.x, j+o.y, tileMinimapColour[(int)t]);
			//cout << "Setting some pixels" << endl;
		}
	}
	changed = true;
}

// TODO: Would be a lot faster if we just refreshed the edges.
//  However, as this only runs when the player moves, this is likely to be pretty fast anyhow.
void Minimap::adjust(int X, int Y, const World *world) {
	if (x != X - w/2 || y != Y - w/2) {
		x = X - w/2;
		y = Y - w/2;
		refresh(world);
	}
}





