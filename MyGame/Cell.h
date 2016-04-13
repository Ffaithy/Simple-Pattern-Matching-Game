#ifndef CELL_H
#define CELL_H

#include <string>
#include <cstdint>
#include "AnimationManager.h"

class Cell
{
public:
	Cell() : x{ 0 }, y{ 0 }, type{ EMPTY }, selected{ 0 }, direction{ Direction::NONE }, mWidth{ DEFAULT_WIDTH }, mHeight{ DEFAULT_HEIGHT } {};
	Cell(int ptype, int px, int py) : x{ px }, y{ py }, type{ ptype }, selected{ false }, explode{ false }, direction{ Direction::NONE }, mWidth{ DEFAULT_WIDTH }, mHeight{ DEFAULT_HEIGHT }{};

	//getters
	bool isSelected() const { return this->selected; }
	bool isExplode() const { return explode; }
	int getType() const { return type; }
	std::string getName() const { return getName(type); };
	int getX() const { return x; }
	int getY() const { return y; }
	int getRow() const { return y / mWidth; }
	int getCol() const { return x / mHeight; }
	Direction getDirection() const { return this->direction; }
	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }

	static int getDefaultWidth() { return DEFAULT_WIDTH; }
	static int getDefaultHeight() { return DEFAULT_HEIGHT; }
	static std::string getName(int type) { return NAME_PREFIX + std::to_string(type) + NAME_SUFIX; }

	//setters
	void setSelected(bool selected) { this->selected = selected; }
	void setExplode(bool e) { this->explode = e; }
	void setType(int type) { this->type = type; }
	void setX(int x) { this->x = x; }
	void setY(int y) { this->y = y; }
	void setDirection(Direction dir) { this->direction = dir; }
	void setWidth(int pwidth) { mWidth = pwidth; }
	void setHeight(int pheight) { mHeight = pheight; }

	//Rendering methods
	void Cell::render(int step = 0, int numSteps = 0) const;
	void Cell::renderWithDynamicParams(int xSrc, int ySrc, int xDst, int yDst, int width, int height) const;

	const static int DEFAULT_WIDTH;
	const static int DEFAULT_HEIGHT;
	const static std::string NAME_PREFIX;
	const static std::string NAME_SUFIX;
	const static std::string SELECTOR;
	const static int EMPTY;

	const static int OFFSET_Y;
	const static int OFFSET_X;

	//Constant for alpha modifiers (necessary for the explosion)
	const static uint8_t MAX_ALPHA;
	const static uint8_t INIT_ALPHA;
	const static uint8_t ALPHA_DIFF_STEP;

private:
	int mWidth;
	int mHeight;
	mutable int type;
	int x;
	int y;
	bool selected;
	mutable bool explode;
	Direction direction;
};

#endif