#ifndef __BOARD_H_
#define __BOARD_H_
#include <memory>
#include <vector>
#include <deque>
#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "blocks.h"

class TetrisBoard {
private:
	int rows, cols;
	std::vector<std::vector<int>> board;
	std::shared_ptr<Block> falling;
	int cleared_rows;

	sf::Font font;
	std::deque<std::shared_ptr<Block>> nextQueue;
public:
	const int DEFAULT_ROWS = 22;
	const int DEFAULT_COLS = 10;
	const float BLOCK_SIZE = 32.f;
	const float CANVAS_X_OFFSET = 0;
	const float CANVAS_Y_OFFSET = BLOCK_SIZE * 4;
	const int maxNexts = 1;
	TetrisBoard() {
		new (this) TetrisBoard(DEFAULT_ROWS, DEFAULT_COLS);
	}

	TetrisBoard(int rows, int cols) : rows(rows), cols(cols), cleared_rows(0),
		board(std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0))) {
		font.loadFromFile("D:\\arvo.ttf");
	}

	void drawRectangle(sf::RenderWindow* window, const int c, const int r, const bool absolute = false) {
		sf::RectangleShape shape(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
		shape.setPosition(sf::Vector2f(BLOCK_SIZE * c, BLOCK_SIZE * r));
		if (!absolute)
			shape.move(sf::Vector2f(CANVAS_X_OFFSET, CANVAS_Y_OFFSET));
		shape.setFillColor(sf::Color::Green);
		window->draw(shape);
	}

	void drawBlock(sf::RenderWindow* window, const Block& block, const int offsetX, const int offsetY, const bool absolute = false) {
		auto shape = block.getShape();
		for (int r = 0; r < shape.size(); ++r)
			for (int c = 0; c < shape[0].size(); ++c) {
				if (shape[r][c] > 0) {
					drawRectangle(window, c + offsetX, r + offsetY, absolute);
				}
			}
	}

	void render(sf::RenderWindow* window) {
		for (int r = 0; r < rows; ++r)
			for (int c = 0; c < cols; ++c) {
				if (board[r][c] > 0) {
					drawRectangle(window, c, r);
				}
			}
		
		drawBlock(window, *falling, falling->getX(), falling->getY());

		sf::Text scoreText;
		scoreText.setString("Cleared Rows: " + std::to_string(cleared_rows));
		scoreText.setFillColor(sf::Color::White);
		scoreText.setFont(font);
		window->draw(scoreText);

		sf::Text nextHint;
		nextHint.setString("NEXT: ");
		nextHint.setPosition(0, BLOCK_SIZE);
		nextHint.setFillColor(sf::Color::White);
		nextHint.setFont(font);
		window->draw(nextHint);

		drawBlock(window, *nextQueue.front(), 3, 1, true);
	}

	void generateNewFalling() {
		while (nextQueue.size() < maxNexts + 1)
			nextQueue.push_back(BlockFactory::generate(5, 0));
		falling = nextQueue.front();
		nextQueue.pop_front();
		
	}

	void hardDrop() {}

	void fall() { 
		if (!checkMove(Block::MOVE_DIR::DOWN, *falling)) {
			if (falling->getY() == 0) {
				new (this) TetrisBoard(DEFAULT_ROWS, DEFAULT_COLS);
				return;
			}
			this->fix();
			this->clearRows();
			this->generateNewFalling();
		} else {
			falling->move(Block::MOVE_DIR::DOWN);
		}
	}

	void fix() {
		auto falling_shape = falling->getShape();
		auto x = falling->getX(); auto y = falling->getY();
		for (int r = 0; r < falling_shape.size(); ++r)
			for (int c = 0; c < falling_shape[0].size(); ++c)
				if (!board[y + r][x + c])
					board[y + r][x + c] = falling_shape[r][c];
	}

	void move(Block::MOVE_DIR dir) { 
		if (checkMove(dir, *falling)) { 
			falling->move(dir); 
		} 
	}

	void rotate(Block::ROTATE_DIR dir) { 
		if (checkRotate(dir, *falling)) { 
			falling->rotate(dir); 
		}  else {
			auto rotated = std::make_shared<Block>(*falling);
			rotated->rotate(dir);
			
			// If it's too left
			if (rotated->getX() < 0) {
				rotated->setX(0);
			} // Too right
			else if (rotated->getX() + rotated->getCols() > this->cols) {
				rotated->setX(this->cols - rotated->getCols() );
			}

			if (!checkCollision(*rotated)) return;

			falling = rotated;
		}
	}

	bool checkRotate(Block::ROTATE_DIR dir, const Block& block) {
		auto rotated = Block(block);
		rotated.rotate(dir);
		return checkBounds(rotated) && checkCollision(rotated);
	}

	bool checkMove(Block::MOVE_DIR dir, const Block& block) { 
		auto moved = Block(block);
		moved.move(dir);
		return checkBounds(moved) && checkCollision(moved);
	}

	bool checkBounds(const Block& block) {
		if (block.getX() < 0 || block.getX() + block.getCols() > this->cols) return false;
		if (block.getY() < 0 || block.getY() + block.getRows() > this->rows) return false;
		return true;
	}

	bool checkCollision(const Block& block) {
		auto const& shape = block.getShape();
		const int x = block.getX();
		const int y = block.getY();
		for (int r = 0; r < shape.size(); ++r)
			for (int c = 0; c < shape[r].size(); ++c)
				if (shape[r][c] > 0 && board[y + r][x + c] > 0) return false;
		return true;
	}

	void clearRows() {
		auto _board = std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0));
		int counter = 0;
		for (int r = rows - 1; r >= 0; --r) {
			int rowCount = 0;
			for (int c = 0; c < cols; ++c) {
				if (board[r][c] > 0) ++rowCount;
			}
			if (rowCount < cols) {
				_board[rows - counter - 1] = board[r];
				++counter;
			}
		}
		cleared_rows += rows - counter;
		board = _board;
	}
};
#endif // __BOARD_H_