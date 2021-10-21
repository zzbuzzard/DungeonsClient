#include "pch.h"
#include "LoginState.h"
#include "Types.h"
#include "Controller.h"
#include "GameState.h"
#include "WebReq.h"
#include "ConnectionManager.h"

const static int fontSize = 60;
const static float boxW = 700, boxH = 70, tpad = 10, yoffset = -17,
	usery = -55, pwdy = 55;

static bool sent = false;

// Called on another thread
static void handleToken(ID_t reqID, std::string token) {
	connection->handleToken(token);
	if (!connection->tokenReady()) {
		// TODO: Error message
		cout << "Login failed" << endl;
		sent = false;
	}
}

LoginState::LoginState(sf::RenderWindow *window) : State(window), usernameText(12), passwordText(30) {

	loginText.setFillColor(sf::Color::White);
	loginText.setCharacterSize(100);
	loginText.setString("Log in");
	loginText.setFont(g_pixelFont);
	util::centerSfText(loginText);
	loginText.setPosition(pf(0, -250));

	usernameBox.setSize(pf(boxW, boxH));
	usernameBox.setPos(pf(0, usery));
	usernameBox.setTint(sf::Color(220, 220, 220));

	passwordBox.setSize(pf(boxW, boxH));
	passwordBox.setPos(pf(0, pwdy));
	passwordBox.setTint(sf::Color(220, 220, 220));

	usernameText.setFontSize(fontSize);
	usernameText.setFontColor(sf::Color::Black);

	usernameText.setPos(pf(-boxW/2 + tpad, usery - 15));

	passwordText.setFontSize(fontSize);
	passwordText.setFontColor(sf::Color::Black);

	passwordText.setPos(pf(-boxW/2 + tpad, pwdy - 15));

	passwordText.setPasswordMode();

	//usernameText.setOutlineColor(sf::Color::Black);
	//usernameText.setOutline(3.0f);
}

bool tabClicked = true;

//size_t callback(char* ptr, size_t size, size_t nmemb) {
//	cout << "Callback called..." << endl;
//	return size * nmemb;
//}


void LoginState::update() {
	if (sent) {
		if (connection->tokenReady()) {
			cout << "Loading GameState" << endl;
			sf::RenderWindow *w = window;
			deleteState();
			g_state = new GameState(GameStateType::Overworld, w);
			return;
		}
	}


	// Change focused text using TAB
	bool c = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) {
		if (!tabClicked) c = true;
		tabClicked = true;
	}
	else tabClicked = false;

	if (c) {
		bool a = usernameText.isInFocus();
		bool b = passwordText.isInFocus();

		if (!a) {
			usernameText.setFocus();
		}
		else {
			passwordText.setFocus();
		}
	}

	// Change focused text by clicking
	if (mouseClicked) {
		if (usernameBox.mouseIsOver(this)) {
			//passwordText.loseFocus();
			usernameText.setFocus();
		}
		if (passwordBox.mouseIsOver(this)) {
			//usernameText.loseFocus();
			passwordText.setFocus();
		}
	}

	usernameText.update(this);
	passwordText.update(this);


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !sent) {
		std::string username = usernameText.getString();
		std::string password = passwordText.getString();

		cout << "Sending web request" << endl;
		if (webReq.threadSend("get_token.php", "username=" + username + "&password=" + password, handleToken, (ID_t)0)) {
			connection->setUsername(username);
			sent = true;
		}
		else {
			cout << "Failed to send login request" << endl;
			sent = false; // TODO: remove and give an error on the screen
		}

		// Then we go to the ServerSelectState.
		// When we click one, we do
		//  connection->setServerIP(server IP)
		//  connection->connect()

		// [causing the program to hang]
		// Then we show loading until we receive P_SERVER_HELLO
		// Which is either a thumbs up or an error.
		// If error we say error
	}

	State::update();
}


void LoginState::draw(const sf::View *v) {
	window->clear(sf::Color::Black);
	
	window->draw(loginText);
	
	usernameBox.draw(window);
	usernameText.draw(window);

	passwordBox.draw(window);
	passwordText.draw(window);
}

void LoginState::resize(const sf::View *v) {

}

sf::Vector2f LoginState::whereToCenter() {
	return pf(0, 0);
}
