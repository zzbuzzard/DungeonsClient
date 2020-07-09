#include "pch.h"
#include "Media.h"
#include "Controller.h"
#include "LoadingState.h"
#include "LoginState.h"

const float loadBarHeight = 200, xMargin = 100, yMargin = -200, textY = -150;

LoadingState::LoadingState(sf::RenderWindow *window) : State(window) {
	prog = 0;

	loadBox.setFillColor(sf::Color::Black);
	loadBar.setFillColor(sf::Color::Green);

	text.setCharacterSize(80);
	text.setFillColor(sf::Color::Black);
	text.setFont(g_pixelFont);

	resize(&window->getView());
	resizeProg();
}

void LoadingState::update() {
	Media::LoadNextTexture();
	prog++;
	if (prog == NUM_TEXTURES) {
		Media::FinishUp();

		sf::RenderWindow *w = window;
		deleteState();
		g_state = new LoginState(w);
		return;
	}
	resizeProg();
}

void LoadingState::resizeProg() {
	float m = prog / (float)NUM_TEXTURES;
	loadBar.setSize(sf::Vector2f(loadBox.getSize().x*m, loadBar.getSize().y));
	text.setString("Loading " + texturePaths[prog]);
	const auto &r = text.getLocalBounds();
	text.setOrigin(r.left + r.width / 2, r.top + r.height / 2);
}

void LoadingState::resize(const sf::View *v) {
	State::resize(v);
	loadBox.setSize(sf::Vector2f(v->getSize().x - 2 * xMargin, loadBarHeight));
	loadBox.setOrigin(loadBox.getSize() * 0.5f);
	loadBox.setPosition(0, v->getSize().y*0.5f + yMargin);

	loadBar.setSize(sf::Vector2f(0, loadBarHeight));
	loadBar.setOrigin(sf::Vector2f(0, loadBar.getSize().y*0.5f));
	loadBar.setPosition(-v->getSize().x*0.5f + xMargin, v->getSize().y*0.5f + yMargin);

	text.setPosition(loadBox.getPosition() + sf::Vector2f(0, textY));
}

void LoadingState::draw(const sf::View *v) {
	window->clear(sf::Color::White);
	window->draw(loadBox);
	window->draw(loadBar);
	window->draw(text);
}

sf::Vector2f LoadingState::whereToCenter() {
	return sf::Vector2f(0, 0);
}