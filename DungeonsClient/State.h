#pragma once
#include "CSMode.h"
#ifdef CLIENT
#include <SFML/Graphics.hpp>
extern double serverLastHeard;

extern bool mouseClicked, mouseReleased, mouseDown;
extern bool rightMouseClicked, rightMouseReleased, rightMouseDown;
extern float mouseHoldDuration;
extern char typedChar;

class TextInput;
extern bool isTyping;
extern TextInput *currentTextInput;
#else
extern int dataSent;
extern int totalUpdateSends;
extern int totalFrames;
#endif

extern float deltaTime;
extern double totalTime;

class State
{
public:
	~State();

	virtual void update();

#ifdef CLIENT
	State(sf::RenderWindow *window);
	virtual void draw(const sf::View *v) = 0;
	virtual sf::Vector2f whereToCenter() = 0;
	virtual void resize(const sf::View *v);
	sf::Vector2i getMousePos() const {
		return sf::Mouse::getPosition(*window);
	}
	sf::Vector2f getScreenCenter() const {
		return ((sf::Vector2f)window->getSize()) * 0.5f;
	}
	sf::Vector2f getTopLeftWorld() const {
		return window->mapPixelToCoords(sf::Vector2i(0, 0));
	}
	sf::Vector2f getBottomRightWorld() const {
		return window->mapPixelToCoords((sf::Vector2i)window->getSize() - sf::Vector2i(1, 1));
	}
	sf::Vector2f getMousePosWorld() const {
		const sf::View &v = window->getView();
		return v.getCenter() - v.getSize() / 2.0f + (sf::Vector2f)getMousePos() * (v.getSize().x / window->getSize().x);
	}

private:

public:

protected:
	sf::RenderWindow *window;
#endif
};

