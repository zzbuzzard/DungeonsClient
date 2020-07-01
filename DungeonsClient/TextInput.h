#pragma once
#include "SFML/Graphics.hpp"

class State;

class TextInput
{
public:
	TextInput(int maxChars_);

	void update(State *state);
	void draw(sf::RenderWindow *window);

	void setPos(sf::Vector2f pos);
	void setFontSize(unsigned int size);
	void setFontColor(sf::Color col); // damn these american spellings
	void setOutlineColor(sf::Color col);
	void setOutline(float w);
	//void setFixedHeight(float h);

	void setFocus();
	void loseFocus();

	void setString(std::string string);
	void clear();
	std::string getString();

	bool isInFocus();
private:
	void recenter();
	void calculateSizes();

	std::string text;
	sf::Text rText;
	const int maxChars;

	float fixedHeight;

	bool movedCaret = false;
	int caretPos = 0; // Index where a new letter would appear if we were to type.
	sf::RectangleShape caret;

	float caretMoveCool;
};

