#pragma once
#include "State.h"
#include "TextInput.h"
#include "Button.h"

class LoginState : public State
{
public:
	LoginState(sf::RenderWindow *window);
	void update();
	void draw(const sf::View *v);
	void resize(const sf::View *v);
	sf::Vector2f whereToCenter();

private:
	TextInput usernameText, passwordText;
	Button usernameBox, passwordBox;

	sf::Text loginText;

	//void resizeProg();
	//int prog;
	//sf::RectangleShape loadBox, loadBar;
	//sf::Text text;
};

