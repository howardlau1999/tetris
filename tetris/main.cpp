#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include "board.h"
int main() {
	sf::RenderWindow window(sf::VideoMode(32 * 10, 32 * 22), "SFML works!");
	sf::RectangleShape shape(sf::Vector2f(32.f, 32.f));
	TetrisBoard board;
	board.generateNewFalling();
	sf::Clock clock, key_delay;
	const sf::Time drop_interval = sf::milliseconds(250);
	const sf::Time key_interval = sf::milliseconds(100);
	while (window.isOpen()) {
		
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (key_delay.getElapsedTime() > key_interval) {
			key_delay.restart();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				board.move(Block::MOVE_DIR::LEFT);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				board.move(Block::MOVE_DIR::RIGHT);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				board.fall();
				clock.restart();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				board.rotate(Block::ROTATE_DIR::CLOCK);
				clock.restart();
			}
		}
		if (clock.getElapsedTime() > drop_interval) {
			clock.restart();
			board.fall();
		}

		window.clear();
		board.render(&window);
		window.display();
	}

	return 0;
}