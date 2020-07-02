#include "pch.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Media.h"
#include "State.h"
#include "Controller.h"
#include "LoadingState.h"
#include "ConnectionManager.h"

// Client controls
//   Left click - Target enemy
//   Right click - Target special
//   E - Enter dungeon
//   Enter - Open chat
//   Click another player - view gear etc.
//   Space - Use special
//   H - Show/hide inventory
//   1, 2, 3 - Change loadout

using std::cout;
using std::endl;

int main()
{
	Media::PreLoad();

	auto style = sf::Style::Default; // TODO: FULLSCREEN WHEN RELEASING : BUT WE ALSO NEED A QUIT BUTTON IN A MENU
	//auto style = sf::Style::Fullscreen;
	sf::RenderWindow window(sf::VideoMode(500, 500), "Dungeons", style);
	window.setVerticalSyncEnabled(true);
	sf::View view = sf::View(sf::Vector2f(0, 0), sf::Vector2f(VIEW_HEIGHT, VIEW_HEIGHT));
	float aspect = window.getSize().x / (float)window.getSize().y;
	view.setSize(VIEW_HEIGHT * aspect, VIEW_HEIGHT);

	window.setView(view);

	sf::Clock clock;

	g_state = new LoadingState(&window);

	int totFrames = 0;
	const float fpsPrintTime = 10;
	float fpsPrintTimeRemain = fpsPrintTime;

	// Main loop
	while (window.isOpen()) {
		deltaTime = clock.restart().asSeconds();
		totalTime += deltaTime;

		totFrames++;
		fpsPrintTimeRemain -= deltaTime;
		if (fpsPrintTimeRemain < 0) {
			float fps = totFrames / (fpsPrintTime - fpsPrintTimeRemain);
			cout << "FPS: " << fps << endl;
			totFrames = 0;
			fpsPrintTimeRemain = fpsPrintTime;
		}

		typedChar = 0;

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
				float aspect;
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				aspect = window.getSize().x / (float)window.getSize().y;
				view.setSize(VIEW_HEIGHT * aspect, VIEW_HEIGHT);
				window.setView(view);
				g_state->resize(&view);
				break;
			case sf::Event::GainedFocus:
				windowFocus = true;
				break;
			case sf::Event::LostFocus:
				windowFocus = false;
				break;
			case sf::Event::TextEntered:
				if (event.text.unicode < 128) {
					typedChar = event.text.unicode;
				}
				break;
			}
		}

		//if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))window.close();

		g_state->update();
		view.setCenter(g_state->whereToCenter());
		window.setView(view);
		g_state->draw(&view);
		window.display();

		// nullptr during loading screen
		if (connection != nullptr)
			connection->checkForUpdates();

	}

	return 0;
}

// Client loop:
// State update
// Check for updates on the connection