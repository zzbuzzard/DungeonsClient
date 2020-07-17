#include "pch.h"
#include "State.h"

double totalTime = 0;
float deltaTime;

int webRequests, failedWebRequests;

#ifdef CLIENT
double serverLastHeard = 0;

float mouseHoldDuration;
bool mouseClicked, mouseReleased, mouseDown = false;
bool rightMouseClicked, rightMouseReleased, rightMouseDown;
char typedChar = 0;

bool isTyping = false;
TextInput *currentTextInput = nullptr;


State::State(sf::RenderWindow *window)
{
	this->window = window;
	resize(&window->getView());
}
#else
int dataSent;
int totalUpdateSends;
int totalFrames;
#endif

State::~State()
{
}

#ifdef CLIENT
void State::resize(const sf::View *v) {

}
#endif

void State::update() {

#ifdef CLIENT
	// mousedown currently represents last frame
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseDown) {
		mouseHoldDuration = 0;
		mouseClicked = true;
	}
	else
		mouseClicked = false;

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseDown)
		mouseReleased = true;
	else
		mouseReleased = false;

	mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	if (mouseDown) mouseHoldDuration += deltaTime;


	// Right mouse button
	if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !rightMouseDown) {
		rightMouseClicked = true;
	}
	else
		rightMouseClicked = false;

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Right) && rightMouseDown)
		rightMouseReleased = true;
	else
		rightMouseReleased = false;

	rightMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Right);
#endif
}

// mouse released: currently mouse down = false, previously mouse down = up