#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <unordered_set>
#include "board.h"
int main() {
	sf::RenderWindow window(sf::VideoMode(32 * 10, 32 * 26), "SFML works!");
	sf::RectangleShape shape(sf::Vector2f(32.f, 32.f));
	TetrisBoard board;
	board.generateNewFalling();
	sf::Clock clock, key_delay, scan_timer;
	const sf::Time drop_interval = sf::milliseconds(250);
	const sf::Time key_interval = sf::milliseconds(100);
	const sf::Time scan_interval = sf::milliseconds(50);

	auto last_key = sf::Keyboard::Unknown;
	auto pressed_key = sf::Keyboard::Unknown;
	static const std::unordered_set<sf::Keyboard::Key> keys = { sf::Keyboard::Left, sf::Keyboard::Right, sf::Keyboard::Down, sf::Keyboard::Up};
	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		if (scan_timer.getElapsedTime() > scan_interval) {
			bool key_pressed = false;
			for (auto const& key : keys) {
				if (sf::Keyboard::isKeyPressed(key)) {
					pressed_key = key;
					key_pressed = true;
					break;
				}
			}
			if (!key_pressed) pressed_key = sf::Keyboard::Unknown;
			scan_timer.restart();
		}
		
		
		if (key_delay.getElapsedTime() > key_interval || (last_key != pressed_key && pressed_key != sf::Keyboard::Unknown)) {
			key_delay.restart();
			
			switch (pressed_key) {
			case sf::Keyboard::Left:
				board.move(Block::MOVE_DIR::LEFT);
				break;
			case sf::Keyboard::Right:
				board.move(Block::MOVE_DIR::RIGHT);
				break;
			case sf::Keyboard::Down:
				board.fall();
				clock.restart();
				break;
			case sf::Keyboard::Up:
				if (pressed_key == last_key) break;
				board.rotate(Block::ROTATE_DIR::CLOCK);
				break;
			default:
				break;
			}

			last_key = pressed_key;
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