#include "pch.h"
#include "ChatManager.h"
#include "Media.h"
#include "State.h"

// Send stuff
#include "SFML/Network.hpp"
#include "Packet.h"
#include "Controller.h"
#include "ConnectionManager.h"

	//sf::Text text;
	//pf pos;

static const int fontSize = 55;
static const float textSpacingY = 38;
static const int numMessages = 6;

static const float bottomPad = 10;
static const float leftPad = 10;

static const float messageTime = 20.0f;

ChatMessage::ChatMessage(std::string message, sf::Color col) {
	text.setFillColor(col);
	text.setOutlineColor(sf::Color::Black);
	text.setOutlineThickness(3.0f);
	text.setFont(g_pixelFont);
	text.setCharacterSize(fontSize);

	text.setString(message);

	showTime = messageTime;

	auto textRect = text.getLocalBounds();
	text.setOrigin(0, textRect.top + textRect.height);
}

void ChatMessage::draw(sf::RenderWindow *window) {
	window->draw(text);
}

void ChatMessage::setPos(pf pos) {
	text.setPosition(pos);
}

// ChatManager
ChatManager::ChatManager() : inputText(255) {
	inputText.setFontSize(fontSize);
	inputText.setFontColor(sf::Color::White);
	inputText.setOutlineColor(sf::Color::Black);
	inputText.setOutline(3.0f);
}


void ChatManager::displayMessage(std::string sender, std::string message) {
	messages.push_back(ChatMessage(sender + ": " + message));
}

void ChatManager::displaySystemMessage(std::string message, sf::Color col) {
	messages.push_back(ChatMessage(message, col));
}

void ChatManager::draw(sf::RenderWindow *window) {
	pf bl = bottomLeft + pf(leftPad, -bottomPad);

	inputText.setPos(bl + pf(0, -textSpacingY));
	inputText.draw(window);

	for (int i = 0; i < numMessages; i++) {
		bl.y -= textSpacingY;

		int ind = messages.size() - 1 - i;
		if (ind < 0) break;

		if (messages[ind].showTime >= 0) {
			messages[ind].setPos(bl);
			messages[ind].draw(window);
		}
	}
}

void ChatManager::sendMessage() {
	std::string message = inputText.getString();
	inputText.clear();

	inputText.loseFocus();

	if (message.size() == 0) return;

	sf::Packet p;
	p.append(&P_SEND_MSG, sizeof(Packet));

	msg_len_t len = message.size();
	p.append(&len, sizeof(msg_len_t));

	for (const char &c : message) {
		p.append(&c, sizeof(char));
	}

	cout << "Sending a message: \"" << message << "\"" << endl;
	connection->send(p);
}

bool sendButtonPressed = true;

void ChatManager::update(GameState *state) {
	inputText.update((State*)state);
	bool sendThis = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
		if (!sendButtonPressed) sendThis = true;
		sendButtonPressed = true;
	}
	else sendButtonPressed = false;

	if (sendThis) {
		if (inputText.isInFocus()) {
			cout << "Sending message" << endl;
			sendMessage();
		}
		else {
			cout << "Set focus" << endl;
			inputText.setFocus();
		}
	}

	for (auto it = messages.rbegin(); it != messages.rend(); ++it) {
		if (it->showTime > 0)
			it->showTime -= deltaTime;
		else
			break;
	}
}
