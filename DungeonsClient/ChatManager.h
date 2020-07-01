#pragma once
#include "SFML/Graphics.hpp"
#include "Util.h"
#include "TextInput.h"

class ChatMessage {
public:
	// TODO: Different constructors for diff message types
	ChatMessage(std::string message, sf::Color col = sf::Color::White);
	void draw(sf::RenderWindow *window);
	void setPos(pf pos);
	sf::Text text;
	float showTime;
};

class GameState;

class ChatManager
{
public:
	ChatManager();

	void update(GameState *state);

	void displayMessage(std::string sender, std::string message);
	void displaySystemMessage(std::string message, sf::Color col);
	void draw(sf::RenderWindow *window);

	void sendMessage();

	pf bottomLeft;
private:
	TextInput inputText;
	std::vector<ChatMessage> messages;
};

