#ifndef __BOARD_H_
#define __BOARD_H_
#include <memory>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include "blocks.h"

class TetrisBoard {
private:
	int rows, cols;
	std::vector<std::vector<int>> board;
	std::shared_ptr<Block> falling;

public:
	const int DEFAULT_ROWS = 22;
	const int DEFAULT_COLS = 10;
	const float BLOCK_SIZE = 32.f;

	TetrisBoard() {
		new (this) TetrisBoard(DEFAULT_ROWS, DEFAULT_COLS);
	}

	TetrisBoard(int rows, int cols) : rows(rows), cols(cols),
		board(std::vector<std::vector<int>>(rows, std::vector<int>(cols, 0))) {}

	void render(sf::RenderWindow* window) {
		for (int r = 0; r < rows; ++r)
			for (int c = 0; c < cols; ++c) {
				if (board[r][c] > 0) {
					sf::RectangleShape shape(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
					shape.setPosition(sf::Vector2f(BLOCK_SIZE * c, BLOCK_SIZE * r));
					shape.setFillColor(sf::Color::Green);
					window->draw(shape);
				}
			}
		auto falling_shape = falling->getShape();
		for (int r = 0; r < falling_shape.size(); ++r) 
			for (int c = 0; c < falling_shape[0].size(); ++c) {
				if (falling_shape[r][c] > 0) {
					sf::RectangleShape shape(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
					shape.setPosition(sf::Vector2f(BLOCK_SIZE * (c + falling->getX()), BLOCK_SIZE * (r + falling->getY())));
					shape.setFillColor(sf::Color::Green);
					window->draw(shape);
				}
			}
	}

	void generateNewFalling() {
		falling = BlockFactory::generate(5, 0);
	}

	void hardDrop() {}
	void fall() { 
		if (!checkMove(Block::MOVE_DIR::DOWN)) {
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
	void move(Block::MOVE_DIR dir) { if (checkMove(dir)) falling->move(dir); }
	void rotate(Block::ROTATE_DIR dir) { if (checkRotate(dir)) falling->rotate(dir); }
	bool checkRotate(Block::ROTATE_DIR dir) {
		auto rotated = Block(*falling);
		rotated.rotate(dir);
		return checkBounds(rotated) && checkCollision(rotated);
	}

	bool checkMove(Block::MOVE_DIR dir) { 
		auto moved = Block(*falling);
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
		board = _board;
	}
};
#endif // __BOARD_H_