#pragma once

#include <vector>
#include <memory>

class Block {
public:
	enum MOVE_DIR { LEFT, RIGHT, DOWN };
	enum ROTATE_DIR { CLOCK, ANTI_CLOCK };
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
	OBlock() : Block(2, 2, { { 1, 1 },{ 1, 1 } }) {};
};


class ZBlock : public Block {
public:
	ZBlock() : Block(2, 3, { { 1, 1, 0 },{ 0, 1, 1 } }) {};
};

class SBlock : public Block {
public:
	SBlock() : Block(2, 3, { { 0, 1, 1 },{ 1, 1, 0 } }) {};
};

class LBlock : public Block {
public:
	LBlock() : Block(3, 2, { { 1, 0 },{ 1, 0 },{ 1, 1 } }) {};
};

class JBlock : public Block {
public:
	JBlock() : Block(3, 2, { { 0, 1 },{ 0, 1 },{ 1, 1 } }) {};
};

class TBlock : public Block {
public:
	TBlock() : Block(2, 3, { { 0, 1 ,0 },{ 1, 1, 1 } }) {};
};

class BlockFactory {
public:
	static std::shared_ptr<Block> generate(int defaultX, int defaultY) {
		std::shared_ptr<Block> block;
		switch (rand() % 7) {
		case 0:
			block = std::make_shared<IBlock>();
			break;
		case 1:
			block = std::make_shared<LBlock>();
			break;
		case 2:
			block = std::make_shared<JBlock>();
			break;
		case 3:
			block = std::make_shared<OBlock>();
			break;
		case 4:
			block = std::make_shared<ZBlock>();
			break;
		case 5:
			block = std::make_shared<SBlock>();
			break;
		case 6:
			block = std::make_shared<TBlock>();
			break;
		default:
			break;
		}
		block->setX(defaultX);
		block->setY(defaultY);

		return block;
	}
};