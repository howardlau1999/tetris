#ifndef __BOARD_H_
#define __BOARD_H_
#include <memory>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <SFML/Graphics.hpp>

class Block {
public:
	enum MOVE_DIR {LEFT, RIGHT, DOWN};
	enum ROTATE_DIR {CLOCK, ANTI_CLOCK};
	Block(int rows, int cols, std::vector<std::vector<int>> shape) : rows(rows),
		cols(cols), shape(shape) {}
	int getX() const { return pos_x; }
	int getY() const { return pos_y; }
	int getRows() const { return rows; }
	int getCols() const { return cols; }
	void setX(int x) { pos_x = x; }
	void setY(int y) { pos_y = y; }
	void move(MOVE_DIR dir) {
		switch (dir) {
		case Block::LEFT:
			--pos_x;
			break;
		case Block::RIGHT:
			++pos_x;
			break;
		case Block::DOWN:
			++pos_y;
			break;
		default:
			break;
		}
	}

	void rotate(ROTATE_DIR dir) {
		
		std::vector<std::vector<int>> _shape(cols, std::vector<int>(rows, 0));
		switch (dir) {
		case Block::CLOCK:
			for (int r = 0; r < rows; ++r)
				for (int c = 0; c < cols; ++c) {
					_shape[c][rows - r - 1] = shape[r][c];
				}
			break;
		case Block::ANTI_CLOCK:
			for (int r = 0; r < rows; ++r)
				for (int c = cols - 1; c >= 0; --c) {
					_shape[cols - c - 1][r] = shape[r][c];
				}
			break;
		default:
			break;
		}
		shape = _shape;
		std::swap(rows, cols);
	}
	const std::vector<std::vector<int>>& getShape() const { return shape; }
private:
	int pos_x, pos_y;
	int rows, cols;
	std::vector<std::vector<int>> shape;
};

class IBlock : public Block {
public:
	IBlock() : Block(1, 4, { { 1, 1, 1, 1 } }) {};
};

class OBlock : public Block {
public:
	OBlock() : Block(2, 2, { { 1, 1}, {1, 1 } }) {};
};


class ZBlock : public Block {
public:
	ZBlock() : Block(2, 3, { {1, 1, 0}, {0, 1, 1} }) {};
};

class SBlock : public Block {
public:
	SBlock() : Block(2, 3, { { 0, 1, 1 },{ 1, 1, 0 } }) {};
};

class LBlock : public Block {
public:
	LBlock() : Block(3, 2, { { 1, 0 },{ 1, 0 }, {1, 1} }) {};
};

class JBlock : public Block {
public:
	JBlock() : Block(3, 2, { { 0, 1 },{ 0, 1 },{ 1, 1 } }) {};
};

class TBlock : public Block {
public:
	TBlock() : Block(2, 3, { { 0, 1 ,0 }, { 1, 1, 1 } }) {};
};
class TetrisBoard {
private:
	int rows, cols;
	std::vector<std::vector<int>> board;
	Block* falling;

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
		if (falling) delete falling;
		switch (rand() % 7) {
		case 0:
			falling = new IBlock();
			break;
		case 1:
			falling = new LBlock();
			break;
		case 2:
			falling = new JBlock();
			break;
		case 3:
			falling = new OBlock();
			break;
		case 4:
			falling = new ZBlock();
			break;
		case 5:
			falling = new SBlock();
			break;
		case 6:
			falling = new TBlock();
			break;
		default:
			break;
		}
		falling->setX(5);
		falling->setY(0);
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
	void rotate(Block::ROTATE_DIR dir) { falling->rotate(dir); }
	bool checkMove(Block::MOVE_DIR dir) { 
		auto falling_shape = falling->getShape();
		auto x = falling->getX(); auto y = falling->getY();
		int dr = 0, dc = 0;
		switch (dir) {
		case Block::LEFT:
			if (x == 0) return false;
			dc = -1;
			break;
		case Block::RIGHT:
			if (x + falling->getCols() == this->cols) return false;
			dc = 1;
			break;
		case Block::DOWN:
			if (y + falling->getRows() == this->rows) return false;
			dr = 1;
			break;
		default:
			break;
		}
		for (int r = 0; r < falling_shape.size(); ++r)
			for (int c = 0; c < falling_shape[0].size(); ++c) 
				if (falling_shape[r][c] > 0 && board[y + dr + r][x + dc + c] > 0) return false;
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
			std::cout << rowCount << ", ";
			if (rowCount < cols) {
				_board[rows - counter - 1] = board[r];
				++counter;
			}
		}
		board = _board;
		std::cout << std::endl;
	}
};
#endif // __BOARD_H_