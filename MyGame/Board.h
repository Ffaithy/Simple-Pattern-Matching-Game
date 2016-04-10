#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <unordered_set>
#include <cstdint>

class Board
{
	enum class Direction
	{
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	class Cell
	{
	public:
		Cell() : x(0), y(0), type(EMPTY), selected(0), direction(Board::Direction::NONE) {};
		Cell(int type, int x, int y) : x(x), y(y), type(type), selected(false), explode(false), direction(Board::Direction::NONE){};

		void setSelected(bool selected) { this->selected = selected; }
		bool isSelected() const { return this->selected; }
		void setExplode(bool e) { this->explode = e; }
		bool isExplode() { return explode; }
		int getType() const { return type; }
		void setType(int type) { this->type = type; }
		void render() const;
		std::string getName() const { return getName(type); };
		int getX() const { return x; }
		int getY() const { return y; }
		void setX(int x) { this->x = x; }
		void setY(int y) { this->y = y; }
		Direction getDirection() const { return this->direction; }
		void setDirection(Direction dir) const { this->direction = dir; }

		static int getWidth() { return width; }
		static int getHeight() { return height;  }
		static void setWidth(int width) { width = width; }
		static void setHeight(int height) { height = height; }
		static std::string getName(int type) { return NAME_PREFIX + std::to_string(type) + NAME_SUFIX; }

		const static int DEFAULT_WIDTH;
		const static int DEFAULT_HEIGHT;
		const static std::string NAME_PREFIX; 
		const static std::string NAME_SUFIX;
		const static std::string SELECTOR;
		const static int EMPTY;

	private:
		mutable int type;
		int x;
		int y;
		bool selected;
		mutable bool explode;
		static int width;
		static int height;
		mutable Board::Direction direction;
	};

	enum class State
	{
		STATE_NONE,
		STATE_SWAP,
		STATE_EXPLODE,
		STATE_FILL
	};

public:
	Board(std::vector<int> probes, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);
	
	~Board() { probes.clear(); cells.clear(); }
	void generate();
	void initRenderer() const;
	void render();
	void handleInput(int x, int y);
	bool checkSwap();
	bool checkLine(int initX, int initY);
	bool checkCol(int initX, int initY);
	bool computeLine(int l);
	bool computeCol(int c);
	void update();

	//Animations
	void beginSwapAnimation();
	void SwapAnimation() const;
	void ComputeSwapCoordinates(const Cell* cell, int& x, int& y) const;
	void endSwap();
	bool checkForExplosion();

	void startFill();
	void animateFill();
	bool endFill();

private:
	const static int DEFAULT_WIDTH = 9;
	const static int DEFAULT_HEIGHT = 9;
	const static int NUM_MATCHES = 3;
	const static int OFFSET_Y;
	const static int OFFSET_X;

	const static int NUM_EXPLODE_STEPS = 30;
	const static uint8_t EXPLODE_DIFF_STEP = 8;
	const static uint8_t MAX_ALPHA = 255;
	const static uint8_t INIT_ALPHA = 240;

	static int STEP;
	static State STATE;
	
	//Swap animation
	const static int NUM_SWAP_STEPS = 30;
	const static int SWAP_DIFF_STEP = 2;

	const int width;
	const int height;
	std::vector<int> probes;
	std::vector<std::vector<Cell>> cells;
	const Cell* selected0;
	const Cell* selected1;
	std::unordered_set<int> linesToExplode;
	std::unordered_set<int> colsToExplode;
	std::vector<Cell> generator;
};

#endif