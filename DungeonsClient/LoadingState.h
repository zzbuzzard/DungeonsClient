#pragma once
#include "State.h"

class LoadingState : public State
{
public:
	LoadingState(sf::RenderWindow *window);
	void update();
	void draw(const sf::View *v);
	void resize(const sf::View *v);
	sf::Vector2f whereToCenter();

private:
	void resizeProg();
	int prog;
	sf::RectangleShape loadBox, loadBar;
	sf::Text text;
};

