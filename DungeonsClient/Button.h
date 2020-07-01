#pragma once
#include "SFML/Graphics.hpp"
#include "Media.h"
#include "Types.h"

class State;

class Button
{
public:
	Button();
	void setPos(pf pos);
	//void move(sf::Vector2f pos);
	void setSize(pf size);
	//void setOrigin(sf::Vector2f origin);
	void setTexture(TEXTURE texture);
	void setTint(sf::Color col);
	void setText(std::string text);
	void setFontSize(unsigned int size);

	bool mouseIsOver(State *state);

	void setForegroundSize(pf size);
	void setForegroundTexture(TEXTURE texture);
	void enableForeground();
	void disableForeground();

	void draw(sf::RenderWindow *window);
private:
//public:
//	bool wasClicked;
private:
	sf::Vector2f size;
	sf::Vector2f pos;

	sf::RectangleShape box;

	bool hasForeground = false;
	sf::RectangleShape fg;

	bool hasText = false;
	sf::Text text;
};

